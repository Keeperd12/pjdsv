#include <unistd.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> //IIRC this could be used to track a hostname instead of a constantly changing IP address.
#include <FastLED.h>  //Version 3.3.0 installed to not run into certain problems like last time.


void UpdateHandler(); //Een functie die alleen naar de server schrijft bij statuswijzingen.
void MovementHandler(); //Afhandelen van bewegingsregistratie.
int RegistrateMovement(); //Registrating movement when someone enters the appartment, for the signal to the security.
void TurnLampOn();
void TurnLampOff();

volatile int prevMovement = 0;
volatile int lampState = 0;
volatile int prevLampState = 0;

#define LED_PIN D5 //De registrated pin of the lamp.
#define LED 1
CRGB led[LED]; //To be able to change the color of the lamp.

#define I2C_SDL    D1 //klok lijn
#define I2C_SDA    D2 // data lijn

#ifndef STASSID
//#define STASSID "PiVanOli4"
#define STASSID "Ramon"
//#define STAPSK "WachtwoordPiVanOli4"
#define STAPSK "12345678"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const uint16_t port = 8080;
//const char* host = "Server Pi van Oli4";
const char *host = "Ramon";
IPAddress local_Ip(192,168,10,5); //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen
IPAddress gateway(192,168,10,1); 
IPAddress subnet(255,255,255,0);

WiFiClient client;

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

  //Server setup
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.config(local_Ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Verbonden met het netwerk!");

  prevMovement = RegistrateMovement();
}


void loop() 
{
  if(!client.connected()){
    Serial.print("connecting to ");
    Serial.print(host);
    Serial.print(':');
    Serial.println(port);
    delay(500);
    client.connect(gateway, 8080);  
  } 
  
  //Serial.println("sending data to server");
  if (client.connected()) {
    //Is er iets uit te lezen
    if (client.available() > 0) {
      String message = ""; // Buffer om het bericht op te slaan
      // Lees alle beschikbare bytes
      while (client.available()) {
        char c = client.read(); // Lees een karakter
          message += c;           // Voeg het toe aan de buffer
      }

    // Toon het volledige bericht
    Serial.print("Ontvangen bericht: ");
    Serial.println(message);
    }
    //is er iets om te versturen?
    client.print("Schemerlamp");
    delay(800);

    //MovementHandler(); //Volgens is MovementHandler volledig in functionaliteit onderdanig aan UpdateHandler nu.
    ToggleLamp(); 
    UpdateHandler();  
  }
  //delay(100); 
}

void UpdateHandler()
{
  // Checking the movement sensor, only a status change should lead to a message to the server.
  if (prevMovement != RegistrateMovement()) 
  {
    if (RegistrateMovement() == 255) 
    { 
      client.print("Movement state changed. Someone is home!");
      Serial.println("Data has been sent, movement has been detected, someone is home.");
    } 
    else 
    {
      client.print("Movement state changed. Nobody's home anymore");
      Serial.println("Data has been sent, no movement is being traced right now.");
    }
    prevMovement = RegistrateMovement(); //De verandering opslaan als de nieuwe waarde om mee te vergelijken.
  }

  // Checking the lamp state, only a status change from the lamp should lead to a message to the server.
  if (lampState != prevLampState) 
  {
    ToggleLamp();
    
    if (lampState == 1) 
    {
      client.print("Lamp is turned on");
      Serial.println("Data has been sent, lamp is turned on.");
    } 
    else if(lampState == 0)
    {
      client.print("Lamp is turned off");
      Serial.println("Data has been sent, lamp is turned off.");
    }
    Serial.println(lampState);
    prevLampState = lampState; //De verandering opslaan als de nieuwe waarde om mee te vergelijken.
  }
}

void MovementHandler() // Afhandelen van bewegingsregistratie
{
  int newMovement = RegistrateMovement();
  //Serial.println(newMovement);

  if(newMovement != prevMovement)
  {
    Serial.println("Movement state changed. Someone is home!");
    prevMovement = newMovement;
  }
  else
    Serial.println("Nothing suspicious.");  
}

void ToggleLamp()
{
   if(lampState == 0)
    {
      TurnLampOn();
      lampState = 1;
    }
    else if( lampState == 1)
    {
      TurnLampOff();
      lampState = 0;
    }  
    delay(500);  
}

int RegistrateMovement()
{
  Wire.beginTransmission(0x38); //PIR sensor is DI0 on address 38, so that's where we decide to transmit to, via I2C.
  Wire.write(byte(0x00)); //Write the data of the very first bit on this address.
  Wire.endTransmission(); //End the data transmission.
  Wire.requestFrom(0x38,1); //We want to request one byte from this address. 
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
