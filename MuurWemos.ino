//#include <iostream>
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

  setupLED();
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
  LED(255, 255, 255);
  updateOudeWaardes();
  setupLCD();
}



void loop(void) {
  String message = "";
  if (!client.connected()) {
    Serial.print("connecting to ");
    Serial.print(host);
    Serial.print(':');
    Serial.println(port);
    delay(500);
    client.connect(gateway, 8080);
    if (client.connected()) {

      while (!client.available()) {
        Serial.println("Wachten tot start teken: Identificeer jezelf van de server");
        delay(500);
      }  //wacht tot er een bericht te ontvangen is en dan continue
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer
      }
      Serial.println(message);
      Serial.println("Ge identificeert!");
      client.print("Muur");
      while (!client.available()) {
        Serial.println("Wachten tot start teken: Identificeer jezelf van de server");
        delay(500);
      }  //wacht oneindig tot dat er iets te ontvangen valt
      // Lees alle beschikbare bytes
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer
      }
      if (message == "ACK") {
        Serial.println("Server heeft correct ontvangen");
      }
    }
  }

  while (client.connected()) {
    //eerst kijken of er een bericht is:
    delay(50);
    //Serial.println("Debug punt 1");
    
    if (client.available()) {
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer
      }
      //unsigned int temp = atoi(message.c_str());
      Serial.println("Dit is het ontvangen bericht van de server");
      char LCDchar = message[0];
      char *binaryString = &message[1];
      uint8_t helderHeid = (uint8_t)strtol(binaryString, NULL, 2);
      //Serial.println(helderHeid);
      dimLed(helderHeid);

      if(LCDchar == '0'){
        SluitLCD();
      }
      if(LCDchar == '1'){
        OpenLCD();
      }
      delay(100);
      //client.write("ACK");
      //unsigned int temp = (message+1).toInt();
      //Serial.println(temp);
      //DimLedInstant(temp);
      //dimLed(temp);
      //FastLED.show();
    }
    //is er verandering en staat er geen bericht om eerst uit te lezen?
    updateNieuweWaardes();
    //Serial.println("Waardes geupdate!");
    if (verandering() && !client.available()) {
      Serial.println("De data is gewijzigd");
      client.print(leesGecombineerd());
      //nu wachten op een ack van de server
      while(!client.available()){
        Serial.println("Aan het wachten op een ACK bericht van de server");
        delay(25);
      }
      message = "";
      while(client.available()){
        char c = client.read();  // Lees een karakter
        message += c; 
      }
      Serial.println("dit de debug uitlezen");
      Serial.println(message);
      // wacht op de data die geretouneerd wordt
      
      client.write("ACK");
    }
    /* //de client is verbonden dus nu kijken of ontvangen of verzenden
    //zolang de client niets uit te lezen heeft
    //maak de buffer om te zenden leeg
    //delay(500);
    client.print(leesGecombineerd());  //verstuur iets in de buffer HIER ELKE KEER OPNIEUW DATA INLEZEN VAN WEMOS EN VERTSTUREN
    //client.flush();
    while (!client.available()) {
      Serial.println("Wachten op een reactie van de server");  // om te debuggen
      delay(50);        
      client.print(leesGecombineerd());                                       //om te debuggen
    }
    message = "";
    while (client.available()) {
      char c = client.read();  // Lees een karakter
      message += c;            // Voeg het toe aan de buffer
    }
    if (message == "ACK") {
      Serial.println("Server heeft correct ontvangen");
    } else {
      Serial.println(message);
      
      //nsigned int temp = message.toInt();

      //dimLed(temp);
      if(message!="69"&& message != "ACK69"){
        unsigned int temp = atoi(message.c_str());
        DimLedInstant(temp);
      }
      
      //client.print("ACK");
      delay(250);
      
    }
    //delay(500);*/
  }
}
int verandering() {
  // Cast to int to avoid ambiguity with unsigned types
  if (abs((int)(anin1 - Pot)) > 5) {  // If the absolute difference is larger than 10
    Pot = anin1;
    Ldr = anin0;
    return 1;  // There is a change
  }
  if (abs((int)(anin0 - Ldr)) > 100) {  // If the absolute difference is larger than 10
    Pot = anin1;
    Ldr = anin0;
    return 1;  // There is a change
  }
  return 0;  // No change
}



void setupLDRPotentio() {
  Wire.beginTransmission(0x36);  //start i2c communicatie omzetter
  Wire.write(byte(0xA2));        //Schrijf naar een register om de MAX11647 in te stellen.
  Wire.write(byte(0x03));        //Schrijf een waarde om de MAX11647 te configureren
  Wire.endTransmission();
}
void setupLED() {
  pinMode(D5, OUTPUT);
}
void LED(int R, int G, int B) {
  //stuur ledstrip aan
  //leds[0] = CRGB::White;
  leds[0] = (R, G, B);
  FastLED.show();
  //leds[1] = CRGB::White;
  leds[1] = (R, G, B);
  FastLED.show();
  //leds[2] = CRGB::White;
  leds[2] = (R, G, B);
  FastLED.show();
}
void dimLed(uint16_t x) {
  //uint8_t scaled_value = x;
  if (FastLED.getBrightness() > x) {
    while (x != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() - 1);
      FastLED.show();
      delay(3);
      //Serial.println(FastLED.getBrightness()); voor debuggen
    }
  }
  if (FastLED.getBrightness() < x) {
    while (x != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() + 1);
      FastLED.show();
      delay(3);
      //Serial.println(FastLED.getBrightness()); voor debuggen
    }
  }
}
void DimLedInstant(uint16_t x) {
  //uint8_t scaled_value = (x / 4);
  uint8_t scaled_value = ((unsigned long)x * 255) / 1023;
  FastLED.setBrightness(scaled_value);
  Serial.print("Zet de LED op: ");
  Serial.println(scaled_value);
  FastLED.show();
}
void ledAan() {
  if (FastLED.getBrightness() < 255) {
    while (255 != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() + 1);
      delay(100);
      FastLED.show();
      //delay(10);
      //Serial.println(FastLED.getBrightness()); voor debuggen
    }
  }
}
//x = waarde tussen 0-255
void ledUit() {
  if (FastLED.getBrightness() > 0) {
    while (0 != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() - 1);
      FastLED.show();
      delay(10);
      //Serial.println(FastLED.getBrightness()); voor debuggen
    }
  }
}
void updateNieuweWaardes() {
  Wire.requestFrom(0x36, 4);                        // Vraag 4 bytes aan van de MAX11647 (2 analoge ingangen)
  anin0 = (Wire.read() & 0x03) << 8 | Wire.read();  // Eerste 10-bit waarde
  //Serial.println(anin0);
  anin1 = (Wire.read() & 0x03) << 8 | Wire.read();  // Tweede 10-bit waarde
  //Serial.println(anin1);
}
void updateOudeWaardes() {
  Wire.requestFrom(0x36, 4);                        // Vraag 4 bytes aan van de MAX11647 (2 analoge ingangen)
  Pot = (Wire.read() & 0x03) << 8 | Wire.read();  // Eerste 10-bit waarde
  //Serial.println(anin0);
  Ldr = (Wire.read() & 0x03) << 8 | Wire.read();  // Tweede 10-bit waarde
  //Serial.println(anin1);
}
//functie leest in 1 keer alle data uit van ldr+potentiometer en kan dit mooi binair sturen
String leesGecombineerd() {
  Wire.requestFrom(0x36, 4);  // Vraag 4 bytes aan van de MAX11647 (2 analoge ingangen)

  if (Wire.available() < 4) {  // Controleer of er 4 bytes zijn ontvangen
    Serial.println("Error: Not enough data received!");
    return "00000000000000000000";
  }

  // Lees en combineer 10-bit waarden voor de twee analoge ingangen
  anin0 = (Wire.read() & 0x03) << 8 | Wire.read();  // Eerste 10-bit waarde
  anin1 = (Wire.read() & 0x03) << 8 | Wire.read();  // Tweede 10-bit waarde

  // Combineer beide 10-bit waarden in een 20-bit resultaat
  unsigned long combi = ((unsigned long)anin0 << 10) | anin1;

  // Zorg ervoor dat de binaire output altijd 20 bits lang is
  char binOutput[21];  // 20 bits + 1 voor null-terminator
  for (int i = 19; i >= 0; i--) {
    binOutput[i] = (combi & 1) ? '1' : '0';  // Zet de laatste bit in de string
    combi >>= 1;                             // Schuif bits naar rechts
  }
  binOutput[20] = '\0';  // Sluit de string af

  // Print de 20-bits waarde
  //Serial.println(binOutput);

  return String(binOutput);
  //return ((unsigned long)anin0 << 10) | anin1;  // Retourneer de oorspronkelijke waarde
}
void setupLCD(){
  Wire.beginTransmission(0x38); //start i2c communicatie met apparaat op adres 0x38 
  Wire.write(byte(0x03)); //schrijf naar register 0x03 wat verantwoordelijk is voor input of output          
  Wire.write(byte(0x0F)); //stel de eerste 4 pinnen als input      
  Wire.endTransmission(); //sluit de i2c transmissie af
}
void OpenLCD() {
  Wire.beginTransmission(0x38);
  //normaal schrijf je nu of die in of output is in dit geval moet die naar output
  Wire.write(byte(0x01));    //schrijf naar lcd pannel licht /
  Wire.write(byte(0 << 4));  //schuif een 1 naar het register
  Wire.endTransmission();    //beindig schrijven
  Serial.println("LCD aangestuurd OPEN");
}
void SluitLCD() {
  Wire.beginTransmission(0x38);
  //normaal schrijf je nu of die in of output is in dit geval moet die naar output
  Wire.write(byte(0x01));    //schrijf naar lcd pannel
  Wire.write(byte(1 << 4));  //schuif een 1 naar het register maak het donker
  Wire.endTransmission();    //beindig schrijven
  Serial.println("LCD aangestuurd DICHT");
}