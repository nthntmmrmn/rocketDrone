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

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 20

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 4

// All the wires needed for full functionality
#define DIR 4
#define STEP 3


// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

Adafruit_BNO055 bno = Adafruit_BNO055(55);


//For Atmega328P's
// XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
SoftwareSerial XBee(2, 3); // RX, TX

// Addresses
// XBEE 1: PANID:3332, DH:0, DL:2FCD, MY:1FCD
// XBEE 2: PANID:3332, DH:0, DL:2FCD, MY:1FCE
// COM XBEE: PANID:3332, DH:0, DL:FFFF, MY:2FCD

String msg;
char buf[80];
int codeLength;

bool rotate = false;
sensors_event_t event; 
void setup() {
    // Set up both ports at 9600 baud. This value is most important
    // for the XBee. Make sure the baud rate matches the config
    // setting of your XBee.
    //XBee.begin(9600);
    delay(1000);
    Serial.begin(9600);
    delay(1000);
    //Serial.println("Setup complete.");
    //initializes stepper motor
    //stepper.begin(RPM, MICROSTEPS);
   

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
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    if (XBee.available())
    { 
      // gets message from Xbee and put it into msg
      codeLength = XBee.readBytesUntil(0x0D, buf, 80);
      msg = buf;
      msg = msg.substring(0, codeLength);
      Serial.println(msg); // Read the msg

      //start rotate proceedure
      if(msg == "ROTATE"){
        XBee.write("Okay, rotating.");
        rotate = true;
        Serial.write("Okay, rotating.");
      } 
    }

    if (rotate) {
      //but new data into event
      
      bno.getEvent(&event);

      //checks to see if platform is flat enough
      if(event.orientation.x >= 5 && event.orientation.x <= 355) {
        
        /* Display the floating point data */
        Serial.print("X: ");
        Serial.print(event.orientation.x, 4);
        Serial.print("\tY: ");
        Serial.print(event.orientation.y, 4);
        Serial.print("\tZ: ");
        Serial.print(event.orientation.z, 4);
        Serial.println("");
        
        delay(100);
        
      
        // rotates plate, might have to switch negative sign
        if(event.orientation.x < 180){
          stepper.rotate(event.orientation.x/8);
          delay(100);
        }else{
          stepper.rotate(-1* event.orientation.x/8);
          delay(100);
        }
      }else{
        rotate = false;
        XBee.write("Done Rotating!!");
        Serial.print("Done Rotating!!");
      }
    }
}
