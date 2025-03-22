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

// Function pointer type for module info display functions
typedef void (*ShowInfo)(void);
// Function pointer type for adjusting values
typedef void (*AdjustFunc)(void);

/* Menu Structure */
typedef struct Menu {
	const char *title;        // Title of the menu
    const char **items;        // Pointer to menu item strings
    uint8_t itemCount;         // Number of items in this menu
    struct Menu **subMenus;    // Array of pointers to submenus (NULL if no submenu)
    struct Menu *parentMenu;   // Pointer to parent menu (NULL if top-level menu)
    ShowInfo showInfo;        // Function pointer for module-specific display
    AdjustFunc adjustFunc;    // Function pointer for adjusting values
} Menu;

//might not need anymore
extern Menu tformerModule;
extern Menu tformerMenu;
extern Menu cityModule;
extern Menu cityMenu;
extern Menu tmissionModule;
extern Menu tmissionMenu;
extern Menu genModule;
extern Menu genMenu;
extern Menu CDCsubmenu1;
extern Menu CDCsubmenu2;
extern Menu CDCmenu;
//new names
extern Menu windMenu;
extern Menu ccgtMenu;

//used externally by encoder for menu
extern uint8_t currentIndex;
extern Menu *currentMenu;
extern uint8_t startIndex;
//used externally by encoder for adjusting
extern int *adjustingValue;
extern const char *adjustLabel;
extern int adjustMin, adjustMax;

/* Adjusting Variables */
extern int *valueToAdjust;
extern char labelToShow[20];
// Adjusting value
extern int adjustActive;
extern int adjustReactive;
extern int rampRate;

/* Declare functions */
void updateMenuDisplay(void);
//void updateAdjustDisplay(void);
void setupMenus(void);
void displaySelection(void);
void navigateToSubmenu(Menu *submenu);
void navigateBack(void);
void handleSelection(void);
void liveUpdate(void);

// Function prototypes for display
void ShowTransformerInfo(void);
void ShowCityInfo(void);
void ShowTransmissionInfo(void);
void ShowGeneratorInfo(void);
void ShowHostInfo(void);

// Function Prototypes for adjusting
void StartAdjustmentMode(int *value, const char *label, int min, int max);
//void AdjustTapChanger(void);
void AdjustActivePower(void);
void AdjustReactivePower(void);
void AdjustRamp(void);


#endif /* INC_DISPLAY_DRIVER_H_ */
