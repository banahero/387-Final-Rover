//This is the code that collects data from the back, right and left sensors and sends it to the other
//Adruino VIA pins A5 Connected to A5 and A4 connected to A4

#include <Wire.h>
#include <NewPing.h>

int triggerPin1 = 2; //triggering on sensor1
int echoPin1 = 3; 
int triggerPin2 = 4; //triggering on sensor2
int echoPin2 = 5;
int triggerPin3 = 6; //triggering on sensor2
int echoPin3 = 7;


void setup() {
    pinMode(triggerPin1, OUTPUT); //defining pins Left
  pinMode(echoPin1, INPUT);
  

  pinMode(triggerPin2, OUTPUT); //defining pins
  pinMode(echoPin2, INPUT);  //right

    pinMode(triggerPin3, OUTPUT); //defining pins
  pinMode(echoPin3, INPUT);  //right

   Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
}



void loop() {
  //Right Distance
   int durationR;//,distance;    //Adding duration and distance
 int distanceR;
  digitalWrite(triggerPin2, HIGH); //triggering the wave(like blinking an LED)
  delay(10);
  digitalWrite(triggerPin2, LOW);
  
  durationR = pulseIn(echoPin2, HIGH); //a special function for listening and waiting for the wave
  distanceR = (durationR/2) / 29.1; //transforming the number to cm(if you want inches, you have to change the 29.1 with a suitable number
  Serial.println("Right distance is ");
   Serial.println(distanceR);

  //Left Distance
  int durationL;//,distance;    //Adding duration and distance
  int distanceL;
  digitalWrite(triggerPin1, HIGH); //triggering the wave(like blinking an LED)
  delay(10);
  digitalWrite(triggerPin1, LOW);

  
  durationL = pulseIn(echoPin1, HIGH); //a special function for listening and waiting for the wave
  distanceL = (durationL/2) / 29.1; //transforming the number to cm(if you want inches, you have to change the 29.1 with a suitable number

    Serial.println("LEft distance is ");
  Serial.println(distanceL);
  
  //Front Distance
  int durationF;//,distance;    //Adding duration and distance
  int distanceF;
  digitalWrite(triggerPin3, HIGH); //triggering the wave(like blinking an LED)
  delay(10);
  digitalWrite(triggerPin3, LOW);
  
  durationF = pulseIn(echoPin3, HIGH); //a special function for listening and waiting for the wave
  distanceF = (durationF/2) / 29.1; //transforming the number to cm(if you want inches, you have to change the 29.1 with a suitable number
  

  Wire.beginTransmission(8); // transmit to device #8
  Wire.write("R");        // sends five bytes
  Wire.write(distanceR);             // sends one byte
  Wire.endTransmission();    // stop transmitting
  delay(100);//maybe delete this

  Wire.beginTransmission(8);
  Wire.write("L");
  Wire.write(distanceL);
  Wire.endTransmission();
  delay(100);//maybe delete 

  Wire.beginTransmission(8); // transmit to device #8
  Wire.write("F");        // sends five bytes
  Wire.write(distanceF);             // sends one byte
  Wire.endTransmission();    // stop transmitting


  delay(500);
}
