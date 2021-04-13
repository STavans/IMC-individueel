#ifndef TALKING_CLOCK_H
#define TALKING_CLOCK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TALKING_CLOCK_MAX_STRING 40
#define TALKING_CLOCK_ITEMS 21

#define TALKING_CLOCK_ITSNOW_INDEX 0
#define TALKING_CLOCK_HOUR_INDEX 1
#define TALKING_CLOCK_AND_INDEX 2
#define TALKING_CLOCK_1_INDEX 3
#define TALKING_CLOCK_2_INDEX 4
#define TALKING_CLOCK_3_INDEX 5
#define TALKING_CLOCK_4_INDEX 6
#define TALKING_CLOCK_5_INDEX 7
#define TALKING_CLOCK_6_INDEX 8
#define TALKING_CLOCK_7_INDEX 9
#define TALKING_CLOCK_8_INDEX 10
#define TALKING_CLOCK_9_INDEX 11
#define TALKING_CLOCK_10_INDEX 12
#define TALKING_CLOCK_11_INDEX 13
#define TALKING_CLOCK_12_INDEX 14
#define TALKING_CLOCK_13_INDEX 15
#define TALKING_CLOCK_14_INDEX 16
#define TALKING_CLOCK_20_INDEX 17
#define TALKING_CLOCK_30_INDEX 18
#define TALKING_CLOCK_40_INDEX 19
#define TALKING_CLOCK_50_INDEX 20
static char talkingClock_files[TALKING_CLOCK_ITEMS][TALKING_CLOCK_MAX_STRING] = {
	"/sdcard/clock/itsnow.mp3",
	"/sdcard/clock/hour.mp3",
	"/sdcard/clock/en.mp3",
	"/sdcard/clock/1.mp3",
	"/sdcard/clock/2.mp3",
	"/sdcard/clock/3.mp3",
	"/sdcard/clock/4.mp3",
	"/sdcard/clock/5.mp3",
	"/sdcard/clock/6.mp3",
	"/sdcard/clock/7.mp3",
	"/sdcard/clock/8.mp3",
	"/sdcard/clock/9.mp3",
	"/sdcard/clock/10.mp3",
	"/sdcard/clock/11.mp3",
	"/sdcard/clock/12.mp3",
	"/sdcard/clock/13.mp3",
	"/sdcard/clock/14.mp3",
	"/sdcard/clock/20.mp3",
	"/sdcard/clock/30.mp3",
	"/sdcard/clock/40.mp3",
	"/sdcard/clock/50.mp3"
};

esp_err_t talkingClock_fillQueue();

#ifdef __cplusplus
}
#endif

#endif  // TALKING_CLOCK_H
