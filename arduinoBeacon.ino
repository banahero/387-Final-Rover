/*
 * The code for the beacon that sends out it's GPS position. 
 * Meant to be picked up by the rover so the rover can travel to the beacon's location
 * The RF24 code is based on some of the examples in the RF24-master library
 */

// These three are from RF24-master library
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// From tinyGPSPlus library
#include <TinyGPS++.h>

//From SoftwareSerial library
#include <SoftwareSerial.h>

// The GPS pins
const int RXPin = 8;
const int TXPin = 7;

// instantiates the variable that checks if GPS was found
bool foundCoord = false;

// Creates the GPS object
TinyGPSPlus gps;

// Sets up SoftwateSerial with the GPS RXPin and TXPin
SoftwareSerial gpsSerial(RXPin, TXPin);

// Instatiates variables for the latitude and longitude
static double latitude = 0.0;
static double longitude = 0.0;

// the hardware configuration for the RF24 on SPI bus pins 9 and 10
RF24 radio(9, 10);

// pipe address for the raadio for one RF24 to transmit and one to receive
const uint64_t pipe = 0xE8E8F0F0E1LL;

// double that holds gps coordinates
double gpsCord_Transmit[2] = {0, 0};

void setup() {
  // Starts the gpsSerial, the Serial and radio
  gpsSerial.begin(4800);
  Serial.begin(57600);
  radio.begin();

  //sets the delay and number of retries
  radio.setRetries(15, 15);

  // This is used for the beacon to start writing to the pipe
  radio.openWritingPipe(pipe);

}

void loop() {
  // checks to see if the GPS coordinate was already found
  if (!foundCoord)
    updateGPS();

  // prints the coordinates to the serial monitor
  for (int i = 0; i < 2; i++) {
    Serial.print(gpsCord_Transmit[i]);
    Serial.print("    ");
  }
  Serial.print("latitude: ");
  Serial.print(latitude, 6);
  Serial.print("   longitude: ");
  Serial.print(longitude, 6);
  Serial.println("Now sending...");

  // transmits the coordinates array and if checks if if it was sent or not
  bool ok = radio.write( &gpsCord_Transmit, sizeof(gpsCord_Transmit));
  if (ok) {
    Serial.println("ok\n\r");
  }
  else
    Serial.println("failed\n\r");
}

/*
   Updates the gps of the beacon, only needs once since beacon doesn't change position
   Updates the gpsCord_Transmit array with the gps coordinates where latitude is first and
   longitude is second
*/
void updateGPS() {
  while ((gpsSerial.available() > 0)) {
    if (gps.encode(gpsSerial.read())) {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      Serial.print(latitude, 6);
      Serial.print(" ");
      Serial.println(longitude, 6);
      gpsCord_Transmit[0] = latitude;
      gpsCord_Transmit[1] = longitude;
      foundCoord = true;
    }
  }
}

