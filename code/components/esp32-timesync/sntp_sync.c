#include "sntp_sync.h"
#include <string.h>
#include <time.h>
	
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sntp.h"

static const char *TAG = "SNTP";


static void timesync_obtainTime(void);
static void timesync_initializeSntp(void);


void timesync_sntpSync(sntp_sync_time_cb_t callback) 
{	
	// Set callback for when time synchronisation is done
	sntp_set_time_sync_notification_cb(callback);
	// Set timezone to Amsterdam Time
    setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
    tzset();
	
	time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
	
	
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) 
    {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        timesync_obtainTime();
        // update 'now' variable with current time
        time(&now);
    }

}


// Obtain SNTP time
static void timesync_obtainTime(void)
{
    timesync_initializeSntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retryCount = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retryCount) 
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retryCount);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    // Update the time
    time(&now);
    localtime_r(&now, &timeinfo);

}

// Set synchronisation settings
static void timesync_initializeSntp(void) 
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "0.nl.pool.ntp.org");
    
    sntp_init();
}