#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "exclamation.h"


void doExclamationMarkFilled(i2c_lcd1602_info_t*);
void doExclamationMarkEmpty(i2c_lcd1602_info_t*);

//The exclamation mark where the squares of the exclamation mark is filled and the surrounding sqaures have holes.
void doExclamationMarkFilled(i2c_lcd1602_info_t* lcdInfo)
{
    i2c_lcd1602_move_cursor(lcdInfo, 0, 0);
    i2c_lcd1602_set_left_to_right(lcdInfo);
    for (int i = 0; i < 20; i ++)
    {
            i2c_lcd1602_move_cursor(lcdInfo, i, 0);
            i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
            
            if (i == 0 || i == 1 || i == 4 || i == 5 || i == 18 || i == 19)
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 1);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
            }else 
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 1);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
            }

            if (i == 0 || i == 1 || i == 4 || i == 5 || i == 18 || i == 19)
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 2);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
            }else 
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 2);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
            }

            i2c_lcd1602_move_cursor(lcdInfo, i, 3);
            i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
    }
}

//The exclamation mark where the squares of the exclamation mark have holes and the surrounding sqaures are filled.
void doExclamationMarkEmpty(i2c_lcd1602_info_t* lcdInfo)
{
    i2c_lcd1602_move_cursor(lcdInfo, 0, 0);
    i2c_lcd1602_set_left_to_right(lcdInfo);
    for (int i = 0; i < 20; i ++)
    {
            i2c_lcd1602_move_cursor(lcdInfo, i, 0);
            i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
            
            if (i == 0 || i == 1 || i == 4 || i == 5 || i == 18 || i == 19)
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 1);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
            }else 
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 1);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
            }

            if (i == 0 || i == 1 || i == 4 || i == 5 || i == 18 || i == 19)
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 2);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
            }else 
            {
                i2c_lcd1602_move_cursor(lcdInfo, i, 2);
                i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_SQUARE);
            }

            i2c_lcd1602_move_cursor(lcdInfo, i, 3);
            i2c_lcd1602_write_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0);
    }
}