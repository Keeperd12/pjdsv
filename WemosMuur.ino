//#include <iostream>
#include <ESP8266WiFi.h>
#include <unistd.h>
#include <Wire.h>
#include <FastLED.h>

#define I2C_SDL    D1 //klok lijn
#define I2C_SDA    D2 // data lijn
#define NUM_LEDS 3 //ledfast
#define DATA_PIN D5 //ledfast
CRGB leds[NUM_LEDS]; //ledfast

#ifndef STASSID
#define STASSID "PiVanOli4"
#define STAPSK "WachtwoordPiVanOli4"
#endif


const char* ssid = STASSID;
const char* password = STAPSK;
const uint16_t port = 8080;
const char* host = "Server Pi van Oli4";
IPAddress local_Ip(192,168,10,99); //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);


WiFiClient client;

void setup() {
  setupLED();
  digitalWrite(D5,HIGH); //groene lichtje onder aansluiting wemos knipperen
  delay(100); 
  digitalWrite(D5,LOW); //groene lichtje onder aansluiting wemos knipperen
  delay(100);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Wire.begin(); //enable de i2c communicatie
  Serial.begin(115200); //begin seriele communicatie

  
  setupLDRPotentio();

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
  Serial.println("\nVerbonden met netwerk!");
  client.println("Muur");
  LED();
  SluitLCD();
}


void loop(void) {

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
    client.print(leespotmeter());
    
    delay(800);
    client.print(leesLDR());
    //delay(800);
  }
}
void setupLDRPotentio(){
  Wire.beginTransmission(0x36); //start i2c communicatie omzetter
  Wire.write(byte(0xA2)); //Schrijf naar een register om de MAX11647 in te stellen.
  Wire.write(byte(0x03));  //Schrijf een waarde om de MAX11647 te configureren
  Wire.endTransmission();
}
void setupLED(){
  pinMode(D5, OUTPUT);
}
void LED(){
  //stuur ledstrip aan
  leds[0] = CRGB::Green;
  FastLED.show();
  leds[1] = CRGB::Red;
  FastLED.show();
  leds[2] = CRGB::Green;
  FastLED.show();
}
unsigned int leespotmeter(){
  Wire.requestFrom(0x36, 4); //Vraag 4 bytes aan van de MAX11647 (voor twee analoge ingangen).   
  unsigned int anin0 = Wire.read()&0x03;//Lees de eerste twee bytes en combineer ze om de 10-bits waarde van de eerste analoge ingang te krijgen.
  anin0=anin0<<8;//Shift de eerste byte en combineer deze met de tweede byte om de volledige 10-bit waarde van de eerste ingang te krijgen.
  anin0 = anin0|Wire.read();//zie hierboven
  unsigned int anin1 = Wire.read()&0x03;//Herhaal hetzelfde voor de tweede analoge ingang.
  anin1=anin1<<8; //zie hierboven
  anin1 = anin1|Wire.read(); //zie hierboven
  return anin1;
}
unsigned int leesLDR(){
  Wire.requestFrom(0x36, 4); //Vraag 4 bytes aan van de MAX11647 (voor twee analoge ingangen).   
  unsigned int anin0 = Wire.read()&0x03;//Lees de eerste twee bytes en combineer ze om de 10-bits waarde van de eerste analoge ingang te krijgen.
  anin0=anin0<<8;//Shift de eerste byte en combineer deze met de tweede byte om de volledige 10-bit waarde van de eerste ingang te krijgen.
  anin0 = anin0|Wire.read();//zie hierboven
  unsigned int anin1 = Wire.read()&0x03;//Herhaal hetzelfde voor de tweede analoge ingang.
  anin1=anin1<<8; //zie hierboven
  anin1 = anin1|Wire.read(); //zie hierboven
  return anin0;
}
void OpenLCD(){
  Wire.beginTransmission(0x38);
  //normaal schrijf je nu of die in of output is in dit geval moet die naar output
  Wire.write(byte(0x01)); //schrijf naar lcd pannel licht /
  Wire.write(byte(0<<4)); //schuif een 1 naar het register
  Wire.endTransmission(); //beindig schrijven
  Serial.println("LCD aangestuurd OPEN");
}
void SluitLCD(){
  Wire.beginTransmission(0x38);
  //normaal schrijf je nu of die in of output is in dit geval moet die naar output
  Wire.write(byte(0x01)); //schrijf naar lcd pannel
  Wire.write(byte(1<<4)); //schuif een 1 naar het register maak het donker
  Wire.endTransmission(); //beindig schrijven
  Serial.println("LCD aangestuurd DICHT");
}
