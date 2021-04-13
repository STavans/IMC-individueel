//Header include guard for the extension file (lcd-menu-elaboration.c)
#ifndef LCD_MENU_C
#define LCD_MENU_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"
#include "lcd-menu-elaboration.h"
#include "animations.h"

//ID's of every lcd menu (is also the number in the lcdMenus array)
#define MAIN_MENU_ID 0
#define ECHO_MENU_ID 1
#define RADIO_MENU_ID 2
#define CLOCK_MENU_ID 3
#define SPEECH_MENU_ID 4
#define ANIMATION_MENU_ID 5

#define INVALID 99

static i2c_lcd1602_info_t *tmp_lcd_info;

//Static functions
static int displayMenu(i2c_lcd1602_info_t*, unsigned int);
static int refreshMenu(i2c_lcd1602_info_t*, unsigned int, unsigned int);
static int displayCursorOn(i2c_lcd1602_info_t*, unsigned int);

//Variable to all the lcd menu's
static LCD_MENU *lcdMenus;
//Variable to the current lcd menu
static unsigned int currentLcdMenu;
//The current selected menu item in the current menu
static unsigned int currentMenuItem;

char* choice = "DALT";
char* setChoice(char* ch);
char* getChoice();

int menu_updateMenu(i2c_lcd1602_info_t *lcdInfo, void *p)
{
    if (lcdMenus[currentLcdMenu].update == NULL)
        return LCD_MENU_ERROR;
    
    if (p != NULL)
        lcdMenus[currentLcdMenu].update(p);

    return refreshMenu(lcdInfo, currentLcdMenu, currentMenuItem);
}

int menu_goToParentMenu(i2c_lcd1602_info_t *lcdInfo)
{
    if (lcdMenus[currentLcdMenu].parent == INVALID)
        return LCD_MENU_ERROR;
    
    return displayMenu(lcdInfo, lcdMenus[currentLcdMenu].parent);
}

int menu_onClick(i2c_lcd1602_info_t *lcdInfo)
{
    if (lcdMenus[currentLcdMenu].items[currentMenuItem].onClick == NULL)
        return LCD_MENU_ERROR;
    
    lcdMenus[currentLcdMenu].items[currentMenuItem].onClick(lcdInfo);
    return LCD_MENU_OKE;
}

int menu_goToNextItem(i2c_lcd1602_info_t *lcdInfo)
{
    return displayCursorOn(lcdInfo, currentMenuItem + 1);
}

int menu_goToPreviousitem(i2c_lcd1602_info_t *lcdInfo)
{
    return displayCursorOn(lcdInfo, currentMenuItem - 1);
}

//Sets the user selector (cursor) on the given menu item
static int displayCursorOn(i2c_lcd1602_info_t *lcdInfo, unsigned int itemToSelect)
{
    LCD_MENU displayedMenu = lcdMenus[currentLcdMenu];
    LCD_MENU_ITEM currentItem = displayedMenu.items[currentMenuItem];
    LCD_MENU_ITEM newItem = displayedMenu.items[itemToSelect];

    //Check if itemToSelect is valid
    if (itemToSelect > MAX_ITEMS_ON_MENU - 1 || newItem.id == INVALID)
        return LCD_MENU_ERROR;
    
    //Remove the old cursor
    i2c_lcd1602_move_cursor(lcdInfo, currentItem.xCoord - 1, currentItem.yCoord);
    i2c_lcd1602_write_char(lcdInfo, ' ');

    //Place the new cursor
    i2c_lcd1602_move_cursor(lcdInfo, newItem.xCoord - 1, newItem.yCoord);
    i2c_lcd1602_write_char(lcdInfo, '>');

    currentMenuItem = itemToSelect;

    return LCD_MENU_OKE;
}

//Displays the given menu to the lcd
static int displayMenu(i2c_lcd1602_info_t *lcdInfo, unsigned int menuToDisplay)
{
    //Get the menu to display
    LCD_MENU newMenu = lcdMenus[menuToDisplay];

    //Perform the exit function of the old menu
    if (currentLcdMenu != INVALID && lcdMenus[currentLcdMenu].menuExit != NULL)
        lcdMenus[currentLcdMenu].menuExit();

    currentMenuItem = newMenu.items[0].id;
    currentLcdMenu = newMenu.id;

    doAnimation(lcdInfo, getChoice());

    //Perform the init function of the new menu
    if (newMenu.menuEnter != NULL)
        newMenu.menuEnter();

    return refreshMenu(lcdInfo, menuToDisplay, currentMenuItem);
}

//Refreshes the display of the menu on the lcd
static int refreshMenu(i2c_lcd1602_info_t *lcdInfo, unsigned int menuToDisplay, unsigned int selectedItem)
{
    //Clear the display
    i2c_lcd1602_clear(lcdInfo);

    //Get the menu to display
    LCD_MENU newMenu = lcdMenus[menuToDisplay];

    //Write the title of the menu to the screen
    i2c_lcd1602_move_cursor(lcdInfo, newMenu.xCoord, 0);
    i2c_lcd1602_write_string(lcdInfo, newMenu.text);

    //Write each item on the screen
    for (int i = 0; i < MAX_ITEMS_ON_MENU; i++)
    {
        //Check if item is valid
        if (newMenu.items[i].id == INVALID) 
            break;

        //Write the item on the screen
        i2c_lcd1602_move_cursor(lcdInfo, newMenu.items[i].xCoord, newMenu.items[i].yCoord);
        i2c_lcd1602_write_string(lcdInfo, newMenu.items[i].text);
    }

    return displayCursorOn(lcdInfo, selectedItem);
}

int menu_initMenus(i2c_lcd1602_info_t *lcdInfo)
{
    tmp_lcd_info = lcdInfo;

    //Hides the cursor
    i2c_lcd1602_set_cursor(lcdInfo, false);

    //Custom white rectangle
    uint8_t rectangle[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    i2c_lcd1602_define_char(lcdInfo, I2C_LCD1602_CHARACTER_CUSTOM_0 , rectangle);

    //Allocate memory for each menu
    lcdMenus = (LCD_MENU*) malloc(sizeof(LCD_MENU) * TOTAL_MENUS);
    if (lcdMenus == NULL)
        return LCD_MENU_ERROR;
    
    //Main menu
    lcdMenus[MAIN_MENU_ID].id = MAIN_MENU_ID;
    strcpy(lcdMenus[MAIN_MENU_ID].text, "MENU");
    lcdMenus[MAIN_MENU_ID].xCoord = 8;
    lcdMenus[MAIN_MENU_ID].parent = INVALID;
    lcdMenus[MAIN_MENU_ID].menuEnter = NULL;
    lcdMenus[MAIN_MENU_ID].update = NULL;
    lcdMenus[MAIN_MENU_ID].menuExit = NULL;
    lcdMenus[MAIN_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[MAIN_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        return LCD_MENU_ERROR;
    }
    
    LCD_MENU_ITEM *itemsMainMenu = lcdMenus[MAIN_MENU_ID].items;
    //Radio item
    itemsMainMenu[0].id = 0;
    strcpy(itemsMainMenu[0].text, "RADIO");
    itemsMainMenu[0].xCoord = 2;
    itemsMainMenu[0].yCoord = 2;
    itemsMainMenu[0].onClick = &onClickMainRadio;
    //Clock item
    itemsMainMenu[1].id = 1;
    strcpy(itemsMainMenu[1].text, "KLOK");
    itemsMainMenu[1].xCoord = 9;
    itemsMainMenu[1].yCoord = 2;
    itemsMainMenu[1].onClick = &onClickMainClock;
    //Echo item
    itemsMainMenu[2].id = 2;
    strcpy(itemsMainMenu[2].text, "ANIM");
    itemsMainMenu[2].xCoord = 15;
    itemsMainMenu[2].yCoord = 2;
    itemsMainMenu[2].onClick = &onClickAnimation;
    //Fill-up item
    itemsMainMenu[3].id = INVALID;


    //Echo menu
    lcdMenus[ECHO_MENU_ID].id = ECHO_MENU_ID;
    strcpy(lcdMenus[ECHO_MENU_ID].text, "MENU");
    lcdMenus[ECHO_MENU_ID].xCoord = 8;
    lcdMenus[ECHO_MENU_ID].parent = MAIN_MENU_ID;
    lcdMenus[ECHO_MENU_ID].menuEnter = NULL;
    lcdMenus[ECHO_MENU_ID].update = NULL;
    lcdMenus[ECHO_MENU_ID].menuExit = NULL;
    lcdMenus[ECHO_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[ECHO_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        free(lcdMenus[MAIN_MENU_ID].items);
        return LCD_MENU_ERROR;
    }

    LCD_MENU_ITEM *itemsEchoMenu = lcdMenus[ECHO_MENU_ID].items;
    //Record item
    itemsEchoMenu[0].id = 0;
    strcpy(itemsEchoMenu[0].text, "REC");
    itemsEchoMenu[0].xCoord = 2;
    itemsEchoMenu[0].yCoord = 2;
    itemsEchoMenu[0].onClick = NULL;
    //Clips item
    itemsEchoMenu[1].id = 1;
    strcpy(itemsEchoMenu[1].text, "CLIPS");
    itemsEchoMenu[1].xCoord = 7;
    itemsEchoMenu[1].yCoord = 2;
    itemsEchoMenu[1].onClick = NULL;
    //Speech item
    itemsEchoMenu[2].id = 2;
    strcpy(itemsEchoMenu[2].text, "SPRAAK");
    itemsEchoMenu[2].xCoord = 14;
    itemsEchoMenu[2].yCoord = 2;
    itemsEchoMenu[2].onClick = &onClickEchoSpeech;
    //Fill-up item
    itemsEchoMenu[3].id = INVALID;


    //Radio menu
    lcdMenus[RADIO_MENU_ID].id = RADIO_MENU_ID;
    strcpy(lcdMenus[RADIO_MENU_ID].text, "RADIO");
    lcdMenus[RADIO_MENU_ID].xCoord = 7;
    lcdMenus[RADIO_MENU_ID].parent = MAIN_MENU_ID;
    lcdMenus[RADIO_MENU_ID].menuEnter = &onEnterRadio;
    lcdMenus[RADIO_MENU_ID].update = NULL;
    lcdMenus[RADIO_MENU_ID].menuExit = &onExitRadio;
    lcdMenus[RADIO_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[RADIO_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        free(lcdMenus[MAIN_MENU_ID].items);
        free(lcdMenus[ECHO_MENU_ID].items);
        return LCD_MENU_ERROR;
    }

    LCD_MENU_ITEM *itemsRadioMenu = lcdMenus[RADIO_MENU_ID].items;
    //538
    itemsRadioMenu[0].id = 0;
    strcpy(itemsRadioMenu[0].text, "538");
    itemsRadioMenu[0].xCoord = 2;
    itemsRadioMenu[0].yCoord = 2;
    itemsRadioMenu[0].onClick = &onClickRadio538;
    //Q
    itemsRadioMenu[1].id = 1;
    strcpy(itemsRadioMenu[1].text, "Qmusic");
    itemsRadioMenu[1].xCoord = 7;
    itemsRadioMenu[1].yCoord = 2;
    itemsRadioMenu[1].onClick = &onClickRadioQ;
    //Sky
    itemsRadioMenu[2].id = 2;
    strcpy(itemsRadioMenu[2].text, "SKY");
    itemsRadioMenu[2].xCoord = 15;
    itemsRadioMenu[2].yCoord = 2;
    itemsRadioMenu[2].onClick = &onClickRadioSky;
    //Fill-up item
    itemsRadioMenu[3].id = INVALID;


    //Klok menu
    lcdMenus[CLOCK_MENU_ID].id = CLOCK_MENU_ID;
    strcpy(lcdMenus[CLOCK_MENU_ID].text, "KLOK");
    lcdMenus[CLOCK_MENU_ID].xCoord = 8;
    lcdMenus[CLOCK_MENU_ID].parent = MAIN_MENU_ID;
    lcdMenus[CLOCK_MENU_ID].menuEnter = &onEnterClock;
    lcdMenus[CLOCK_MENU_ID].update = &onUpdateClock;
    lcdMenus[CLOCK_MENU_ID].menuExit = &onExitClock;
    lcdMenus[CLOCK_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[CLOCK_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        free(lcdMenus[MAIN_MENU_ID].items);
        free(lcdMenus[ECHO_MENU_ID].items);
        free(lcdMenus[RADIO_MENU_ID].items);
        return LCD_MENU_ERROR;
    }

    LCD_MENU_ITEM *itemsClockMenu = lcdMenus[CLOCK_MENU_ID].items;
    //Clock item
    itemsClockMenu[0].id = 0;
    // strcpy(itemsClockMenu[0].text, "tijd");
    itemsClockMenu[0].xCoord = 6;
    itemsClockMenu[0].yCoord = 2;
    itemsClockMenu[0].onClick = &onClickClockItem;
    //Fill-up item
    itemsClockMenu[1].id = INVALID;


    //Speech recognition menu
    lcdMenus[SPEECH_MENU_ID].id = SPEECH_MENU_ID;
    strcpy(lcdMenus[SPEECH_MENU_ID].text, "SPRAAK");
    lcdMenus[SPEECH_MENU_ID].xCoord = 7;
    lcdMenus[SPEECH_MENU_ID].parent = ECHO_MENU_ID;
    lcdMenus[SPEECH_MENU_ID].menuEnter = &onEnterSpeech;
    lcdMenus[SPEECH_MENU_ID].update = &onUpdateSpeech;
    lcdMenus[SPEECH_MENU_ID].menuExit = &onExitSpeech;
    lcdMenus[SPEECH_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[SPEECH_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        free(lcdMenus[MAIN_MENU_ID].items);
        free(lcdMenus[ECHO_MENU_ID].items);
        free(lcdMenus[RADIO_MENU_ID].items);
        free(lcdMenus[CLOCK_MENU_ID].items);
        return LCD_MENU_ERROR;
    }
    
    LCD_MENU_ITEM *itemsSpeechMenu = lcdMenus[SPEECH_MENU_ID].items;
    //Recognition item
    itemsSpeechMenu[0].id = 0;
    strcpy(itemsSpeechMenu[0].text, "NULL");
    itemsSpeechMenu[0].xCoord = 2;
    itemsSpeechMenu[0].yCoord = 2;
    itemsSpeechMenu[0].onClick = NULL;
    //Fill-up item
    itemsSpeechMenu[3].id = INVALID;

    //Change Animation menu
    lcdMenus[ANIMATION_MENU_ID].id = ANIMATION_MENU_ID;
    strcpy(lcdMenus[ANIMATION_MENU_ID].text, "ANIMATION");
    lcdMenus[ANIMATION_MENU_ID].xCoord = 6;
    lcdMenus[ANIMATION_MENU_ID].parent = MAIN_MENU_ID;
    lcdMenus[ANIMATION_MENU_ID].menuEnter = NULL;
    lcdMenus[ANIMATION_MENU_ID].update = NULL;
    lcdMenus[ANIMATION_MENU_ID].menuExit = NULL;
    lcdMenus[ANIMATION_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[ANIMATION_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        free(lcdMenus[MAIN_MENU_ID].items);
        free(lcdMenus[ECHO_MENU_ID].items);
        free(lcdMenus[RADIO_MENU_ID].items);
        free(lcdMenus[CLOCK_MENU_ID].items);
        return LCD_MENU_ERROR;
    }
    
    LCD_MENU_ITEM *itemsAnimationMenu = lcdMenus[ANIMATION_MENU_ID].items;
    //Recognition item
    itemsAnimationMenu[0].id = 0;
    strcpy(itemsAnimationMenu[0].text, "FANCY");
    itemsSpeechMenu[0].xCoord = 2;
    itemsSpeechMenu[0].yCoord = 2;
    itemsSpeechMenu[0].onClick = &onClickFancy;

    itemsSpeechMenu[1].id = 1;
    strcpy(itemsSpeechMenu[0].text, "!!!!");
    itemsSpeechMenu[1].xCoord = 9;
    itemsSpeechMenu[1].yCoord = 2;
    itemsSpeechMenu[1].onClick = &onClickExcl;
    
    itemsSpeechMenu[2].id = 2;
    strcpy(itemsSpeechMenu[0].text, "DALT");
    itemsSpeechMenu[2].xCoord = 15;
    itemsSpeechMenu[2].yCoord = 2;
    itemsSpeechMenu[2].onClick = &onClickDALT;

    //Fill-up item
    itemsSpeechMenu[3].id = INVALID;


    //Display the main menu
    currentLcdMenu = INVALID;
    return displayMenu(lcdInfo, MAIN_MENU_ID);
}

//Sets choice for animation
char* setChoice(char *change){
    choice = strncpy(choice, change, 5);
    return choice;
}

//Gets the choice for animation
char* getChoice(){
    return choice;
}

i2c_lcd1602_info_t* menu_getLcdInfo()
{
    return tmp_lcd_info;
}

#endif