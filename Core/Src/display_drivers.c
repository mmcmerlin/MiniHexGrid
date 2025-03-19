/*
 * display_drivers.c
 *
 *  Created on: Mar 3, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#include "display_driver.h"
#include <string.h>

/* Scrolling Parameters */
uint8_t startIndex = 0; // First item visible in the current window
uint8_t currentIndex = 0;

/* Current Menu Pointer */
Menu *currentMenu;

// **Independent values for each module**
extern int16_t realPower, reactivePower;
extern int totalLoad;
extern int hostFreq, hostPower;
extern int transformerStatus;

// Adjusting value
int adjustActive = 100;
int adjustReactive = 100;
int rampRate = 100;

/*Declare menus*/
Menu hostMenu, ccgtMenu, transformerMenu, cityMenu, transmissionMenu;
Menu windMenu, windInfoMenu;
Menu hostInfoMenu, ccgtInfoMenu, transformerInfoMenu, cityInfoMenu, transmissionInfoMenu;
Menu ccgtSetpointMenu;
//Add adjust menu
Menu ccgtActive, ccgtReactive, ccgtRamp;

// Main Menu Items
const char *hostItems[] = {"Module info", "Game mode"};
const char *transformerItems[] = {"Module Info", "Tap Changer"};
const char *cityItems[] = {"Module Info", "Load Value"};
const char *transmissionItems[] = {"Module Info", "Resistance", "Reactance"};
const char *ccgtItems[] = {"Module Info", "Setpoints"};
const char *windItems[] = {"Module Info", "Setpoints"};
// Module Info Items //pretty sure im not displaying this
const char *hostInfoItems[] = {"Host Data", "Back"};
const char *transformerInfoItems[] = {"Transformer Data", "Back"};
const char *cityInfoItems[] = {"City Data", "Back"};
const char *transmissionInfoItems[] = {"Transmission Data", "Back"};
const char *ccgtInfoItems[] = {"Generator Data", "Back"};
const char *windInfoItems[] = {"Generator Data", "Back"};
// Setpoint Items (TO DO)
//const char *hostSetpoints[] = {"Mode 1", "Mode 2", "Back"};
//const char *transformerSetpoints[] = {"Transformer Data", "Back"};
//const char *citySetpoints[] = {"City Data", "Back"};
//const char *transmissionSetpoints[] = {"Transmission Data", "Back"};
const char *ccgtSetpoints[] = {"Active", "Reactive", "Ramp rate",  "Back"};

// Submenu pointers
Menu *hostSubmenus[] = {&hostInfoMenu, NULL};
Menu *transformerSubmenus[] = {&transformerInfoMenu, NULL};
Menu *citySubmenus[] = {&cityInfoMenu, NULL};
Menu *transmissionSubmenus[] = {&transmissionInfoMenu,NULL, NULL};
Menu *ccgtSubmenus[] = {&ccgtInfoMenu, &ccgtSetpointMenu};
Menu *windSubmenus[] = {&windInfoMenu, NULL};

//Setpoint submenus
Menu *ccgtSetpointSubmenu[] = {&ccgtActive, &ccgtReactive, &ccgtRamp, NULL};

//Setup Menus
void setupMenus(void) {
	//default menu
	currentMenu = &hostMenu;

    // Initialize Host Menu
    hostMenu.title = "Host";
    hostMenu.items = hostItems;
    hostMenu.itemCount = 2;
    hostMenu.subMenus = hostSubmenus;
    hostMenu.parentMenu = NULL;
    hostMenu.showInfo = NULL;

    // Transformer Menu
    transformerMenu.title = "Transformer";
    transformerMenu.items = transformerItems;
    transformerMenu.itemCount = 2;
    transformerMenu.subMenus = transformerSubmenus;
    transformerMenu.parentMenu = NULL;
    transformerMenu.showInfo = NULL;

    // City Menu
    cityMenu.title = "City";
    cityMenu.items = cityItems;
    cityMenu.itemCount = 2;
    cityMenu.subMenus = citySubmenus;
    cityMenu.parentMenu = NULL;
    cityMenu.showInfo = NULL;

    // Transmission Menu
    transmissionMenu.title = "Transmission";
    transmissionMenu.items = transmissionItems;
    transmissionMenu.itemCount = 3;
    transmissionMenu.subMenus = transmissionSubmenus;
    transmissionMenu.parentMenu = NULL;
    transmissionMenu.showInfo = NULL;

    // CCGT Generator Menu
    ccgtMenu.title = "CCGT";
    ccgtMenu.items = ccgtItems;
    ccgtMenu.itemCount = 2;
    ccgtMenu.subMenus = ccgtSubmenus;
    ccgtMenu.parentMenu = NULL;
    ccgtMenu.showInfo = NULL;

    // Wind Generator Menu
    windMenu.title = "Wind Generator";
    windMenu.items = windItems;
    windMenu.itemCount = 2;
    windMenu.subMenus = windSubmenus;
    windMenu.parentMenu = NULL;
    windMenu.showInfo = NULL;

    // Wind Generator Info Menu
    windInfoMenu.title = "Wind Info";
    windInfoMenu.items = NULL;
    windInfoMenu.itemCount = 0;
    windInfoMenu.subMenus = NULL;
    windInfoMenu.parentMenu = &windMenu;
    windInfoMenu.showInfo = ShowGeneratorInfo;
    windInfoMenu.adjustFunc = NULL;

    //Host Grid Info Menu
    hostInfoMenu.title = "Grid Info";
    hostInfoMenu.items = hostInfoItems;
    hostInfoMenu.itemCount = 2;
    hostInfoMenu.subMenus = NULL;
    hostInfoMenu.parentMenu = &hostMenu;
    hostInfoMenu.showInfo = ShowHostInfo;
    hostInfoMenu.adjustFunc = NULL;

    // Transformer Module Info Menu
    transformerInfoMenu.title = "Transformer Info";
    transformerInfoMenu.items = transformerInfoItems;
    transformerInfoMenu.itemCount = 2;
    transformerInfoMenu.subMenus = NULL;
    transformerInfoMenu.parentMenu = &transformerMenu;
    transformerInfoMenu.showInfo = ShowTransformerInfo;

    // City Module Info Menu
    cityInfoMenu.title = "City Info";
    cityInfoMenu.items = cityInfoItems;
    cityInfoMenu.itemCount = 2;
    cityInfoMenu.subMenus = NULL;
    cityInfoMenu.parentMenu = &cityMenu;
    cityInfoMenu.showInfo = ShowCityInfo;

    // Transmission Module Info Menu
    transmissionInfoMenu.title = "Transmission Info";
    transmissionInfoMenu.items = transmissionInfoItems;
    transmissionInfoMenu.itemCount = 2;
    transmissionInfoMenu.subMenus = NULL;
    transmissionInfoMenu.parentMenu = &transmissionMenu;
    transmissionInfoMenu.showInfo = ShowTransmissionInfo;

    // CCGT Module Info Menu
    ccgtInfoMenu.title = "Generator Info";
    ccgtInfoMenu.items = ccgtInfoItems;
    ccgtInfoMenu.itemCount = 2;
    ccgtInfoMenu.subMenus = NULL;
    ccgtInfoMenu.parentMenu = &ccgtMenu;
    ccgtInfoMenu.showInfo = ShowGeneratorInfo;

    // CCGT Setpoint Menu
    ccgtSetpointMenu.title = "CCGT Setpoint";
    ccgtSetpointMenu.items = ccgtSetpoints;
    ccgtSetpointMenu.itemCount = 4;
    ccgtSetpointMenu.subMenus = ccgtSetpointSubmenu;
    ccgtSetpointMenu.parentMenu = &ccgtMenu;
    //ccgtInfoMenu.showInfo = NULL;

    // CCGT Active Power
    ccgtActive.title = "Active Power";
    ccgtActive.items = NULL;
    ccgtActive.itemCount = 0;
    ccgtActive.subMenus = NULL;
    ccgtActive.parentMenu = &ccgtSetpointMenu;
    ccgtActive.showInfo = NULL;
    ccgtActive.adjustFunc = AdjustActivePower;

    // CCGT Reactive Power
    ccgtReactive.title = "Reactive Power";
    ccgtReactive.items = NULL;
    ccgtReactive.itemCount = 0;
    ccgtReactive.subMenus = NULL;
    ccgtReactive.parentMenu = &ccgtSetpointMenu;
    ccgtReactive.showInfo = NULL;
    ccgtReactive.adjustFunc = AdjustReactivePower;

    // CCGT Ramp rate
    ccgtRamp.title = "Ramp rate";
    ccgtRamp.items = NULL;
    ccgtRamp.itemCount = 0;
    ccgtRamp.subMenus = NULL;
    ccgtRamp.parentMenu = &ccgtSetpointMenu;
    ccgtRamp.showInfo = NULL;
    ccgtRamp.adjustFunc = AdjustRamp;
}

// **Module-specific display functions**
void ShowHostInfo(){
    ssd1306_SetCursor(10, 17);
    ssd1306_WriteString("Frequency:", Font_7x10, White);

    char buffer[10];
    sprintf(buffer, "%d", hostFreq);
    ssd1306_SetCursor(10, 27);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(10, 37);
    ssd1306_WriteString("Total Power:", Font_7x10, White);

    sprintf(buffer, "%d", hostPower);
    ssd1306_SetCursor(10, 47);
    ssd1306_WriteString(buffer, Font_7x10, White);
}

void ShowTransformerInfo() {
    ssd1306_SetCursor(10, 17);
    ssd1306_WriteString("Transformer Status:", Font_7x10, White);

    char buffer[10];
    sprintf(buffer, "%d", transformerStatus);
    ssd1306_SetCursor(10, 27);
    ssd1306_WriteString(buffer, Font_7x10, White);
}

void ShowCityInfo() {
    ssd1306_SetCursor(10, 17);
    ssd1306_WriteString("Total load:", Font_7x10, White);

    char buffer[10];
    sprintf(buffer, "%d", totalLoad);
    ssd1306_SetCursor(10, 27);
    ssd1306_WriteString(buffer, Font_7x10, White);
}

void ShowTransmissionInfo() {
    ssd1306_SetCursor(10, 20);
    ssd1306_WriteString("Transmission Status:", Font_7x10, White);

    char buffer[10];
    sprintf(buffer, "%d", transformerStatus); // Example value
    ssd1306_SetCursor(10, 40);
    ssd1306_WriteString(buffer, Font_7x10, White);
}

void ShowGeneratorInfo() {
    ssd1306_SetCursor(10, 17);
    ssd1306_WriteString("Real Power:", Font_7x10, White);

    char buffer[10];
    sprintf(buffer, "%d", realPower);
    ssd1306_SetCursor(10, 27);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(10, 37);
    ssd1306_WriteString("Reactive Power:", Font_7x10, White);

    sprintf(buffer, "%d", reactivePower);
    ssd1306_SetCursor(10, 47);
    ssd1306_WriteString(buffer, Font_7x10, White);
}

// Module Specific Adjust functions
void AdjustActivePower() {
    StartAdjustmentMode(&adjustActive, "Adjust Active", 70, 200);
}

void AdjustReactivePower() {
    StartAdjustmentMode(&adjustReactive, "Adjust Reactive", 0, 50);
}

void AdjustRamp(){
	StartAdjustmentMode(&rampRate, "Adjust Ramp rate", 0, 100);
}

void updateMenuDisplay(void)
{
	ssd1306_Fill(Black);
	//New
	ssd1306_SetCursor(10, 0);
    // Display menu title
    ssd1306_WriteString(currentMenu->title, Font_7x10, White);

    // **Call module-specific display function**
    if (currentMenu->showInfo != NULL) {
        currentMenu->showInfo();  // Call assigned function
    } else {
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
    }
	ssd1306_UpdateScreen();
}

/*Navigation and selection functions*/
void displaySelection() {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(10, 0);
	ssd1306_WriteString("Selected:", Font_11x18, White);
	ssd1306_SetCursor(10, 25);
	ssd1306_WriteString(currentMenu->items[currentIndex], Font_11x18, White);
	ssd1306_UpdateScreen();
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
