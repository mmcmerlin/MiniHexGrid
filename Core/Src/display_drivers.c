/*
 * display_drivers.c
 *
 *  Created on: Mar 3, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#include "display_drivers.h"

void updateMenuDisplay(void)
{
	if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
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
		osMutexRelease(displayMutexHandle);
		}
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

