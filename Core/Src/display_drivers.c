/*
 * display_drivers.c
 *
 *  Created on: Mar 3, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#include "display_driver.h"
#include <string.h>

//Adjusting encoder value
int *valueToAdjust = NULL;  // Initialize as NULL
char labelToShow[20];
int minVal, maxVal;
//default values
int tapChange = 100;
int load = 100;
int resist = 100;
int react = 50;
int active = 100;
int reactive = 20;


/* Display Variables */
/* Scrolling Parameters */
uint8_t startIndex = 0; // First item visible in the current window


/* Menu Items */
//const char *startupItems[] = {"Yes", "No"};
const char *CDCmenuItems[] = {"CDC info", "Ext peripherals"};
//const char *optionMenuItems[] = {"Transformer", "City", "Transmission", "Generator", "Return"};
//new stuff
const char *tformerMenuItems[] = {"Module info"};
const char *tformerSubmenuItems[] = {"Tap changer", "Back"};
const char *cityMenuItems[] = {"Module info"};
const char *citySubmenuItems[] = {"Load value", "Back"};
const char *tmissionMenuItems[] = {"Module info"};
const char *tmissionSubmenuItems[] = {"Resistance", "Reactance", "Back"};
const char *genMenuItems[] = {"Module info"};
const char *genSubmenuItems[] = {"Active","Reactive", "Back"};
//old
const char *CDCsubmenu1Items[] = {"Mode 1", "Mode 2", "Back"}; //ask what game modes to add
const char *CDCsubmenu2Items[] = {"Option 1", "Option 2", "Back"};

/* Submenus */
//new stuff
Menu tformerModule = {tformerSubmenuItems, 2, NULL, NULL};
Menu *tformerSubmenu[] = {&tformerModule};
Menu tformerMenu = {tformerMenuItems, 1, tformerSubmenu, NULL};
Menu cityModule = {citySubmenuItems, 2, NULL, NULL};
Menu *citySubmenu[] = {&cityModule};
Menu cityMenu = {cityMenuItems, 1, citySubmenu, NULL};
Menu tmissionModule = {tmissionSubmenuItems, 3, NULL, NULL};
Menu *tmissionSubmenu[] = {&tmissionModule};
Menu tmissionMenu = {tmissionMenuItems, 1, tmissionSubmenu, NULL};
Menu genModule = {genSubmenuItems, 3, NULL, NULL};
Menu *genSubmenu[] = {&genModule};
Menu genMenu = {genMenuItems, 1, genSubmenu, NULL};
//Menu *optionSubmenu[] = {&tformerMenu, &cityMenu, &tmissionMenu, &genMenu, NULL};
//old
Menu CDCsubmenu1 = {CDCsubmenu1Items, 3, NULL, NULL};
Menu CDCsubmenu2 = {CDCsubmenu2Items, 3, NULL, NULL};
Menu *CDCsubMenus[] = {&CDCsubmenu1, &CDCsubmenu2};
Menu CDCmenu = {CDCmenuItems,2, CDCsubMenus, NULL};
//Menu optionMenu = {optionMenuItems, 5, optionSubmenu, NULL}; // add submenu
//Menu *startupSubMenus[] = {&CDCmenu, &optionMenu};
//Menu startupMenu = {startupItems, 2, startupSubMenus, NULL};


/* Current Menu Pointer */
Menu *currentMenu = &genMenu;
uint8_t currentIndex = 0;

void updateMenuDisplay(void)
{
	ssd1306_Fill(Black);
	if (currentMenu == &tformerMenu) {
		ssd1306_SetCursor(10, 0);
		ssd1306_WriteString("Transformer menu", Font_7x10, White);
	} else if (currentMenu == &cityMenu) {
		ssd1306_SetCursor(10, 0);
		ssd1306_WriteString("City menu", Font_7x10, White);
	} else if (currentMenu == &tmissionMenu) {
		ssd1306_SetCursor(10, 0);
		ssd1306_WriteString("Transmission menu", Font_7x10, White);
	} else if (currentMenu == &genMenu) {
		ssd1306_SetCursor(10, 0);
		ssd1306_WriteString("Generator menu", Font_7x10, White);
	} else if (currentMenu == &CDCmenu) {
		ssd1306_SetCursor(10, 0);
		ssd1306_WriteString("CDC menu", Font_7x10, White);
	}

	// Display menu items starting from the second line
	for (uint8_t i = 0; i < WINDOW_SIZE; i++) {
		uint8_t itemIndex = startIndex + i;
		if (itemIndex >= currentMenu->itemCount) {
		  break;  // Stop if there are no more items to display
		}
		ssd1306_SetCursor(10, (i + 1) * 16); // Each line is 16px high
		if (itemIndex == currentIndex) {
		  ssd1306_WriteString("> ", Font_7x10, White);
		} else {
		  ssd1306_WriteString("  ", Font_7x10, White);
		}
		ssd1306_WriteString(currentMenu->items[itemIndex], Font_7x10, White);
		}
	ssd1306_UpdateScreen();
}

void setupParentMenus(void) {
    //new stuff
    tformerModule.parentMenu = &tformerMenu;
    tformerMenu.parentMenu = NULL; //Top-level menu
    cityModule.parentMenu = &cityMenu;
    cityMenu.parentMenu = NULL; //Top-level menu
    tmissionModule.parentMenu = &tmissionMenu;
    tmissionMenu.parentMenu = NULL; //Top-level menu
    genModule.parentMenu = &genMenu;
    genMenu.parentMenu = NULL; //Top-level menu
    //old stuff
    CDCsubmenu1.parentMenu = &CDCmenu;
    CDCsubmenu2.parentMenu = &CDCmenu;
    CDCmenu.parentMenu = NULL; //Top-level menu
  //optionMenu.parentMenu = &startupMenu;
  //startupMenu.parentMenu = NULL; // Entry point
}

/*Navigation and selection functions*/
void displaySelection() {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(10, 0);
	ssd1306_WriteString("Selected:", Font_11x18, White);
	ssd1306_SetCursor(10, 25);
	ssd1306_WriteString(currentMenu->items[currentIndex], Font_11x18, White);
	ssd1306_UpdateScreen();
    osDelay(500);                // Delay to show feedback
}

void navigateToSubmenu(Menu *submenu) {
    displaySelection(); 	// Show feedback for the selection
    currentMenu = submenu;    		// Navigate to the given submenu
    currentIndex = 0;         		// Reset index for the new menu
    startIndex = 0;           		// Reset the display window

}

void navigateBack() {
    if (currentMenu->parentMenu) {
        currentMenu = currentMenu->parentMenu; // Navigate back to the parent menu
        currentIndex = 0;                      // Reset index for the parent menu
        startIndex = 0;                        // Reset the display window
    }
}

void handleSelection() {
	  if (currentMenu->subMenus && currentMenu->subMenus[currentIndex]) {
		  navigateToSubmenu(currentMenu->subMenus[currentIndex]);
	  } else if (strcmp(currentMenu->items[currentIndex], "Back") == 0) {
		  navigateBack();
	  }
}

void liveUpdate(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(10, 10);
    ssd1306_WriteString(labelToShow, Font_11x18, White);
    ssd1306_SetCursor(10, 30);

    char buffer[10];
    sprintf(buffer, "%d", *valueToAdjust);
    ssd1306_WriteString(buffer, Font_11x18, White);
    ssd1306_UpdateScreen();  // Update the OLED screen immediately
}
