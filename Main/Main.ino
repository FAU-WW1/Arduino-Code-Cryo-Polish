//#include <Wire.h> // Fundamentals like SPI   
#include <SPI.h>
#include <SD.h>

// Thermoelement Breakout Board:
#include <Adafruit_MAX31856.h>
// Use software SPI: CS, DI, DO, CLK
// Adafruit_MAX31856 max = Adafruit_MAX31856(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
// above SPI Pins stay the same for Uno Board with normal Pinout
Adafruit_MAX31856 T_specimen = Adafruit_MAX31856(10);
Adafruit_MAX31856 T_loop = Adafruit_MAX31856(9);

// LCD Shield:   
#include <Adafruit_RGBLCDShield.h>      
#include <utility/Adafruit_MCP23017.h>
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
// define custom chracter for active heating glass
byte heatsign[8] = {
  0b01000, //  *
  0b01100, //  **
  0b00110, //   **
  0b00110, //   **
  0b01100, //  **
  0b01100, //  **
  0b00110, //   **
  0b00010  //    *
};
byte blank[8] = {
  0b00000, //  
  0b00000, //  
  0b00000, //   
  0b00000, //   
  0b00000, //  
  0b00000, //  
  0b00000, //   
  0b00000  //    
};

// Relais:
#define RELAIS_PIN_GLASS 2
#define RELAIS_PIN_CIRCUIT 4
bool heating_glass_active = false;

// NeoPixel Ring:
#include <Adafruit_NeoPixel.h>
#define PIN 5 // PWM Pin
#define NUMPIXELS 24  // number of pixels in ring
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

/*
 * 
 */
void setup() {
  pinMode(RELAIS_PIN_GLASS, OUTPUT);
  pinMode(RELAIS_PIN_CIRCUIT, OUTPUT);

  ring.begin(); // initializes NeoPixel
  ring.show();  // sets all pixel to off at start
  for (uint8_t i=0; i<NUMPIXELS; i++) {  // small startup demo to show connection
    ring.setPixelColor(i, ring.Color(0,0,0,255));
    ring.show();  // send data to pixels
    delay(50);
  }
  for (uint8_t i=0; i<NUMPIXELS; i++) {
    ring.setPixelColor(i, ring.Color(0,0,0,0));
    ring.show();  // send data to pixels
  }

  Serial.begin(115200);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  Serial.println("MAX31856 thermocouple test");
  lcd.setBacklight(TEAL);
  // Create byte for custom character
  lcd.createChar(0, heatsign);
  T_specimen.begin();
  T_loop.begin();

  T_specimen.setThermocoupleType(MAX31856_TCTYPE_K);
  T_loop.setThermocoupleType(MAX31856_TCTYPE_K);

  Serial.print("Thermocouple type1: ");
  switch ( T_specimen.getThermocoupleType() ) {
    case MAX31856_TCTYPE_B: Serial.println("B Type"); break;
    case MAX31856_TCTYPE_E: Serial.println("E Type"); break;
    case MAX31856_TCTYPE_J: Serial.println("J Type"); break;
    case MAX31856_TCTYPE_K: Serial.println("K Type"); break;
    case MAX31856_TCTYPE_N: Serial.println("N Type"); break;
    case MAX31856_TCTYPE_R: Serial.println("R Type"); break;
    case MAX31856_TCTYPE_S: Serial.println("S Type"); break;
    case MAX31856_TCTYPE_T: Serial.println("T Type"); break;
    case MAX31856_VMODE_G8: Serial.println("Voltage x8 Gain mode"); break;
    case MAX31856_VMODE_G32: Serial.println("Voltage x8 Gain mode"); break;
    default: Serial.println("Unknown"); break;
  }

}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print("spec ");
  lcd.print(T_specimen.readThermocoupleTemperature(),1); //  one digit for float number
  
  lcd.setCursor(0, 1);
  lcd.print("loop ");
  int time = millis();
  float temp_test = T_loop.readThermocoupleTemperature();
  int delta = millis() - time;
  lcd.print(T_loop.readThermocoupleTemperature(),1); //  one digit for float number
  
  Serial.println(delta);

  uint8_t buttons = lcd.readButtons();  // value for specific button see .h file
  if (buttons) {
    //lcd.clear();
    lcd.setCursor(0,0);
    
    if (buttons & BUTTON_SELECT) {  // switch button
      heating_glass_active = !heating_glass_active; // invert booelan
      if (heating_glass_active == true) {
        digitalWrite(RELAIS_PIN_GLASS, HIGH);
        lcd.print("heating on");
        delay(500);  // delay for user visibility
        lcd.clear();  // remove letters again with whole whipe
        lcd.setCursor(14,0);
        lcd.write(byte(0));
        lcd.setCursor(15,0);
        lcd.write(byte(0));
      }
      if (heating_glass_active == false) {
        digitalWrite(RELAIS_PIN_GLASS, LOW);
        lcd.print("heating off");
        delay(500);  // delay for user visibility
        lcd.clear();  // remove letters again with whole wipe
      }    
    }
    if (buttons & BUTTON_LEFT) {  // hold button closing circuit
      digitalWrite(RELAIS_PIN_CIRCUIT, HIGH);
      }
    if (buttons & BUTTON_UP) {  // switch on leds
      led_on();
      lcd.print("led on        ");
      delay(500);  // delay for user visibility
      //lcd.clear(); // erase letters before showing temp again
      lcd.setCursor(0,0);
      lcd.print("      ");
    }
    if (buttons & BUTTON_DOWN) {  // switch off leds
      led_off();
      lcd.print("led off       ");
      delay(500);  // delay for user visibility
      //lcd.clear(); // erase letters before showing temp again
      lcd.setCursor(0,0);
      lcd.print("       ");
    }  
  }
  else {  // open cirucit if left button is no longer pressed
    digitalWrite(RELAIS_PIN_CIRCUIT, LOW);
  }
}




/* 
  // Check and print any faults
  uint8_t fault = T_specimen.readFault();
  if (fault) {
    if (fault & MAX31856_FAULT_CJRANGE) Serial.println("Cold Junction Range Fault");
    if (fault & MAX31856_FAULT_TCRANGE) Serial.println("Thermocouple Range Fault");
    if (fault & MAX31856_FAULT_CJHIGH)  Serial.println("Cold Junction High Fault");
    if (fault & MAX31856_FAULT_CJLOW)   Serial.println("Cold Junction Low Fault");
    if (fault & MAX31856_FAULT_TCHIGH)  Serial.println("Thermocouple High Fault");
    if (fault & MAX31856_FAULT_TCLOW)   Serial.println("Thermocouple Low Fault");
    if (fault & MAX31856_FAULT_OVUV)    Serial.println("Over/Under Voltage Fault");
    if (fault & MAX31856_FAULT_OPEN)    Serial.println("Thermocouple Open Fault");
  }
*/

void led_on() {
  for(uint8_t i=0; i<NUMPIXELS; i++) {
    ring.setPixelColor(i, ring.Color(0,0,0,255));
    ring.show();  // send data to pixels
  }
}

void led_off() {
  for(uint8_t i=0; i<NUMPIXELS; i++) {
    ring.setPixelColor(i, ring.Color(0,0,0,0));
    ring.show();  // send data to pixels
  }
}


