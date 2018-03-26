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


#define INPUT1 A0 //  demo sensor for live graphing
#define INPUT2 A1

// boolean to redraw coordinate system on new screen
boolean display1 = true;

double ox , oy, oy2 ; // base values for incremental line drwaing within the graph

  // diagram argument list:
  double  y, y2; // initialize x, y and y2
  double x = 0 ;
  double s = 0; // seconds as x values
  double xrange = 60; // range of x values on one screen
  double gx = 40; // x graph base location (lower left corner) relative to upper left corner of display
  double gy = 210; // y graph base location (lower left corner) relative to upper left corner of display
  double w = 250; // width of graph
  double h = 200; // height of graph
  double xcapright = s + xrange; // stores previous x bounds
  double xcapleft = s; // stores previous x bounds, start with 0
  double xinc = 10; // division of x axis
  double ylo = -100; // lower bound of y axis
  double yhi = 20; // upper bound of y axis
  double yinc = 10; // division of y axis
  String title = "Temperatures";
  String xlabel = "Time [s]";
  String ylabel = "Temperature [Celsius]";
  String yname1 = "loop"; // name for diagram legend
  String yname2 = "specimen"; // name for diagram legend
  unsigned int gcolor = RED; // graph background color
  unsigned int acolor = RED; // axis line color
  unsigned int pcolor1 = GREEN; // y plot color same as legend
  unsigned int pcolor2 = CYAN; // y2 plot color same as legend
  unsigned int tcolor = WHITE; // text color
  unsigned int bcolor = BLACK; // background color text boxes

  unsigned long previousMillis = 0;
  unsigned long interval = 1000; // interval in ms for plotting and data saving on sd

void setup() {

  Serial.begin(9600); // debug
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(3);

}


void loop() {
  // millis() returns an unsigned long, so define it wright!
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){ // maybe check for offset if other tasks take longer than a second
    previousMillis = currentMillis;
    //x = millis(); // conversion needed from long look it up
    x +=1; // rough test
    Serial.println(millis()); // see how good it is


  //for (x = s; x <= xcapright; x += 1) {
    y = -analogRead(INPUT1) / 10;
    y2 = -analogRead(INPUT2) / 10 + 1;
    Serial.println(y);
    Serial.println(y2);
    
/* remaining arguments for graph function:
  tft = name of display object
  x = x data point
  y = y data point
  y2 = y2 data point
  &redraw = flag to redraw graph on fist call only, pointer to display
*/
    Graph(tft, x, y, y2, gx, gy, w, h, xcapleft, xcapright, xinc, ylo, yhi, yinc, title, xlabel, ylabel, yname1, yname2, gcolor, acolor, pcolor1, pcolor2, tcolor, bcolor, display1);
    //delay(1000);
    Serial.println(millis());
    // improve with millis for exact seconds
  //}
  if (x == xcapright){
  // overwrite x axis bounds for new screen labelling:
  //s = s + xrange;
  
  
  xcapright = xcapright + xrange;
  xcapleft = xcapleft + xrange;
  tft.fillScreen(BLACK);
  display1 = true; // redraws axis for new plot on fresh screen
  }
  }
}

void Graph(Adafruit_ILI9341 &d, double x, double y, double y2, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, String title, String xlabel, String ylabel, String yname1, String yname2, unsigned int gcolor, unsigned int acolor, unsigned int pcolor1,unsigned int pcolor2, unsigned int tcolor, unsigned int bcolor, boolean &redraw) {

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
    d.setCursor(gx +150, gy - h -10);
    d.println(yname1);

    d.setTextSize(1);
    d.setTextColor(pcolor2, bcolor);
    d.setCursor(gx +200, gy - h -10);
    d.println(yname2);

  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
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

