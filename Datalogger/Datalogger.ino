/*
  SD card datalogger

 This example shows how to log data from three analog sensors
 to an SD card using the SD library.

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>

File dataFile; // creates File Object

const int CCS = 7; // Card Chip Select for SPI

int Input1 = A0;
int Input2 = A1;

long seconds = 0;


void setup() {
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(CCS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  

}

void loop() {
  // make a string for assembling the data to log, overwrite it with each loop
  String dataString = "";
  dataString += String(seconds);
  dataString += ",";
  dataString += String(analogRead(Input1));
  dataString += ",";
  dataString += String(analogRead(Input2));

  seconds ++;
//  // read three sensors and append to the string:
//  for (int analogPin = 0; analogPin < 3; analogPin++) {
//    int sensor = analogRead(analogPin);
//    dataString += String(sensor);
//    if (analogPin < 2) {
//      dataString += ",";
//    }
//  }
  
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
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }
  delay(1000);
}









