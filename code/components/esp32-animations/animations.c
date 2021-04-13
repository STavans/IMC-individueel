#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"
#include "lcd-menu-elaboration.h"
#include "animations.h"
#include "exclamation.h"

void doAnimation(i2c_lcd1602_info_t*, char*);
void doFancyAnimation(i2c_lcd1602_info_t*);
void doWaveAnimation(i2c_lcd1602_info_t*);
void doExclamationMarkAnimation(i2c_lcd1602_info_t*);
void doAlternateAnimation(i2c_lcd1602_info_t*);
void doDoubleAlternateAnimation(i2c_lcd1602_info_t*);

//This function calls the appropriate animation according to the given choice attribute.
void doAnimation(i2c_lcd1602_info_t* lcdInfo, char* choice)
{
    if (strcmp(choice, "FANCY") == 0)
    {
        doFancyAnimation(lcdInfo);   
    }else if (strcmp (choice, "WAVE") == 0)
    {
        doWaveAnimation(lcdInfo);
    }else if (strcmp (choice, "!!!!") == 0)
    {
        doExclamationMarkAnimation(lcdInfo);
    }else if (strcmp (choice, "ALT") == 0)
    {
        doAlternateAnimation(lcdInfo);
    }else if(strcmp (choice, "DALT") == 0){
        doDoubleAlternateAnimation(lcdInfo);
    }
}

//Plays a fancy animation when called
void doFancyAnimation(i2c_lcd1602_info_t* lcdInfo)
{
    i2c_lcd1602_move_cursor(lcdInfo, 0, 0);
    i2c_lcd1602_set_right_to_left(lcdInfo);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(30 / portTICK_RATE_MS);
    }
    i2c_lcd1602_move_cursor(lcdInfo, 19, 1);
    i2c_lcd1602_set_left_to_right(lcdInfo);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(30 / portTICK_RATE_MS);
    }
    i2c_lcd1602_move_cursor(lcdInfo, 0, 2);
    i2c_lcd1602_set_right_to_left(lcdInfo);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(30 / portTICK_RATE_MS);
    }
    i2c_lcd1602_move_cursor(lcdInfo, 19, 3);
    i2c_lcd1602_set_left_to_right(lcdInfo);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(30 / portTICK_RATE_MS);
    }
    vTaskDelay(100 / portTICK_RATE_MS);
}

//Plays a wave animation when called
void doWaveAnimation(i2c_lcd1602_info_t* lcdInfo)
{   
    i2c_lcd1602_move_cursor(lcdInfo, 0, 0);
    i2c_lcd1602_set_left_to_right(lcdInfo);
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            i2c_lcd1602_move_cursor(lcdInfo, i, j);
            i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
            vTaskDelay(15 / portTICK_RATE_MS);
        }
    }
    vTaskDelay(100 / portTICK_RATE_MS);
}

//Plays the exlamantion animation when called
void doExclamationMarkAnimation(i2c_lcd1602_info_t* lcdInfo)
{
    //Alternates between 2 different version of the exclamation mark with a slight delay in between (methods found in exclamation.c).
    doExclamationMarkFilled(lcdInfo);
    vTaskDelay(15 / portTICK_RATE_MS);
    doExclamationMarkEmpty(lcdInfo);
    vTaskDelay(15 / portTICK_RATE_MS);
    doExclamationMarkFilled(lcdInfo);
    vTaskDelay(15 / portTICK_RATE_MS);
    doExclamationMarkEmpty(lcdInfo);
    vTaskDelay(15 / portTICK_RATE_MS);
    doExclamationMarkFilled(lcdInfo);
    vTaskDelay(15 / portTICK_RATE_MS);
    doExclamationMarkEmpty(lcdInfo);
    vTaskDelay(100 / portTICK_RATE_MS);
}

//In this animation the even numbered lines come from the right to left and the odd numbered lines from left to right (all at the same time).
void doAlternateAnimation(i2c_lcd1602_info_t* lcdInfo)
{
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_set_right_to_left(lcdInfo);
        i2c_lcd1602_move_cursor(lcdInfo, i, 0);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        i2c_lcd1602_move_cursor(lcdInfo, i, 2);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);

        i2c_lcd1602_set_left_to_right(lcdInfo);
        i2c_lcd1602_move_cursor(lcdInfo, 19 - i, 1);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        i2c_lcd1602_move_cursor(lcdInfo, 19 - i, 3);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);

        vTaskDelay(15 / portTICK_RATE_MS);
    }
    
    vTaskDelay(100 / portTICK_RATE_MS);
}

void doDoubleAlternateAnimation(i2c_lcd1602_info_t* lcdInfo)
{
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_set_right_to_left(lcdInfo);
        i2c_lcd1602_move_cursor(lcdInfo, i, 0);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
        i2c_lcd1602_move_cursor(lcdInfo, i, 2);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);

        i2c_lcd1602_set_left_to_right(lcdInfo);
        i2c_lcd1602_move_cursor(lcdInfo, 19 - i, 1);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        i2c_lcd1602_move_cursor(lcdInfo, 19 - i, 3);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);

        vTaskDelay(30 / portTICK_RATE_MS);
    }

    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_set_left_to_right(lcdInfo);
        i2c_lcd1602_move_cursor(lcdInfo, 19 - i, 0);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
        i2c_lcd1602_move_cursor(lcdInfo, 19 - i, 2);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);

        i2c_lcd1602_set_right_to_left(lcdInfo);
        i2c_lcd1602_move_cursor(lcdInfo, i, 1);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
        i2c_lcd1602_move_cursor(lcdInfo, i, 3);
        i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);

        vTaskDelay(30 / portTICK_RATE_MS);
    }

    vTaskDelay(100 / portTICK_RATE_MS);
    i2c_lcd1602_set_left_to_right(lcdInfo);
}

