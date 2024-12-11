#include <Servo.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> //IIRC this could be used to track a hostname instead of a constantly changing IP address.
#include <FastLED.h>  //Version 3.3.0 installed to not run into certain problems like last time.

int RegistrateMovement(); //Registrating movement when someone enters the appartment, for the signal to the security.
void TurnLampOn();
void TurnLampOff();

#define LED_PIN D5 //De registrated pin of the lamp.
#define LED 1
CRGB led[LED]; //To be able to change the color of the lamp.

void setup()
{
  Wire.begin();
  Serial.begin(115200); 

  //Setup for Wire bitshifting: (From WIB-Test)
  Wire.beginTransmission(0x38); //Starting I2C communication to this specific device. DI0 is on this address, so we want 38 as the I2C address instead of 36.
  Wire.write(byte(0x03));  //Initialising the first 4 bits, 0-3, to input.        
  Wire.write(byte(0x0F));  //Initialising the last 4 bits, 4-7, to output.
  Wire.endTransmission(); //Stop condition, ending the communication.

  //Setup settings for the lamp:
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(led, LED); //Setup for controlling the lamp.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); //Maximum power drawage.
  FastLED.clear(); //Clear the remaining buffer.
  FastLED.show(); //Update the lamp with the current data.
}

void loop() 
{
  RegistrateMovement();
  TurnLampOn();
}

int RegistrateMovement()
{
  Wire.beginTransmission(0x38); //PIR sensor is DI0 on address 38, so that's where we decide to transmit to.
  Wire.write(byte(0x00)); //Write the data of the very first bit on this address.
  Wire.endTransmission(); //End the data transmission.
  Wire.requestFrom(0x38,1); //We want to request the byte from this address now, and do this once.
  int readValue = Wire.read(); //Read the current value that is being registered by the movement sensor. 
  return readValue;
}

void TurnLampOn()
{
  led[0] = CRGB::Yellow; //Making the lamp turn yellow when it's turned on.
  FastLED.setBrightness(64);
  FastLED.show();
}

void TurnLampOff()
{
  FastLED.setBrightness(0); //Brightness of 0 is the same as the lamp being turned off.
  FastLED.show(); //Portray the lamp being turned off.
}
