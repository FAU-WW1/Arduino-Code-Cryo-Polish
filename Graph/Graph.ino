/*

This program provides cartesian type graph function

It requires and Arduino Mega (or UNO) and an Adafruit 3.5" TFT 320x480 + Touchscreen Breakout Board
https://learn.adafruit.com/adafruit-3-5-color-320x480-tft-touchscreen-breakout/overview

Adafruit libraries
https://github.com/adafruit/Adafruit_HX8357_Library/archive/master.zip
https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip

optional touch screen libraries
https://github.com/adafruit/Touch-Screen-Library/archive/master.zip

Revisions
rev     date        author      description
1       12-24-2015  kasprzak    initial creation


This pin setting will also operate the SD card

Pin settings

  Arduino   device
  5V        Vin
  GND       GND
  A0
  A1
  A2         Y+ (for touch screen use)
  A3         X- (for touch screen use)
and so on

*/
#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>

//#include "TouchScreen.h"
// These are the four touchscreen analog pins
//#define YP A2  // must be an analog pin, use "An" notation!
//#define XM A3  // must be an analog pin, use "An" notation!
//#define YM 8   // can be a digital pin
//#define XP 9   // can be a digital pin equals DC!
//
//#define TS_MINX 150
//#define TS_MINY 120
//#define TS_MAXX 920
//#define TS_MAXY 940
//
//#define MINPRESSURE 5
//#define MAXPRESSURE 1000

// These are 'flexible' lines that can be changed
/* The display uses hardware SPI plus #9 & #10:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
**
 */
#define TFT_CS 10 // SPI Chip Select
#define TFT_DC 9
//#define TFT_RST 8 // RST can be set to -1 if you tie it to Arduino's reset
#define SD_CCS 7 // Card Chip Select for SPI access to sd card

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


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


#define ADJ_PIN A0 //  demo sensor for live graphing

double a1, b1, c1, d1, r2, r1, vo, tempC, tempF, tempK;



// this is the only external variable used by the graph
// it's a flat to draw the coordinate system only on the first pass
boolean display1 = true;
boolean display2 = true;
boolean display3 = true;
boolean display4 = true;
boolean display5 = true;
boolean display6 = true;
boolean display7 = true;
boolean display8 = true;
boolean display9 = true;
double ox , oy ;

boolean tft_state = true;

void setup() {

  Serial.begin(9600);

  pinMode(ADJ_PIN, INPUT);
  tft.begin();
  tft.fillScreen(BLACK);

  tft.setRotation(2);
  a1 = 3.354016E-03 ;
  b1 = 2.569850E-04 ;
  c1 = 2.620131E-06 ;
  d1 = 6.383091E-08 ;


 double x, y;


  tft.setRotation(3);


//  for (x = 0; x <= 6.3; x += .1) {
//
//    y = sin(x);
//    Graph(tft, x, y, 60, 290, 390, 260, 0, 6.5, 1, -1, 1, .25, "Sin Function", "x", "sin(x)", DKBLUE, RED, YELLOW, WHITE, BLACK, display1);
//
//  }
//
//  delay(1000);
//
//  tft.fillScreen(BLACK);
//  for (x = 0; x <= 6.3; x += .1) {
//
//    y = sin(x);
//    Graph(tft, x, y, 100, 280, 100, 240, 0, 6.5, 3.25, -1, 1, .25, "Sin Function", "x", "sin(x)", GREY, GREEN, RED, YELLOW, BLACK, display9);
//
//  }
//
//    delay(1000);
//
//  tft.fillScreen(BLACK);
//  for (x = 0; x <= 25.2; x += .1) {
//
//    y = sin(x);
//    Graph(tft, x, y, 50, 190, 400, 60, 0, 25, 5, -1, 1, .5, "Sin Function", "x", "sin(x)", DKYELLOW, YELLOW, GREEN, WHITE, BLACK, display8);
//
//  }
//
//  delay(1000);
//
//  tft.fillScreen(BLACK);
//  for (x = 0.001; x <= 10; x += .1) {
//
//    y = log(x);
//    Graph(tft, x, y, 50, 240, 300, 180, 0, 10, 1, -10, 5, 1, "Natural Log Function", "x", "ln(x)", BLUE, RED, WHITE, WHITE, BLACK, display2);
//
//  }
//
//  
//
//  delay(1000);
//  tft.fillScreen(BLACK);
//
//  for (x = 0; x <= 10; x += 1) {
//
//    y = x * x;
//    Graph(tft, x, y, 50, 290, 390, 260, 0, 10, 1, 0, 100, 10, "Square Function", "x", "x^2", DKRED, RED, YELLOW, WHITE, BLACK, display3);
//
//  }
//
//  delay(1000);
//  tft.fillScreen(BLACK);
//
//  for (x = 0.00; x <= 20; x += .01) {
//
//    y = ((sin(x)) * x + cos(x)) - log(x);
//    Graph(tft, x, y, 50, 290, 390, 260, 0, 20, 1, -20, 20, 5, "Weird Function", "x", " y = sin(x) + cos(x) - log(x)", ORANGE, YELLOW, CYAN, WHITE, BLACK, display4);
//
//  }
//
//  delay(1000);
//  tft.fillScreen(BLACK);
//  tft.setRotation(2);
//  for (x = 0; x <= 12.6; x += .1) {
//
//    y = sin(x);
//    Graph(tft, x, y, 50, 250, 150, 150, 0, 13, 3.5, -1, 1, 1, "Sin(x)", "x", "sin(x)", DKBLUE, RED, YELLOW, WHITE, BLACK, display5);
//
//  }
//  tft.setRotation(3);
//  delay(1000);
//  tft.fillScreen(WHITE);
//
//  for (x = 0; x <= 6.3; x += .05) {
//
//    y = cos(x);
//    Graph(tft, x, y, 100, 250, 300, 200, 0, 6.5, 3.25, -1, 1, 1, "Cos Function", "x", "cos(x)", DKGREY, GREEN, BLUE, BLACK, WHITE, display6);
//
//  }
//
//  delay(1000);
//  tft.fillScreen(BLACK);

while(5 > 0){
  for (x = 0; x <= 60; x += 1) {
    y = analogRead(ADJ_PIN) / 100;


    Graph(tft, x, y, 50, 210, 240, 170, 0, 60, 10, 0, 20, 5, "Input Voltage", " Time [s]", "Temperature [deg F]", DKBLUE, RED, GREEN, WHITE, BLACK, display7);
    delay(250);
  }

 delay(1000);
  tft.fillScreen(BLACK);
  Serial.println(display7);
  display7 = true;
  Serial.println(display7);
  delay(3000);
}



}


void loop(void) {
//  uint16_t i = millis()/1000;
//  double y;
//  for (i = 0; i <= 60; i += 1) {
//    y = analogRead(ADJ_PIN) / 300;
//
//
//    Graph(tft, i, y, 50, 230, 240, 170, 0, 60, 10, 70, 90, 5, "Input Voltage", " Time [s]", "Temperature [deg F]", DKBLUE, RED, GREEN, WHITE, BLACK, display7);
//    delay(250);
//  }
//
// delay(1000);
//  tft.fillScreen(BLACK);
}

/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn

  huge arguement list
  &d name of your display object
  x = x data point
  y = y datapont
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x asis label
  ylabel = y asis label
  gcolor = graph line colors
  acolor = axi ine colors
  pcolor = color of your plotted data
  tcolor = text color
  bcolor = background color
  &redraw = flag to redraw graph on fist call only
*/


void Graph(Adafruit_ILI9341 &d, double x, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean &redraw) {

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
      d.println(i);
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
      d.println(i);
    }

    //now draw the labels
    d.setTextSize(2);
    d.setTextColor(tcolor, bcolor);
    d.setCursor(gx , gy - h - 30);
    d.println(title);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx , gy + 20);
    d.println(xlabel);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx - 30, gy - h - 10);
    d.println(ylabel);


  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox, oy, x, y, pcolor);
  d.drawLine(ox, oy + 1, x, y + 1, pcolor);
  d.drawLine(ox, oy - 1, x, y - 1, pcolor);
  ox = x;
  oy = y;

}

/*
  End of graphing functioin
*/





