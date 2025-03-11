/*
 * display_driver.h
 *
 *  Created on: Mar 6, 2025
 *      Author: DELL
 */

#ifndef INC_DISPLAY_DRIVER_H_
#define INC_DISPLAY_DRIVER_H_

/*Includes*/
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


/* Declare functions */
void updateMenuDisplay(void);
void setupParentMenus(void);
void displaySelection(void);
void navigateToSubmenu(Menu *submenu);
void navigateBack(void);
void handleSelection();

#endif /* INC_DISPLAY_DRIVER_H_ */
