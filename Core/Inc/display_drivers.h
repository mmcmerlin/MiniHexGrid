/*
 * display_drivers.c
 *
 *  Created on: March 03, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#ifndef INC_DISPLAY_DRIVER_H_
#define INC_DISPLAY_DRIVER_H_

/*Includes*/
#include "app_freertos.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

/* Defines */
#define WINDOW_SIZE 3 // Maximum number of items visible on the screen (64px height / 16px per line)

/* Menu Structure */
typedef struct Menu {
    const char **items;        // Pointer to menu item strings
    uint8_t itemCount;         // Number of items in this menu
    struct Menu **subMenus;    // Array of pointers to submenus (NULL if no submenu)
    struct Menu *parentMenu;   // Pointer to parent menu (NULL if top-level menu)
} Menu;

/* Variables */

/* Scrolling Parameters */
uint8_t startIndex = 0; // First item visible in the current window

/* Menu Items */
const char *startupItems[] = {"Yes", "No"};
const char *CDCmenuItems[] = {"CDC info", "Ext peripherals", "Back"};
const char *optionMenuItems[] = {"Transformer", "City", "Transmission", "Generator", "Return"};
//new stuff
const char *tformerMenuItems[] = {"Module info", "Exit"};
const char *tformerSubmenuItems[] = {"Tap changer", "Back"};
const char *cityMenuItems[] = {"Module info", "Exit"};
const char *citySubmenuItems[] = {"Load value", "Back"};
const char *tmissionMenuItems[] = {"Module info", "Exit"};
const char *tmissionSubmenuItems[] = {"Resistance", "Reactance", "Back"};
const char *genMenuItems[] = {"Module info", "Exit"};
const char *genSubmenuItems[] = {"Active","Reactive", "Back"};
//old
const char *CDCsubmenu1Items[] = {"Mode 1", "Mode 2", "Back"}; //ask what game modes to add
const char *CDCsubmenu2Items[] = {"Option 1", "Option 2", "Back"};

/* Submenus */
//new stuff
Menu tformerModule = {tformerSubmenuItems, 2, NULL, NULL};
Menu *tformerSubmenu[] = {&tformerModule, NULL};
Menu tformerMenu = {tformerMenuItems, 2, tformerSubmenu, NULL};
Menu cityModule = {citySubmenuItems, 2, NULL, NULL};
Menu *citySubmenu[] = {&cityModule, NULL};
Menu cityMenu = {cityMenuItems, 2, citySubmenu, NULL};
Menu tmissionModule = {tmissionSubmenuItems, 3, NULL, NULL};
Menu *tmissionSubmenu[] = {&tmissionModule, NULL};
Menu tmissionMenu = {tmissionMenuItems, 2, tmissionSubmenu, NULL};
Menu genModule = {genSubmenuItems, 3, NULL, NULL};
Menu *genSubmenu[] = {&genModule, NULL};
Menu genMenu = {genMenuItems, 2, genSubmenu, NULL};
Menu *optionSubmenu[] = {&tformerMenu, &cityMenu, &tmissionMenu, &genMenu, NULL};
//old
Menu CDCsubmenu1 = {CDCsubmenu1Items, 3, NULL, NULL};
Menu CDCsubmenu2 = {CDCsubmenu2Items, 3, NULL, NULL};
Menu *CDCsubMenus[] = {&CDCsubmenu1, &CDCsubmenu2, NULL};
Menu CDCmenu = {CDCmenuItems,3, CDCsubMenus, NULL};
Menu optionMenu = {optionMenuItems, 5, optionSubmenu, NULL}; // add submenu
Menu *startupSubMenus[] = {&CDCmenu, &optionMenu};
Menu startupMenu = {startupItems, 2, startupSubMenus, NULL};


/* Current Menu Pointer */
Menu *currentMenu = &startupMenu;
uint8_t currentIndex = 0;

/* Declare functions */
void updateMenuDisplay(void);
void setupParentMenus(void);
void displaySelection(void);
void navigateToSubmenu(Menu *submenu);
void navigateBack(void);
void handleSelection();

#endif /* INC_NEOPIXEL_DRIVER_H_ */
