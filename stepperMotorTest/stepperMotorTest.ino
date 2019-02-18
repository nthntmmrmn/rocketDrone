/*
 * Simple demo, should work with any driver board
 *
 * Connect STEP, DIR as indicated
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include "BasicStepperDriver.h"

// We'll use SoftwareSerial to communicate with the XBee:
#include <SoftwareSerial.h>
// For server communication
#include "Servo.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 20

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 4

// All the wires needed for full functionality
#define DIR 8
#define STEP 9
//Uncomment line to use enable/disable functionality
//#define SLEEP 13

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

Adafruit_BNO055 bno = Adafruit_BNO055(55);

//Uncomment line to use enable/disable functionality
//BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, SLEEP);

//For Atmega328P's
// XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
SoftwareSerial XBee(2, 3); // RX, TX

//For Atmega2560, ATmega32U4, etc.
// XBee's DOUT (TX) is connected to pin 10 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 11 (Arduino's Software TX)
//SoftwareSerial XBee(10, 11); // RX, TX

// Addresses
// XBEE 1: PANID:3332, DH:0, DL:2FCD, MY:1FCD
// XBEE 2: PANID:3332, DH:0, DL:2FCD, MY:1FCE
// COM XBEE: PANID:3332, DH:0, DL:FFFF, MY:2FCD

String msg;
char buf[80];
int codeLength;

// For Servo
Servo noseServo;
int servoPin = 5;
int noseDegrees = 0;
int noseCloesDegrees = 34;

bool rotate = true;

void setup() {
    // Set up both ports at 9600 baud. This value is most important
    // for the XBee. Make sure the baud rate matches the config
    // setting of your XBee.
    XBee.begin(9600);
    Serial.begin(9600);
    noseServo.write(noseCloesDegrees);
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    pinMode(11, INPUT);
    pinMode(12, INPUT);
   
  
    noseServo.attach(servoPin);
    Serial.println("Setup complete.");
    
    stepper.begin(RPM, MICROSTEPS);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);

    /* Initialise the sensor */
    if(!bno.begin())
    {
      /* There was a problem detecting the BNO055 ... check your connections */
      Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      while(1);
    } 
  
    delay(1000);
    
    bno.setExtCrystalUse(true);
}

void loop() {
    if (Serial.available())
    { // If data comes in from serial monitor, send it out to XBee
      XBee.write(Serial.read());
    }
//    if (XBee.available())
//    { // If data comes in from XBee, send it out to serial monitor
//      codeLength = XBee.readBytesUntil(0x0D, buf, 80);
//      msg = buf;
//      msg = msg.substring(0, codeLength);
//      Serial.println(msg); // Read the msg
//      if(msg == "ROTATE"){
//        XBee.write("Okay, rotating.");
//        rotate = true;
//      }
//      if(msg == "Open Nosecone"){
//        //rotate servo noseDegrees degrees
//        noseServo.write(noseDegrees);
//        XBee.write("NoseCone Opened");
//      }
//      if(msg == "Close Nosecone"){
//        //rotate to 0
//        noseServo.write(noseCloesDegrees);
//        XBee.write("NoseCone Closed");
//      }
//      Serial.write(XBee.read());
//    }
  
    if (digitalRead(11) != LOW) {
      noseServo.write(noseDegrees);
    }
    if (digitalRead(12) != LOW) {
      noseServo.write(noseCloesDegrees);
    }

    if (rotate) {
      sensors_event_t event; 
      bno.getEvent(&event);
      while (event.orientation.x != 0) {
        /* Get a new sensor event */ 
  //      sensors_event_t event; 
  //      bno.getEvent(&event);
        
        /* Display the floating point data */
        Serial.print("X: ");
        Serial.print(event.orientation.x, 4);
        Serial.print("\tY: ");
        Serial.print(event.orientation.y, 4);
        Serial.print("\tZ: ");
        Serial.print(event.orientation.z, 4);
        Serial.println("");
        
        delay(100);
        
        // energize coils - the motor will hold position
        // stepper.enable();
      
        /*
         * Moving motor one full revolution using the degree notation
         */
        stepper.rotate(event.orientation.x);
        delay(3000);
        bno.getEvent(&event);
        /*
         * Moving motor to original position using steps
         */
//          stepper.move(-1*MOTOR_STEPS*MICROSTEPS/2);
    
        // pause and allow the motor to be moved by hand
        // stepper.disable();
    
//          delay(5000);
      }
    }
}
