/*****************************************************************
XBee_Serial_Passthrough.ino

Set up a software serial port to pass data between an XBee Shield
and the serial monitor.

Hardware Hookup:
  The XBee Shield makes all of the connections you'll need
  between Arduino and XBee. If you have the shield make
  sure the SWITCH IS IN THE "DLINE" POSITION. That will connect
  the XBee's DOUT and DIN pins to Arduino pins 2 and 3.

*****************************************************************/
// We'll use SoftwareSerial to communicate with the XBee:
#include <SoftwareSerial.h>
// For server communication
#include "Servo.h"

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
// The DL address is the broadcast to the PAN
// The DL address for the XBEEs are just to talk back to the controller XBEE

String msg;
char buf[80];
int codeLength;

// For Servo
Servo noseServo;
int servoPin = 5;
int noseDegrees = 109; 

void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  XBee.begin(9600);
  Serial.begin(9600);

  noseServo.attach(servoPin);
  Serial.println("Setup complete.");
}

void loop()
{
  if (Serial.available())
  { // If data comes in from serial monitor, send it out to XBee
    XBee.write(Serial.read());
  }
  XBee.write("Tx2");
  delay(100);
  /*
  if (XBee.available())
  { // If data comes in from XBee, send it out to serial monitor
    codeLength = XBee.readBytesUntil(0x0D, buf, 80);
    msg = buf;
    msg = msg.substring(0, codeLength);
    Serial.println(msg); // Read the msg
    if(msg == "PING2"){
      XBee.write("Pong2\n");
    }
    if(msg == "Open Nosecone"){
      //rotate servo noseDegrees degrees
      noseServo.write(noseDegrees);
    }
    if(msg == "Close Nosecone"){
      //rotate to 0
      noseServo.write(0);
    }
    Serial.write(XBee.read());
  }
  */
}

