#include <ESP8266WiFi.h>
#include <unistd.h>
#include <Wire.h>
#include <FastLED.h>

#define I2C_SDL D1    //klok lijn
#define I2C_SDA D2    // data lijn
#define NUM_LEDS 3    //ledfast
#define DATA_PIN D5   //ledfast
CRGB leds[NUM_LEDS];  //ledfast

#ifndef STASSID
#define STASSID "PiVanOli4"
#define STAPSK "WachtwoordPiVanOli4"
#endif


const char* ssid = STASSID;
const char* password = STAPSK;
const uint16_t port = 8080;
const char* host = "Server Pi van Oli4";
IPAddress local_Ip(192, 168, 10, 99);  //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);

volatile unsigned int Pot = 0;
volatile unsigned int Ldr = 0;
volatile unsigned int anin0 = 0;
volatile unsigned int anin1 = 0;

WiFiClient client;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Wire.begin();          //enable de i2c communicatie
  Serial.begin(115200);  //begin seriele communicatie

  setupLED();            //init led
  setupLDRPotentio();    //init LDR en Potentiometer

  //verbind met het netwerk
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(local_Ip, gateway, subnet);
  WiFi.begin(ssid, password);

  //wacht totdat verbonden met het netwerk
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nVerbonden met netwerk!");
  LED(255, 255, 255); //LED blauw
  updateOudeWaardes(); //eenmalig
  setupLCD(); // initalisatie van de LCD
}

void loop(void) {
  String message = "";
  //verbind met de socket
  if (!client.connected()) {
    Serial.print("connecting to ");
    Serial.print(host);
    Serial.print(':');
    Serial.println(port);
    delay(500);
    client.connect(gateway, 8080);
    if (client.connected()) {
  
      while (!client.available()) {   //wacht oneindig totdat er een bericht ontvangen wordt
        Serial.println("Wachten tot start teken: Identificeer jezelf van de server");
        delay(500);
      }  
      
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer
      }
      
      client.print("Muur"); //laat weten wat voor type device je bent
      while (!client.available()) {
        Serial.println("Wachten op ACK (of de identificatie is gelukt)");
        delay(500);
      }  //wacht oneindig tot dat er een bericht ontvangen wordt
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer
      }
      if (message == "ACK") {
        Serial.println("Server heeft correct ontvangen, identificatie gelukt!");
      }
    }
  }

  while (client.connected()) {
    delay(50);

    if (client.available()) {
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer
      }
      if (message[0] == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        client.write("ACK");
      }
      if (message[0] == '0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
      }
      char LCDchar = message[1]; //index bit 1 voor LCD
      char* binaryString = &message[2]; //vanaf index bit 2 de waarde (0-255) voor de LED-helderheid
      uint8_t helderHeid = (uint8_t)strtol(binaryString, NULL, 2); //omzetten naar uint8_t

      dimLed(helderHeid); //dim de LED strip met de waarde

      if (LCDchar == '0') { //LCD aansturen
        SluitLCD();
      }
      if (LCDchar == '1') { //LCD aansturen
        OpenLCD();
      }
      client.flush();
    }
    //is er verandering en staat er geen bericht om eerst uit te lezen?
    updateNieuweWaardes();
    //Serial.println("Waardes geupdate!");
    if (verandering() && !client.available()) {
      Serial.println("De data is gewijzigd");
      client.print(leesGecombineerd());
      //nu wachten op een ack van de server
      while (!client.available()) {
        Serial.println("Aan het wachten op een ACK bericht van de server");
        delay(25);
      }
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;
      }
      // wacht op de data die geretouneerd wordt
      message = "";
      client.write("ACK");
    }
  }
}
int verandering() {
  if(abs((int)(anin1 - Pot)) > 5) {  //is het verschil groter dan 5?
    Pot = anin1;
    Ldr = anin0;
    return 1;  //geef 1 terug -> er is dus verandering 
  }
  if (abs((int)(anin0 - Ldr)) > 100) {  //is het verschil groter dan 100?
    Pot = anin1; //werk oude waarde bij
    Ldr = anin0; //werk oude waarde bij
    return 1;  //geef 1 terug -> er is dus verandering 
  }
  return 0;  //geef 0 terug -> er is geen verandering
}

void setupLDRPotentio() {
  Wire.beginTransmission(0x36);  //start i2c communicatie omzetter
  Wire.write(byte(0xA2));        //Schrijf naar een register om de MAX11647 in te stellen.
  Wire.write(byte(0x03));        //Schrijf een waarde om de MAX11647 te configureren
  Wire.endTransmission();        //beindig de transmissie
}
void setupLED() {
  pinMode(D5, OUTPUT);           //zet D5 (pint) op output voor LED-strip
}
void LED(int R, int G, int B) {
  leds[0] = (R, G, B);
  FastLED.show();
  leds[1] = (R, G, B);
  FastLED.show();
  leds[2] = (R, G, B);
  FastLED.show();
}
void dimLed(uint16_t x) {
  if (FastLED.getBrightness() > x) {
    while (x != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() - 1);
      FastLED.show();
      delay(3);
    }
  }
  if (FastLED.getBrightness() < x) {
    while (x != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() + 1);
      FastLED.show();
      delay(3);
    }
  }
}
void DimLedInstant(uint8_t x) {
  FastLED.setBrightness(x);
  Serial.print("Zet de LED op: ");
  Serial.println(x);
  FastLED.show();
}
//functie voor het update van de nieuwe waarde (herhaaldelijk elke loop 1x aanroepen)
void updateNieuweWaardes() {
  Wire.requestFrom(0x36, 4);                        // Vraag 4 bytes aan van de MAX11647 (2 analoge ingangen)
  anin0 = (Wire.read() & 0x03) << 8 | Wire.read();  // Eerste 10-bit waarde
  anin1 = (Wire.read() & 0x03) << 8 | Wire.read();  // Tweede 10-bit waarde
}
//functie voor het update van de oude waarden (1x aanroepen bij initalisatie)
void updateOudeWaardes() {
  Wire.requestFrom(0x36, 4);                      // Vraag 4 bytes aan van de MAX11647 (2 analoge ingangen)
  Pot = (Wire.read() & 0x03) << 8 | Wire.read();  // Eerste 10-bit waarde Potentiometer
  Ldr = (Wire.read() & 0x03) << 8 | Wire.read();  // Tweede 10-bit waarde LDR
}
//functie leest in 1 keer alle data uit van ldr+potentiometer en kan dit mooi binair sturen
String leesGecombineerd() {
  Wire.requestFrom(0x36, 4);  // Vraag 4 bytes aan van de MAX11647 (2 analoge ingangen)

  anin0 = (Wire.read() & 0x03) << 8 | Wire.read();  // Eerste 10-bit waarde potentiometer
  anin1 = (Wire.read() & 0x03) << 8 | Wire.read();  // Tweede 10-bit waarde LDR

  // Combineer beide 10-bit waarden in een 20-bit resultaat
  unsigned long combi = ((unsigned long)anin0 << 10) | anin1;

  //binaire output altijd 20 bits lang is
  char binOutput[21];  // 20 bits + 1 voor null-terminator
  for (int i = 19; i >= 0; i--) {
    binOutput[i] = (combi & 1) ? '1' : '0';  // Zet de laatste bit in de string
    combi >>= 1;                             // Schuif bits naar rechts 
  }
  binOutput[20] = '\0';  // Sluit de string af

  return String(binOutput); //return 20 bits om te verzenden
}
void setupLCD() {
  Wire.beginTransmission(0x38);  //start i2c communicatie met apparaat op adres 0x38
  Wire.write(byte(0x03));        //schrijf naar register 0x03 wat verantwoordelijk is voor input of output
  Wire.write(byte(0x0F));        //stel de eerste 4 pinnen als input
  Wire.endTransmission();        //sluit de i2c transmissie af
}
void OpenLCD() {
  Wire.beginTransmission(0x38);
  //normaal schrijf je nu of die in of output is in dit geval moet die naar output
  Wire.write(byte(0x01));    //schrijf naar lcd pannel
  Wire.write(byte(0 << 4));  //schuif een 0 naar het register maak de lcd licht
  Wire.endTransmission();    //beindig schrijven
}
void SluitLCD() {
  Wire.beginTransmission(0x38);
  //normaal schrijf je nu of die in of output is in dit geval moet die naar output
  Wire.write(byte(0x01));    //schrijf naar lcd pannel
  Wire.write(byte(1 << 4));  //schuif een 1 naar het register maak de LCD donker
  Wire.endTransmission();    //beindig schrijven
}
