/***************************************************************************************
    Name    : LCD Button Shield Menu
    Author  : Paul Siewert
    Created : June 14, 2016
    Last Modified: June 14, 2016
    Version : 1.0
    Notes   : This code is for use with an Arduino Uno and LCD/button shield. The
              intent is for anyone to use this program to give them a starting
              program with a fully functional menu with minimal modifications
              required by the user.
    License : This program is free software. You can redistribute it and/or modify
              it under the terms of the GNU General Public License as published by
              the Free Software Foundation, either version 3 of the License, or
              (at your option) any later version.
              This program is distributed in the hope that it will be useful,
              but WITHOUT ANY WARRANTY; without even the implied warranty of
              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
              GNU General Public License for more details.
 ***************************************************************************************/
/*
   This program is designed to get you as close as possible to a finished menu for the standard Arduino Uno LCD/button shield. The only required modifications
   are to add as menu items to the master menu (menuItems array) and then modify/adjust the void functions below for each of those selections.
*/

#include <Wire.h>
//#include <LiquidCrystal.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

// Setting the LCD shields pins
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b11111, // *****
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b11111, // *****
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};

// You can have up to 10 menu items in the menuItems[] array below without having to change the base programming at all. Name them however you'd like. Beyond 10 items, you will have to add additional "cases" in the switch/case
// section of the operateMainMenu() function below. You will also have to add additional void functions (i.e. menuItem11, menuItem12, etc.) to the program.
String menuItems[] = {"Light", "Heating Glass", "ITEM 3", "ITEM 4", "ITEM 5", "ITEM 6"};

// define submenus here as well later

// Navigation button variables
//int readKey;

// Menu control variables
int menuPage = 0; //  first menupage 0 contains 2 entries with 2 lines available on the display
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);  // two items can be shown on each menu page double lined display
int cursorPosition = 0;

void setup() {

  // Initializes serial communication
  Serial.begin(9600);

  // Initializes and clears the LCD screen columns and rows
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setBacklight(WHITE);

  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);

// mainMenuDraw and drawCursor executed within operateMainMenu in the void loop, only initialize here once
  mainMenuDraw();
  drawCursor(); 
}

void loop() {
  operateMainMenu();
  // void functions for events like LED lighting or relay
  // question is will they stay in their current state like drivers for led while programm is looping through button interface command
}

// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  Serial.print(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);

  if (menuPage == 0) {  // your are in the upper most level of the menu, indication arrow points downwards
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {  // in between arrows points in both available directions
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {  // your are in the lowest level of the menu, indication arrow points upwards
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {     // Erases current cursor on the left side
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  // The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
  // in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}

void operateMainMenu(){  // navigation through menu
  int nav_delay = 200;  // delay for up and down navigation not to skip entries too fast
  uint8_t buttons = lcd.readButtons();
  if (buttons) {  //check if any button is pressed for user interface
    if (buttons & BUTTON_UP) {
      buttons = 0;
      if (menuPage == 0) {  //  you are already in the upmost menu page
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
          //break;
        }
      if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
          //break;
        }

      if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
          //break;
        }
        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw(); //  those will eventually loop already in the void loop
        drawCursor();
        delay(nav_delay);
       
    }
   
    if (buttons & BUTTON_DOWN) {
      buttons = 0;
      if (menuPage == maxMenuPages) {  //  you are already in the lowest menu page
          cursorPosition = cursorPosition + 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
          //break;
        }     
      if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
          //break;
        }

      if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
          //break;
        }
        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor(); 
        delay(nav_delay);  
    }
    
    if (buttons & BUTTON_RIGHT) {  //  enter next menu entry
      switch (cursorPosition) { // The case that is selected here is dependent where the cursor is on the main menu
          case 0:
            //sub_menu_item1();
            break;
          case 1:
            //menuItem2();
            break;
          case 2:
            //menuItem3();
            break;
          case 3:
            //menuItem4();
            break;
          case 4:
            //menuItem5();
            break;
          case 5:
            //menuItem6();
            break;
          case 6:
            //menuItem7();
            break;
          case 7:
            //menuItem8();
            break;
          case 8:
            //menuItem9();
            break;
          case 9:
            //menuItem10();
            break;
        }
        buttons = 0;
        mainMenuDraw();  //  show submenu as if main menu; no, need another function with array for the submenu entries!!!
        drawCursor();
    }
    
    }
    if (buttons & BUTTON_LEFT) {  // no function in the main menu level
      buttons = 0;
    }
    
    if (buttons & BUTTON_SELECT) {  //  not used yet
      buttons = 0;
    }
      
  }


//void sub_menu_item1() { // Function executes when you select the 1st item from main menu
//  int activeButton = 0; //  allows to go back to main menu
//  lcd.clear();
//  lcd.setCursor(3, 0);
//  lcd.print("Sub Menu 1");
//
//  while (activeButton == 0) {
//    int button;
//    readKey = analogRead(0);
//    if (readKey < 790) {
//      delay(100);
//      readKey = analogRead(0);
//    }
//    button = evaluateButton(readKey);
//    switch (button) {
//      case 4:  // This case will execute if the "back" button is pressed
//        button = 0;
//        activeButton = 1;
//        break;
//    }
//  }
//}

  
//void operateMainMenu() {
//  int activeButton = 0;
//  while (activeButton == 0) {
//    int button;
//    readKey = analogRead(0);
//    if (readKey < 790) {
//      delay(100); //  debouncing
//      readKey = analogRead(0);
//    }
//    button = evaluateButton(readKey);
//    switch (button) {
//      case 0: // When button returns as 0 there is no action taken
//        break;
//      case 1:  // This case will execute if the "forward" button is pressed
//        button = 0;
//        switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
//          case 0:
//            menuItem1();
//            break;
//          case 1:
//            menuItem2();
//            break;
//          case 2:
//            menuItem3();
//            break;
//          case 3:
//            menuItem4();
//            break;
//          case 4:
//            menuItem5();
//            break;
//          case 5:
//            menuItem6();
//            break;
//          case 6:
//            menuItem7();
//            break;
//          case 7:
//            menuItem8();
//            break;
//          case 8:
//            menuItem9();
//            break;
//          case 9:
//            menuItem10();
//            break;
//        }
//        activeButton = 1;
//        mainMenuDraw();
//        drawCursor();
//        break;
//      case 2:
//        button = 0;
//        if (menuPage == 0) {
//          cursorPosition = cursorPosition - 1;
//          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
//        }
//        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
//          menuPage = menuPage - 1;
//          menuPage = constrain(menuPage, 0, maxMenuPages);
//        }
//
//        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
//          menuPage = menuPage - 1;
//          menuPage = constrain(menuPage, 0, maxMenuPages);
//        }
//
//        cursorPosition = cursorPosition - 1;
//        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
//
//        mainMenuDraw();
//        drawCursor();
//        activeButton = 1;
//        break;
//      case 3:
//        button = 0;
//        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
//          menuPage = menuPage + 1;
//          menuPage = constrain(menuPage, 0, maxMenuPages);
//        }
//
//        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
//          menuPage = menuPage + 1;
//          menuPage = constrain(menuPage, 0, maxMenuPages);
//        }
//
//        cursorPosition = cursorPosition + 1;
//        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
//        mainMenuDraw();
//        drawCursor();
//        activeButton = 1;
//        break;
//    }
//  }
//}
//
//// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
//int evaluateButton(int x) {
//  int result = 0;
//  if (x < 50) {
//    result = 1; // right
//  } else if (x < 195) {
//    result = 2; // up
//  } else if (x < 380) {
//    result = 3; // down
//  } else if (x < 790) {
//    result = 4; // left
//  }
//  return result;
//}
//
//// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
//// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
//// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.
//void drawInstructions() {
//  lcd.setCursor(0, 1); // Set cursor to the bottom line
//  lcd.print("Use ");
//  lcd.print(byte(1)); // Up arrow
//  lcd.print("/");
//  lcd.print(byte(2)); // Down arrow
//  lcd.print(" buttons");
//}
//
//void menuItem1() { // Function executes when you select the 1st item from main menu
//  int activeButton = 0;
//
//  lcd.clear();
//  lcd.setCursor(3, 0);
//  lcd.print("Sub Menu 1");
//
//  while (activeButton == 0) {
//    int button;
//    readKey = analogRead(0);
//    if (readKey < 790) {
//      delay(100);
//      readKey = analogRead(0);
//    }
//    button = evaluateButton(readKey);
//    switch (button) {
//      case 4:  // This case will execute if the "back" button is pressed
//        button = 0;
//        activeButton = 1;
//        break;
//    }
//  }
//}
//
//void menuItem2() { // Function executes when you select the 2nd item from main menu
//  int activeButton = 0;
//
//  lcd.clear();
//  lcd.setCursor(3, 0);
//  lcd.print("Sub Menu 2");
//
//  while (activeButton == 0) {
//    int button;
//    readKey = analogRead(0);
//    if (readKey < 790) {
//      delay(100);
//      readKey = analogRead(0);
//    }
//    button = evaluateButton(readKey);
//    switch (button) {
//      case 4:  // This case will execute if the "back" button is pressed
//        button = 0;
//        activeButton = 1;
//        break;
//    }
//  }
//}

