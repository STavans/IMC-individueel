/* Play an MP3 file from HTTP

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "http_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0))
#include "esp_netif.h"
#else
#include "tcpip_adapter.h"
#endif

#include "radioController.h"

// Checks wheter the channel is corrupted and/or stopped
static void update();
// Stops the radio from playing (does not close pipelines, streams, etc.)
static void reset();
// Updates the http stream
static int httpStreamEventHandle(http_stream_event_msg_t*);

static const char *TAG = "HTTP_MP3_EXAMPLE";

static int running = 0;
static int isInit = 0;
static int isPlaying = 0;

// Semaphore for the radio task
SemaphoreHandle_t radioMutex;

// Audio variables
audio_pipeline_handle_t pipeline;
audio_element_handle_t httpStreamReader, i2sStreamWriter, mp3Decoder;
audio_event_iface_handle_t evt;
esp_periph_set_handle_t set;

void radio_switch(char channel[])
{    
    if (!isInit)
        return;

    char* ip = " ";

    xSemaphoreTake(radioMutex, portMAX_DELAY);

    if (strcmp(channel, "538") == 0)
        ip = "https://21253.live.streamtheworld.com/RADIO538.mp3";
    else if(strcmp(channel, "Qmusic") == 0)
        ip = "https://icecast-qmusicnl-cdp.triple-it.nl/Qmusic_nl_live_96.mp3";
    else if (strcmp(channel, "SKY") == 0)
        ip = "https://19993.live.streamtheworld.com/SKYRADIO.mp3";

    if (strcmp(ip, " ") != 0)
    {
        // Stop the current playing (if playing) and start the new one

        if (isPlaying)
            reset();
        audio_element_set_uri(httpStreamReader, ip);
        audio_pipeline_run(pipeline);
        isPlaying = 1;
    }

    xSemaphoreGive(radioMutex);
}

void radio_task(void *p)
{
    radioMutex = xSemaphoreCreateMutex();
    running = 1;
    radio_init();

    while (running)
    {
        if (isPlaying)
        {
            xSemaphoreTake(radioMutex, portMAX_DELAY);
            update();
            xSemaphoreGive(radioMutex);
        }
        
        vTaskDelay(50 / portTICK_RATE_MS);
    }
    
    radio_stop();
    vTaskDelete(NULL);
}

void radio_quit()
{
    xSemaphoreTake(radioMutex, portMAX_DELAY);
    running = 0;
    xSemaphoreGive(radioMutex);
}

static void update()
{
    audio_event_iface_msg_t msg;
    esp_err_t ret = audio_event_iface_listen(evt, &msg, 200);
    if (ret != ESP_OK)
        return;

    if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
        && msg.source == (void *) mp3Decoder
        && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) 
    {
        audio_element_info_t music_info = {0};
        audio_element_getinfo(mp3Decoder, &music_info);

        ESP_LOGI(TAG, "[ * ] Receive music info from mp3 decoder, sample_rates=%d, bits=%d, ch=%d",
                    music_info.sample_rates, music_info.bits, music_info.channels);

        audio_element_setinfo(i2sStreamWriter, &music_info);
        i2s_stream_set_clk(i2sStreamWriter, music_info.sample_rates, music_info.bits, music_info.channels);
        return;
    }

    /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
    if ((int)msg.data == AEL_STATUS_STATE_FINISHED) 
    {
        ESP_LOGW(TAG, "[ * ] Stop event received");
        reset();
    }
}

void radio_init()
{
    if (running)
        xSemaphoreTake(radioMutex, portMAX_DELAY);

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) 
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0))
    ESP_ERROR_CHECK(esp_netif_init());
	tcpip_adapter_init();
#else
    tcpip_adapter_init();
#endif

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "[2.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipelineCfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipelineCfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[2.1] Create http stream to read data");
    http_stream_cfg_t httpCfg = HTTP_STREAM_CFG_DEFAULT();
    httpCfg.event_handle = httpStreamEventHandle;
    httpCfg.type = AUDIO_STREAM_READER;
    httpCfg.enable_playlist_parser = true;
    httpStreamReader = http_stream_init(&httpCfg);

    ESP_LOGI(TAG, "[2.2] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2sCfg = I2S_STREAM_CFG_DEFAULT();
    i2sCfg.type = AUDIO_STREAM_WRITER;
    i2sStreamWriter = i2s_stream_init(&i2sCfg);

    ESP_LOGI(TAG, "[2.3] Create mp3 decoder to decode mp3 file");
    mp3_decoder_cfg_t mp3Cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3Decoder = mp3_decoder_init(&mp3Cfg);

    ESP_LOGI(TAG, "[2.4] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, httpStreamReader, "http");
    audio_pipeline_register(pipeline, mp3Decoder,        "mp3");
    audio_pipeline_register(pipeline, i2sStreamWriter,  "i2s");

    ESP_LOGI(TAG, "[2.5] Link it together http_stream-->mp3_decoder-->i2s_stream-->[codec_chip]");
    const char *linkTag[3] = {"http", "mp3", "i2s"};
    audio_pipeline_link(pipeline, &linkTag[0], 3);

    ESP_LOGI(TAG, "[2.6] Set up  uri (http as http_stream, mp3 as mp3 decoder, and default output is i2s)");
    audio_element_set_uri(httpStreamReader, "https://icecast-qmusicnl-cdp.triple-it.nl/Qmusic_nl_live_96.mp3");
    
    static int isWifiInit = 0;
    ESP_LOGI(TAG, "[ 3 ] Start and wait for Wi-Fi network");
    static esp_periph_config_t periphCfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    set = esp_periph_set_init(&periphCfg);
    periph_wifi_cfg_t wifiCfg = {
        .ssid = CONFIG_WIFI_SSID,
        .password = CONFIG_WIFI_PASSWORD,
    };

    ESP_LOGI(TAG, "[ 3.2 ] Start and wait for Wi-Fi network");
    static esp_periph_handle_t wifiHandle;
    wifiHandle = periph_wifi_init(&wifiCfg);
    ESP_LOGI(TAG, "[ 3.3 ] Start and wait for Wi-Fi network");
    if (!isWifiInit)
    {
        esp_periph_start(set, wifiHandle);
        ESP_LOGI(TAG, "[ 3.4 ] Start and wait for Wi-Fi network");
        periph_wifi_wait_for_connected(wifiHandle, portMAX_DELAY);
        isWifiInit = 1;
    }

    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evtCfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evtCfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    isInit = 1;

    if (running)
        xSemaphoreGive(radioMutex);
}

void radio_reset()
{
    xSemaphoreTake(radioMutex, portMAX_DELAY);
    reset();
    xSemaphoreGive(radioMutex);
}

static void reset()
{
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_element_reset_state(mp3Decoder);
    audio_element_reset_state(i2sStreamWriter);
    audio_pipeline_reset_ringbuffer(pipeline);
    audio_pipeline_reset_items_state(pipeline);
    isPlaying = 0;
}

void radio_stop()
{
    ESP_LOGI(TAG, "[ 6 ] Stop audio_pipeline");
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    /* Terminate the pipeline before removing the listener */
    audio_pipeline_unregister(pipeline, httpStreamReader);
    audio_pipeline_unregister(pipeline, i2sStreamWriter);
    audio_pipeline_unregister(pipeline, mp3Decoder);

    audio_pipeline_remove_listener(pipeline);

    /* Stop all peripherals before removing the listener */
    esp_periph_set_stop_all(set);
    audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(httpStreamReader);
    audio_element_deinit(i2sStreamWriter);
    audio_element_deinit(mp3Decoder);

    isPlaying = 0;
    isInit = 0;
    ESP_LOGI(TAG, "[ 7 ] Finished");
}

static int httpStreamEventHandle(http_stream_event_msg_t *msg)
{
    if (msg->event_id == HTTP_STREAM_RESOLVE_ALL_TRACKS) 
        return ESP_OK;

    if (msg->event_id == HTTP_STREAM_FINISH_TRACK) 
        return http_stream_next_track(msg->el);
    if (msg->event_id == HTTP_STREAM_FINISH_PLAYLIST) 
        return http_stream_fetch_again(msg->el);
    return ESP_OK;
}