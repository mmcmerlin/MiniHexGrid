/*
 * display_drivers.c
 *
 *  Created on: Mar 3, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#include "display_drivers.h"

void updateMenuDisplay(void)
{
		ssd1306_Fill(Black);
		// Display top message based on the current menu
		if (currentMenu == &startupMenu) {
			ssd1306_SetCursor(10, 0);
			ssd1306_WriteString("Are you the CDC?", Font_7x10, White);
		} else if (currentMenu == &optionMenu) {
			ssd1306_SetCursor(10, 0);
			ssd1306_WriteString("Select a module", Font_7x10, White);
		} else if (currentMenu == &tformerMenu) {
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
    tformerMenu.parentMenu = &optionMenu;
    cityModule.parentMenu = &cityMenu;
    cityMenu.parentMenu = &optionMenu;
    tmissionModule.parentMenu = &tmissionMenu;
    tmissionMenu.parentMenu = &optionMenu;
    genModule.parentMenu = &genMenu;
    genMenu.parentMenu = &optionMenu;
    //old stuff
    CDCsubmenu1.parentMenu = &CDCmenu;
    CDCsubmenu2.parentMenu = &CDCmenu;
    CDCmenu.parentMenu = &startupMenu; // Top-level menu
    optionMenu.parentMenu = &startupMenu;
    startupMenu.parentMenu = NULL; // Entry point
}

/*Navigation and selection functions*/
void displaySelection() {
    if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) { // Acquire the mutex
        ssd1306_WriteString("Selected:", Font_11x18, White);
        ssd1306_SetCursor(10, 25);
        ssd1306_WriteString(currentMenu->items[currentIndex], Font_11x18, White);
        ssd1306_UpdateScreen();
        osDelay(500);                // Delay to show feedback
        osMutexRelease(displayMutexHandle); // Release the mutex
    }
}

void navigateToSubmenu(Menu *submenu) {
    displaySelectionFeedback(); 	// Show feedback for the selection
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
	  ssd1306_Fill(Black);
	  ssd1306_SetCursor(10, 0);

	  if (currentMenu->subMenus && currentMenu->subMenus[currentIndex]) {
		  navigateToSubmenu(currentMenu->subMenus[currentIndex]);
	  } else if (strcmp(currentMenu->items[currentIndex], "Back") == 0) {
		  navigateBack();
	  } else if (strcmp(currentMenu->items[currentIndex], "Exit") == 0) {
		  navigateBack();        // Navigate back to Option Menu
	  } else if (strcmp(currentMenu->items[currentIndex], "Return") == 0) {
		  navigateBack();        // Navigate back to Option Menu
	  }
}
