// Libraries ============================
#include <Wire.h> //  Fundamentals like SPI   
//#include <SPI.h>

// SD Lib:
#include <SD.h>

// Thermoelement Breakout Board:
#include <Adafruit_MAX31856.h>

// LCD Shield with Buttons:
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

// LCD Touchscreen Breakout Board:
#include <Adafruit_GFX.h> //  Core graphics library
#include <Adafruit_ILI9341.h>    // Driver chip display
#include "TouchScreen.h"  //  Resistive Touch Screen

// NeoPixel Ring:
#include <Adafruit_NeoPixel.h>

// Global Pinout =========================
/*
  Pin Setting for Wiring
  Arduino Uno                 device
  5V
  GND
  A0 
  A1
  A2  Resistive Touch Y+
  A3  Resistive Touch X-
  Fix I2C Pins:
  A4 (SCL)  Display 16x2
  A5 (SDA)  Display 16x2
  D0  Relais#1 circuit
  D1  Relais#2 heating glass
  D2  Resistive Touch Y-
  D3 (PWM)  Neopixel Ring
  D4  Resistive Touch X+
  D6  D/C TFT Display = X+ Resistive Touch
  SPI Chip Selects (CS):
  D7  MAX31856#1
  D8  MAX31856#2
  D9  SD Card (integrated in Display)
  D10 TFT Display
  Fix SPI Pins:
  D11 DI (Data In)  MAX31856#1  MAX31856#2  TFT Display
  D12 DO (Dato Out) MAX31856#1  MAX31856#2  TFT Display
  D13 CLK (Clock)   MAX31856#1  MAX31856#2  TFT Display
  
  Fixed SPI Pins for Uno Boards:
  DI(Data in on devices)/MOSI = 11
  DO(Data out on devices)/MISO = 12
  CLK(clock) = 13
*/

// Resistive Touch:
#define YP A2 //  Y+ touch direction, requires analog pin: "A"
#define XM A3 //  X- touch direction, requires analog pin: "A"
#define YM 8  //  Y- can be a digital pin
#define XP 9  //  X+ can be a digital pin; equals DC!
// LCD Touch Display:
#define TFT_CS 10 //  SPI Chip Select Display
#define TFT_DC 9  //  equals X+
//#define TFT_RST 8 // RST can be set to -1 if you tie it to Arduino's reset
#define CARD_CS 7 // SPI Chip Select integrated SD card

// Thermoelement Breakout Board:
#define MAX_SPEC_CS 7
#define MAX_LOOP_CS 8

// Relais:
#define RELAIS_PIN_GLASS 1

// NeoPixel Ring:
#define PIN 3 // PWM required



// Object Definitions and corresponding variables ================

// SD Card for Data Logging:
File dataFile; // creates File Object
bool log_active = false; // begin and end logging with specific buttons
unsigned long currentMillis = 0; // time variables for logging and or graphing
unsigned long previousMillis = 0;
unsigned long interval = 1000; // interval in ms for plotting and data saving on sd

// Thermoelement Breakout Board SPI Chip Selects:
Adafruit_MAX31856 T_specimen = Adafruit_MAX31856(MAX_SPEC_CS);
Adafruit_MAX31856 T_loop = Adafruit_MAX31856(MAX_LOOP_CS);
double current_T_loop;
double current_T_specimen;

// LCD  Button Shield:
// uses I2C with fixed Pins SCL = A4 and SDA = A5
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
// define custom chracter for active heating glass
byte heatsign[8] = {
  0b01000, //  *
  0b01100, //  **
  0b00110, //   **
  0b00110, //   **
  0b01100, //  **
  0b01100, //  **
  0b00110, //   **
  0b11111  // *****
};

// Touchscreen Definitions:
//#define TS_MINX 150
//#define TS_MINY 120
//#define TS_MAXX 920
//#define TS_MAXY 940
//#define MINPRESSURE 5
//#define MAXPRESSURE 1000
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Color Definitions to chose from for both displays:
#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN   0xBFF7
#define LTCYAN    0xC7FF
#define LTRED     0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW  0xFFF8
#define LTORANGE  0xFE73
#define LTPINK    0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY    0xE71C

#define BLUE      0x001F
#define TEAL      0x0438
#define GREEN     0x07E0
#define CYAN      0x07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFC00
#define PINK      0xF81F
#define PURPLE    0x8010
#define GREY      0xC618
#define WHITE     0xFFFF
#define BLACK     0x0000

#define DKBLUE    0x000D
#define DKTEAL    0x020C
#define DKGREEN   0x03E0
#define DKCYAN    0x03EF
#define DKRED     0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW  0x8400
#define DKORANGE  0x8200
#define DKPINK    0x9009
#define DKPURPLE  0x4010
#define DKGREY    0x4A49

// LCD Touch Display:
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
boolean display1 = true;  //  redraw coordinate system on new screen?
double ox , oy, oy2 ; // base values for incremental line drawing within the graph

// diagram argument list, define plot style here:
double  y, y2; // initialize x, y and y2
int x = 0 ; // seconds as x values combine with long from logging sd card
//int s = 0; 
int xrange = 60; // range of x values on one screen
int gx = 40; // x graph base location (lower left corner) relative to upper left corner of display
int gy = 210; // y graph base location (lower left corner) relative to upper left corner of display
int w = 250; // width of graph
int h = 200; // height of graph
int xcapright = x + xrange; // stores previous x bounds
//int xcapright = s + xrange; // stores previous x bounds
int xcapleft = x; // stores previous x bounds, start with 0
//int xcapleft = s; // stores previous x bounds, start with 0
int xinc = 10; // division of x axis
int ylo = -100; // lower bound of y axis
int yhi = 20; // upper bound of y axis
int yinc = 10; // division of y axis
String title = "Temperatures";
String xlabel = "Time [s]";
String ylabel = "Temperature [Celsius]";
String yname1 = "loop"; // data name  1 for diagram legend
String yname2 = "specimen"; // data name 2 for diagram legend
unsigned int gcolor = RED; // graph background color
unsigned int acolor = RED; // axis line color
unsigned int pcolor1 = GREEN; // y plot color same as legend
unsigned int pcolor2 = CYAN; // y2 plot color same as legend
unsigned int tcolor = WHITE; // text color
unsigned int bcolor = BLACK; // background color text boxes

// Relais:
bool heating_glass_active = false;

// NeoPixel Ring:
#define NUMPIXELS 24  // number of pixels in ring
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);



// Setup =======================================================================
void setup() {
  // LCD Touch Display:
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(3); //  horizontal with SPI pins on the left
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // see if the card is present and can be initialized:
  if (!SD.begin(CARD_CS)) {
    tft.setTextSize(2);
    tft.setTextColor(RED, BLACK);
    tft.setCursor(100, 100);
    tft.println("Card failed");
    //Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  tft.setTextSize(2);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(100, 100);
  tft.println("Card initialized");
  delay(1000);
  tft.fillScreen(BLACK);

  // I/O Pin Definitions:
  // Demo Sensors, replace with MAX values!!!!!!!!!!
 // pinMode(INPUT1, INPUT);
 // pinMode(INPUT2, INPUT);
  pinMode(RELAIS_PIN_GLASS, OUTPUT);
//  pinMode(RELAIS_PIN_CIRCUIT, OUTPUT);

  // Neopixel Ring:
  ring.begin(); // initializes NeoPixel
  ring.show();  // sets all pixel to off at start
//  startup_demo(); // small startup demo to show connection

  // LCD Shield with Buttons:
  lcd.begin(16, 2);
  lcd.setBacklight(TEAL);
  // Create byte for custom character
  lcd.createChar(0, heatsign);

  // Max Breakout Boards:
  T_specimen.begin();
  T_loop.begin();
  T_specimen.setThermocoupleType(MAX31856_TCTYPE_K);
  T_loop.setThermocoupleType(MAX31856_TCTYPE_K);

  
}


// Loop ========================================================================
void loop() {

  // Button Interface:
  uint8_t buttons = lcd.readButtons();  // value for specific button see .h file
  if (buttons) {
    lcd.setCursor(0, 0);

    if (buttons & BUTTON_SELECT) {  // switch button
      heating_glass_active = !heating_glass_active; // invert boolean
      if (heating_glass_active == true) {
        digitalWrite(RELAIS_PIN_GLASS, HIGH);
        lcd.print("heating on");
        delay(500);  // delay for user visibility
        lcd.clear();  // remove letters again with complete wipe
        lcd.setCursor(14, 0);
        lcd.write(byte(0));
        lcd.setCursor(15, 0);
        lcd.write(byte(0));
      }
      if (heating_glass_active == false) {
        digitalWrite(RELAIS_PIN_GLASS, LOW);
        lcd.print("heating off");
        delay(500);  // delay for user visibility
        lcd.clear();  // remove heat signs again with whole wipe
      }
    }
    if (buttons & BUTTON_LEFT) {  // start logging
      log_active = true;
      tft.fillScreen(BLACK);
      display1 = true; // redraw and create new graph at start of logging
      // set time and time borders back to start: x s xcapright xcapleft
      x = 0;
      xcapleft = x;
      xcapright = x + xrange;
      lcd.print("logging       ");
      delay(500);  // delay for user visibility
      lcd.setCursor(0, 0); // remove message
      lcd.print("       ");
    }
    if (buttons & BUTTON_RIGHT) {  // stop logging
      log_active = false;
      lcd.print("logging end   ");
      delay(500);  // delay for user visibility
      lcd.setCursor(0, 0); // remove message
      lcd.print("           ");
    }
    if (buttons & BUTTON_UP) {  // switch on leds
      led_on();
      lcd.print("led on        ");
      delay(500);  // delay for user visibility
      lcd.setCursor(0, 0); // remove message
      lcd.print("      ");
    }
    if (buttons & BUTTON_DOWN) {  // switch off leds
      led_off();
      lcd.print("led off       ");
      delay(500);  // delay for user visibility
      lcd.setCursor(0, 0); // remove message
      lcd.print("       ");
    }
  }


// Perform logging and or graphing every given interval e.g.each second
// keep on graphing every second no matter what, refresh screen and begin with 0s when logging starts see button section
// start logging only when selected

  
  currentMillis = millis(); // millis() returns an unsigned long!
  if (currentMillis - previousMillis >= interval) {
    unsigned long t_offset = currentMillis - previousMillis; // check for time offset if other tasks take too long to execute
    Serial.println(t_offset);
    previousMillis = currentMillis; // immedeately overwrite variable
    // for small enough t_offset just add 1 to time variable, this also allows to start from 0s at logging start
    x += interval/1000; // rough method
    
    // permanent graphing on touch and displaying current temp on lcd at given interval
    // read each thermocouple just once due to conversion times and use variable for later actions, overwrite within each interval
    current_T_loop = T_loop.readThermocoupleTemperature();
    current_T_specimen = T_specimen.readThermocoupleTemperature();

    // lcd display with current temperatures:
    // set the cursor to column 0, line 1; note: line 1 is the second row, since counting begins with 0:
    lcd.setCursor(0, 0);
    lcd.print("spec ");
    lcd.print(current_T_specimen, 1); //  one digit float number
    lcd.setCursor(0, 1);
    lcd.print("loop ");
    lcd.print(current_T_loop, 1); //  one digit float number

    // Graph plotting on touch display:
    y = current_T_loop;
    y2 = current_T_specimen;
    //Serial.println(y);
    //Serial.println(y2);

    /* remaining arguments for graph function:
      tft = name of display object
      x = x data point
      y = y data point
      y2 = y2 data point
      &redraw = flag to redraw graph on fist call only, pointer to display
    */
    Graph(tft, x, y, y2, gx, gy, w, h, xcapleft, xcapright, xinc, ylo, yhi, yinc, title, xlabel, ylabel, yname1, yname2, gcolor, acolor, pcolor1, pcolor2, tcolor, bcolor, display1);

    if (x == xcapright) { // overwrite x axis bounds for new screen labelling:
      //s = s + xrange;
      xcapright = xcapright + xrange;
      xcapleft = xcapleft + xrange;
      tft.fillScreen(BLACK);
      display1 = true; // redraws axis for new plot on fresh screen
    }

    // SD Logging:  
    if (log_active == true) {
      // make a string for assembling the data to log, overwrite String with each loop
      String dataString = "";
      dataString += String(x); // x stores current seconds
      dataString += ",";
      dataString += String(current_T_specimen);
      dataString += ",";
      dataString += String(current_T_loop);

      // automatic filenames see example scetch sdfat lib
    
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      // open creates new file or opens a prexisiting one
      dataFile = SD.open("test.txt", FILE_WRITE); // argument FILE_WRITE makes it writable and readable
    
      // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        //Serial.println(dataString);
      }
      // if the file isn't open, pop up an error:
      else {
        //Serial.println("error opening file");
      }
    }
  }
}
    


// Functions ============================================================
// Graph Plotting
void Graph(Adafruit_ILI9341 &d, int x, double y, double y2, int gx, int gy, int w, int h, int xlo, int xhi, int xinc, int ylo, int yhi, int yinc, String title, String xlabel, String ylabel, String yname1, String yname2, unsigned int gcolor, unsigned int acolor, unsigned int pcolor1, unsigned int pcolor2, unsigned int tcolor, unsigned int bcolor, boolean &redraw) {

  double ydiv, xdiv;
  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  //static double ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
  //static double oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  double i;
  double temp;
  int rot, newrot;

  if (redraw == true) {

    redraw = false;
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    oy2 = (y2 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) { // on axis
        d.drawLine(gx, temp, gx + w, temp, acolor);
      }
      else { // on graph
        d.drawLine(gx, temp, gx + w, temp, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(gx - 40, temp);
      // precision is default Arduino--this could really use some format control
      // round for °C here
      d.println(int(i));
      // d.println(i);
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform

      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawLine(temp, gy, temp, gy - h, acolor);
      }
      else {
        d.drawLine(temp, gy, temp, gy - h, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(temp, gy + 10);
      // precision is default Arduino--this could really use some format control
      // round for seconds here
      d.println(int(i));
      // d.println(i);
    }

    //now draw the labels
//    d.setTextSize(2);
//    d.setTextColor(tcolor, bcolor);
//    d.setCursor(gx , gy - h - 30);
//    d.println(title);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx + 200 , gy + 20);
    d.println(xlabel);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx - 40, gy - h - 10);
    d.println(ylabel);

    d.setTextSize(1);
    d.setTextColor(pcolor1, bcolor);
    d.setCursor(gx + 150, gy - h - 10);
    d.println(yname1);

    d.setTextSize(1);
    d.setTextColor(pcolor2, bcolor);
    d.setCursor(gx + 200, gy - h - 10);
    d.println(yname2);

  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above ?????????????????? no they are not
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  y2 = (y2 - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox, oy, x, y, pcolor1);
  // broadens plot line
  //d.drawLine(ox, oy + 1, x, y + 1, pcolor1);
  //d.drawLine(ox, oy - 1, x, y - 1, pcolor1);

  // broadens plot line
  d.drawLine(ox, oy2, x, y2, pcolor2);
  //d.drawLine(ox, oy2 + 1, x, y2 + 1, pcolor2);
  //d.drawLine(ox, oy2 - 1, x, y2 - 1, pcolor2);
  ox = x;
  oy = y;
  oy2 = y2;
}

// Neopixel Functions:
void led_on() {
  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    ring.setPixelColor(i, ring.Color(0, 0, 0, 255));
    ring.show();  // send data to pixels
  }
}
void led_off() {
  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    ring.setPixelColor(i, ring.Color(0, 0, 0, 0));
    ring.show();  // send data to pixels
  }
}

/*
void startup_demo() { // small startup demo to show connection
  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    ring.setPixelColor(i, ring.Color(0, 0, 0, 255));
    ring.show();  // send data to pixels
    delay(50);
  }
  for (uint8_t i = 0; i < NUMPIXELS; i++) { //  shut off again
    ring.setPixelColor(i, ring.Color(0, 0, 0, 0));
    ring.show();  // send data to pixels
  }
}
*/

