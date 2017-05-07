/*Receiver Code*/
/*GPS SETUP*/
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
RF24 radio(9, 10);
//gpsCoordinates are stored in this array
double gpsCord_Receive[2];
const uint64_t pipe = 0xE8E8F0F0E1LL;


/*Wire code for sender arduino*/
#include <Wire.h>
String passback = "";



const int RXPin = 2; //= BLUE , YELLOW used to be 12
const int TXPin = 3; //= WHITE, ORANGE used to be 13

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

static double rLat = 0.0;
static double rLng = 0.0;
int distance = 0;

static double bLat = 0.0;
static double bLng = 0.0;


/*Rover Sheild setup*/
int rightMotorPin = 5;
int leftMotorPin = 6;
int rightDirectionPin = 7;
int leftDirectionPin = 8;
int leftspeed = 255; //setmaximum speed, goes constant speed
int rightspeed = 250;

void setup() {

  /*TRANSCIEVER CODE*/
  //this allows the rover to listen for the gps coordinates
  radio.begin();
  radio.setRetries(15, 15);

  // This is used for the rover to read what ever is sent on the pipe
  radio.openReadingPipe(1, pipe);
  // Makes it so the rover is listening for the coordinates
  radio.startListening();

  /* Sheild code */
  pinMode(rightMotorPin, OUTPUT);
  pinMode(leftMotorPin, OUTPUT);
  pinMode(rightDirectionPin, OUTPUT);
  pinMode(leftDirectionPin, OUTPUT);

  /* WIRE TRANSMISSION SETUP*/
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);

  /*GPS SETUP*/

  //while (!radio.available()) {
  //radioFunc();
  //delay(20);
  //  7}
  gpsSerial.begin(4800);
}
boolean looper;

void loop(void)
{
  while (gpsSerial.available() > 0 )
  {
    updateGPS();//Get original GPS coordinates
      delay(500);
      if (radio.available()) {  //Receiving the coordinates from the beacon
        bool done = false;
        while (!done) {
          done = radio.read(&gpsCord_Receive, sizeof(gpsCord_Receive));
          for (int i = 0; i < 2; i++) {
            Serial.print("Got Coordinates...");
            Serial.print(gpsCord_Receive[i],6);
            Serial.print("   ");
            done = true;
          }
          Serial.println();
        }
      }
      else {
        Serial.println("Failed...");
      }
      delay(20);
       bLat = gpsCord_Receive[0];
       bLng = gpsCord_Receive[1];
       Serial.println("bLat");
        Serial.println(bLat);
        Serial.println("bLng");
        Serial.println(bLng);

    int functionTime = millis();
/*Using the beacon cooridantes and the Rover Coordinates to find the Beacon */
    while (fabs(bLat - rLat) > 0.00001) {
      if ((bLat - rLat) > 0) { //go north
        Serial.println("Go north");
        Serial.println("Lat");
        Serial.println(rLat);
        Serial.println("Long");
        Serial.println(rLng);
        selfDriving();
      }
      else if ((bLat - rLat) < 0) { //go south
        //180 deg turn
        Serial.println("GO south");
        turnLeft(leftspeed, rightspeed);
        delay(2400);
        selfDriving();
      }
    }
    //  // then drive to find the the right latitude, once found, turn 90 degree, then drive till it finds the
    //  // right longitude
    turnLeft(leftspeed, rightspeed);
    delay(1200);
    while (fabs(bLng - rLng) > 0.00001) {
      if ((bLng - rLng) > 0) { //go east
        Serial.println("Go East");
        selfDriving();
      }
      else if ((bLng - rLng) < 0) { //go west
        //180 deg turn;
        Serial.println("Go West");
        turnLeft(leftspeed, rightspeed);
        delay(2400);
        selfDriving();
      }
    }

    boolean selfDrive = true;

    Serial.println(passback);
    delay(500);
  }
}

/*  SELF DRIVING FUNCTION */

void selfDriving() {
  Serial.println("Self Drive");
  while (true) {
    updateGPS(); //Keep the GPS coordinates Updated

    if (distance < 10 && distance > 0) { //If vehicle within 10 cm of something it stops,
      Serial.println("Collision Stopped "); //Delete THis
      stop();//stop for some time, then check the objects around iit
      delay(1000);

      if (passback.equals("DontTurnRight")) {
        Serial.println("Turn Left "); //Delete THis
        turnLeft(leftspeed, rightspeed);
        delay(1200);
        stop();
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(2000);
        stop();
        delay(1000);
        turnRight(leftspeed, rightspeed);
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(3000);
        turnRight(leftspeed, rightspeed);
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(2000);
        turnLeft(leftspeed, rightspeed);
        delay(1200);
        passback = "Everything is fine";

      }


      else if (passback.equals("DontTurnLeft")) {
        Serial.println("Turn Right "); //Delete THis
        turnRight(leftspeed, rightspeed);
        delay(1200);
        stop();
        delay(1600);
        forward(leftspeed, rightspeed);
        delay(2000);
        stop();
        delay(1000);
        turnLeft(leftspeed, rightspeed);
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(3000);
        turnLeft(leftspeed, rightspeed);
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(2000);
        turnRight(leftspeed, rightspeed);
        delay(1200);

        passback = "Everything is fine";
      }
      else {
        Serial.println("Didnt detect any objects on the Sides");
        turnRight(leftspeed, rightspeed);
        delay(1200);
        stop();
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(2000);
        turnLeft(leftspeed, rightspeed);
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(3000);
        turnLeft(leftspeed, rightspeed);
        delay(1200);
        forward(leftspeed, rightspeed);
        delay(2000);
        turnRight(leftspeed, rightspeed);
        delay(1200);
      }
      //  break; // to break out of the big while lop
    }
    else {
      forward(leftspeed, rightspeed);
      delay(2000);

      break;
    }
    // }
  }
}

/*METHODS FOR THE ROVER TO TRAVEL*/
void stop(void) //Stop
{
  digitalWrite(leftMotorPin, LOW);
  digitalWrite(rightMotorPin, LOW);
}
void forward(char a, char b)
{
  analogWrite (leftMotorPin, a);
  digitalWrite(leftDirectionPin, LOW);
  analogWrite (rightMotorPin, b);
  digitalWrite(rightDirectionPin, LOW);
}

void reverse (char a, char b)
{
  analogWrite (leftMotorPin, a);
  digitalWrite(leftDirectionPin, HIGH);
  analogWrite (rightMotorPin, b);
  digitalWrite(rightDirectionPin, HIGH);
}

void turnLeft(char a, char b)
{
  analogWrite (leftMotorPin, a);
  digitalWrite(leftDirectionPin, LOW);
  analogWrite (rightMotorPin, b);
  digitalWrite(rightDirectionPin, HIGH);

}

void turnRight(char a, char b)
{
  analogWrite (leftMotorPin, a);
  digitalWrite(leftDirectionPin, HIGH);
  analogWrite (rightMotorPin, b);
  digitalWrite(rightDirectionPin, LOW);
}

/*WIRE EVENT*/
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
int old = 0;
int x = 1;
void receiveEvent(int howMany) {

  if (old != x) {

    String str = "";
    while (1 < Wire.available()) { // loop through all but the last
      char c = Wire.read(); // receive byte as a character
      str.concat(c);
    }
    int x = Wire.read();    // receive byte as an integer
    //passback = str + x ;
    if (x < 10 && x > 0 && str.equals("L")) {
      passback = "DontTurnLeft";
      delay(250);
    }
    else if (x < 10 && x > 0 && str.equals("R")) {
      passback = "DontTurnRight";
      delay(250);
    }
    else if (x < 10 && x > 0 && str.equals("F")) {
      passback = "FrontDistance";
      distance = x;
      delay(250);
    }
    else {
    }
  }
}
void updateGPS() {
  int t = millis();
  while (gpsSerial.available() > 0 && millis() < (t + 1000)) {
    if (gps.encode(gpsSerial.read()))
    {
      Serial.println("Calculating");
      rLat = gps.location.lat();
      rLng = gps.location.lng();

      Serial.print("Lat: ");
      Serial.print(rLat, 6);
      Serial.print(", ");
      Serial.print("Lng: ");
      Serial.print(rLng, 6);
      delay(100);

      Serial.println("GPS Degree is");
      Serial.print(gps.course.deg());
      //delay(1300);
    }
  }
}

////////////////////////////////////////This can be a function that can be called once

///////////////////////////////////////////////////////////////////////////////////////////
