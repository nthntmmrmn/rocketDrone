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
#include <utility/imumaths.h>
#include "BasicStepperDriver.h"

// We'll use SoftwareSerial to communicate with the XBee:
#include <SoftwareSerial.h>
// For server communication

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 600

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 1

// All the wires needed for full functionality
#define DIR 4
#define STEP 3
#define SLEEP 8

//motor closed loop multiplier
int motorMultiplier = 27*2;

// PIN 2 to turn on solenoid valve
int solenoidPin = 2;



// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

Adafruit_BNO055 bno = Adafruit_BNO055();


//For Atmega328P's
// XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
SoftwareSerial XBee(9, 10); // RX, TX

// Addresses
// XBEE 1: PANID:3332, DH:0, DL:2FCD, MY:1FCD
// XBEE 2: PANID:3332, DH:0, DL:2FCD, MY:1FCE
// COM XBEE: PANID:3332, DH:0, DL:FFFF, MY:2FCD

String msg;
char buf[80];
int codeLength;

bool rotate1 = false;
sensors_event_t event; 
void setup() {
    // Set up both ports at 9600 baud. This value is most important
    // for the XBee. Make sure the baud rate matches the config
    // setting of your XBee.
    XBee.begin(9600);
    delay(10);
    Serial.begin(9600);
    delay(10);
    
     pinMode(solenoidPin, OUTPUT); 
     pinMode(SLEEP, OUTPUT); 
     digitalWrite(SLEEP, LOW);
     
    //initializes stepper motor
    stepper.begin(RPM, MICROSTEPS);
   

    /* Initialise the sensor */
    if(!bno.begin())
    {
      /* There was a problem detecting the BNO055 ... check your connections */
      Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      //while(1);
    } 
  
    
    bno.setExtCrystalUse(true);

//    //calibrates rpobs dont need this though
//    uint8_t system1, gyro, accel, mag;
//    system1 = gyro = accel = mag = 0;
//    while(gyro == 0){
//      delay(1);
//      bno.getCalibration(&system1, &gyro, &accel, &mag);
//      Serial.print(gyro, DEC);
//    }
    
    Serial.println("Setup complete.");
    delay(1000);
}

void loop() {
    if (XBee.available())
    { 
      // gets message from Xbee and put it into msg
      codeLength = XBee.readBytesUntil(0x0D, buf, 80);
      msg = buf;
      msg = msg.substring(0, codeLength);
      Serial.println(msg); // Read the msg
      if(msg == "PING2") {
        XBee.write("PONG2\n");
      }
      //start rotate1 proceedure
      if(msg == "ROTATE"){
        XBee.write("Okay, rotating. Mass matrix diagonized: ready for jump!\n");
        rotate1 = true;
        Serial.write("Okay, rotating.");
      }
      if(msg == "Stop rotate"){
        XBee.write("Okay, stopped rotating.\n");
        rotate1 = false;
        Serial.write("Okay, stopped rotating.");
      }
      if(msg == "LetTheDogsOut") {
        digitalWrite(solenoidPin, HIGH);  
        XBee.write("Solenoid open: Dogs are out\n");
        Serial.write("Solenoid open.");
      }
      if(msg == "PutDogsBackIn") {
        digitalWrite(solenoidPin, LOW);  
        XBee.write("Solenoid closed: Dogs are back in\n");
        Serial.write("Solenoid closed.");
      }
    }
    if (rotate1) {
      //but new data into event
      digitalWrite(SLEEP, HIGH);
      bno.getEvent(&event);
      Serial.print("\t every Y: ");
        Serial.print(event.orientation.y, 4);
      //checks to see if platform is flat enough
      if(event.orientation.y >= 5 || event.orientation.y <= -5) {
        
        /* Display the floating point data */
        Serial.print("X: ");
        Serial.print(event.orientation.x, 4);
        Serial.print("\tY: ");
        Serial.print(event.orientation.y, 4);
        Serial.print("\tZ: ");
        Serial.print(event.orientation.z, 4);
        Serial.println("");
        
        
      
        // rotates plate, might have to switch negative sign
        if(event.orientation.y > 180){
          stepper.rotate(event.orientation.y*motorMultiplier);
          
          
        }else{
          //stepper.rotate(360*27);
          stepper.rotate(-1* event.orientation.y*motorMultiplier);
          
        }
      }else{
        rotate1 = false;
        XBee.write("Done Rotating!! Hamster is in the wheel.\n");
        Serial.print("Done Rotating!!");
        digitalWrite(SLEEP, LOW);
      }
    }
}
