#include "lcd-menu.c"
#include "radioController.h"
#include "sdcard-mp3.h"
#include "talking_clock.h"
#include <sys/time.h>
#include "goertzel.h"
#include "animations.h"

// Clock timer
TimerHandle_t timer_1_sec;
void time1SecCallback(TimerHandle_t xTimer);

/*
This file is to work out the onClicks, onExit, onEnter and update functions of the lcd-menu's. 

(This file is an extension of the lcd-menu.c file, so that is why there is a ".c" file included)
*/

//Main menu
void onClickMainEcho(i2c_lcd1602_info_t* lcdInfo)
{
    displayMenu(lcdInfo, ECHO_MENU_ID);
}

void onClickMainRadio(i2c_lcd1602_info_t* lcdInfo)
{
    displayMenu(lcdInfo, RADIO_MENU_ID);
}

void onClickMainClock(i2c_lcd1602_info_t* lcdInfo)
{
    displayMenu(lcdInfo, CLOCK_MENU_ID);
}

//Echo menu
void onClickEchoSpeech(i2c_lcd1602_info_t* lcdInfo)
{
    displayMenu(lcdInfo, SPEECH_MENU_ID);
}

//Radio menu
void onEnterRadio()
{
    printf("Entered the radio menu\n");
    xTaskCreate(&radio_task, "radio_task", 1024 * 4, NULL, 8, NULL);
}

void onExitRadio()
{
    printf("Exited the radio menu\n");
    radio_quit();
}

void onClickRadio538()
{
    radio_switch(lcdMenus[RADIO_MENU_ID].items[0].text);
}

void onClickRadioQ()
{
    radio_switch(lcdMenus[RADIO_MENU_ID].items[1].text);
}

void onClickRadioSky()
{
    radio_switch(lcdMenus[RADIO_MENU_ID].items[2].text);
} 

//Klok menu
void onEnterClock()
{
    printf("Entered the clock menu\n");
    xTaskCreate(&mp3_task, "radio_task", 1024 * 3, NULL, 8, NULL);

    vTaskDelay(1000 / portTICK_RATE_MS);

    talkingClock_fillQueue();

    static int timeIsInit = 0;
    if (timeIsInit)
    return;
    
    timeIsInit = 1;

    // This timer will update every second, this way the system time will get updated
    timer_1_sec = xTimerCreate("MyTimer", pdMS_TO_TICKS(1000), pdTRUE, (void *) 1, &time1SecCallback);
    if (xTimerStart(timer_1_sec, 10) != pdPASS)
        printf("Cannot start 1 second timer\n");
}

void onExitClock()
{
    printf("Exited the clock menu\n");
    mp3_stopTask();
}

void onUpdateClock(void *p)
{
    strcpy(lcdMenus[CLOCK_MENU_ID].items[0].text, (char*) p);
}

void onClickClockItem()
{
    talkingClock_fillQueue();
}

//Speech menu
void onEnterSpeech()
{
    goertzel_start();
}

void onUpdateSpeech(void *p)
{
    strcpy(lcdMenus[SPEECH_MENU_ID].items[0].text, (char*) p);
}

void onExitSpeech()
{
    goertzel_stop();
}

void time1SecCallback(TimerHandle_t xTimer)
{
    // Print current time to the screen
    time_t now;
    struct tm timeinfo;
    time(&now);

    char strftime_buf[20];
    localtime_r(&now, &timeinfo);
    sprintf(&strftime_buf[0], "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    if (currentLcdMenu == CLOCK_MENU_ID)
        menu_updateMenu(menu_getLcdInfo(), (void*) strftime_buf);
}

void onClickAnimation(i2c_lcd1602_info_t* lcdInfo)
{
    displayMenu(lcdInfo, ANIMATION_MENU_ID);
}

void onClickFancy()
{
    setChoice("FANCY");
}

void onClickExcl()
{
    setChoice("!!!!");
}

void onClickDALT()
{
    setChoice("DALT");
}