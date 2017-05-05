#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int RXPin = 7;
const int TXPin = 8;

TinyGPSPlus gps;

SoftwareSerial gpsSerial(RXPin, TXPin);

static double latitude = 0.0;
static double longitude = 0.0;

RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

// double that holds gps coordinates
static double gpsCord_Transmit[2] = {1234.1234, -7894.1234};

void setup() {
  gpsSerial.begin(4800);
  Serial.begin(57600);
  radio.begin();
  radio.setRetries(15, 15);

  // This is used for the beacon to start writing to the pipe
  radio.openWritingPipe(pipe);

  // This is used for the rover to read what ever is sent on the pipe
  //radio.openReadingPipe(1,pipes);

  // Makes it so it's always listening
  //radio.startListening();

  radio.printDetails();
  //updateGPS();
  //delay(1000);
//  /radioFunc();
}

void loop() {
  radioFunc();
}

void updateGPS() {
  if (gpsSerial.available() > 0 ) {
    if (gps.encode(gpsSerial.read())) {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      gpsCord_Transmit[0] = latitude;
      gpsCord_Transmit[1] = longitude;
    }
  }
}


void radioFunc() {
  Serial.println("Now sending...");
  gpsCord_Transmit[0] += 0.1; 
  bool ok = radio.write( &gpsCord_Transmit, sizeof(gpsCord_Transmit));
  if (ok)
    Serial.println("ok\n\r");
  else
    Serial.println("failed\n\r");
  delay(100);
}

Skidmore, Gabe
1:14 PM (1 hour ago)

to me 
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

double testData[2];
RF24 radio(9, 10);

const uint64_t pipe = 0xE8E8F0F0E1LL;
void setup() {
  Serial.begin(57600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  radio.printDetails();
}

void loop() {
  if ( radio.available() ) {
    // Dump the payloads until we've gotten everything
    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read(&testData, sizeof(testData));
      for (int i = 0; i < 2; i++) {
        Serial.print(testData[i]);
        Serial.print(", ");
      }
      Serial.println();
    }
  }
  else {
    Serial.print("Failed");
  }
  delay(20);
}
