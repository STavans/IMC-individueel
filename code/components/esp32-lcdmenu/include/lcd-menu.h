#ifndef LCDMENU_H
#define LCDMENU_H

//LCD_MENU error codes
#define LCD_MENU_OKE 0
#define LCD_MENU_ERROR 1

//Some constants for the lcd menu's
#define TOTAL_MENUS 7
#define MAX_ITEMS_ON_MENU 4
#define MAX_TEXT_LENGTH 8

//A struct for a menu item
typedef struct
{
    unsigned int id;
    char text[MAX_TEXT_LENGTH];
    void (*onClick)(i2c_lcd1602_info_t*);
    unsigned int xCoord;  //Where to start to write the text
    unsigned int yCoord;  //Where to start to write the text
} LCD_MENU_ITEM;

//A struct for a menu
typedef struct menu
{
    unsigned int id;
    unsigned int xCoord; //Where to start to write the text
    char text[MAX_TEXT_LENGTH];
    void (*menuEnter)(void);
    void (*update)(void*); 
    void (*menuExit)(void);
    LCD_MENU_ITEM *items; //Pointer to the menu items
    unsigned int parent; //ID to the parent LCD_MENU
} LCD_MENU;

/*
Call this method to init all the menu's

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_initMenus(i2c_lcd1602_info_t*);

/*
Call this method to update the current lcd menu

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info, a generic value     
*/
int menu_updateMenu(i2c_lcd1602_info_t*, void*);

/*
Function to go to the parent menu of the current menu

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_goToParentMenu(i2c_lcd1602_info_t*);

/*
Function to start the action (the onClick() function) of
the current selected item on the current menu

-Returns:       A LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_onClick(i2c_lcd1602_info_t*);

/*
Function to set the cursor to the next item in the menu

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_goToNextItem(i2c_lcd1602_info_t*);

/*
Function to set the cursor to the previous item in the menu

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_goToPreviousitem(i2c_lcd1602_info_t*);

//Returns the lcd info
i2c_lcd1602_info_t* menu_getLcdInfo();

//Sets choice for animation
char* setChoice(char*);

//Gets the choice for animation
char* getChange();

#endif