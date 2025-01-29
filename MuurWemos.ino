#include <ESP8266WiFi.h>
#include <unistd.h>
#include <Wire.h>
#include <FastLED.h>

#define I2C_SDL D1    // Kloklijn voor I2C communicatie
#define I2C_SDA D2    // Datallijn voor I2C communicatie
#define NUM_LEDS 3    // Aantal LEDs in de strip
#define DATA_PIN D5   // Pin voor de LED-strip (FastLED)
CRGB leds[NUM_LEDS];  // Array voor de LEDs in de strip

#ifndef STASSID
#define STASSID "PiVanOli4"  // WiFi SSID
#define STAPSK "WachtwoordPiVanOli4"  // WiFi wachtwoord
#endif

const char* ssid = STASSID;  // WiFi netwerknaam
const char* password = STAPSK;  // WiFi wachtwoord
const uint16_t port = 8080;  // Poort waarop de server luistert
const char* host = "Server Pi van Oli4";  // Serverhost
IPAddress local_Ip(192, 168, 10, 99);  // Statische IP voor dit apparaat
IPAddress gateway(192, 168, 10, 1);  // Gateway IP
IPAddress subnet(255, 255, 255, 0);  // Subnetmasker

// Variabelen voor het lezen van analoge inputs
volatile unsigned int Pot = 0;
volatile unsigned int Ldr = 0;
volatile unsigned int anin0 = 0;
volatile unsigned int anin1 = 0;

WiFiClient client;  // Clientobject voor de netwerkverbinding

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // Initialiseert de LED-strip
  Wire.begin();  // Start I2C-communicatie
  Serial.begin(115200);  // Start seriële communicatie op 115200 baud

  setupLED();  // Initialiseer LED (instellen van kleuren of effecten)
  setupLDRPotentio();  // Initialiseer de LDR en potentiometer

  // Verbind met WiFi-netwerk
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);  // Zet de WiFi-modus op Station (client)
  WiFi.config(local_Ip, gateway, subnet);  // Stel statisch IP in
  WiFi.begin(ssid, password);  // Verbind met WiFi netwerk

  // Wacht totdat de verbinding tot stand is gebracht
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  // Geef aan dat er gewacht wordt
  }
  Serial.println("\nVerbonden met netwerk!");  // Geef aan dat de verbinding succesvol is
  LED(255, 255, 255);  // Zet de LED op blauw
  updateOudeWaardes();  // Update de oude waarden van sensoren
  setupLCD();  // Initialiseer de LCD-scherm
}

void loop(void) {
  String message = "";  // Variabele om berichten op te slaan

  // Verbind met de server via de socket
  if (!client.connected()) {
    Serial.print("connecting to ");
    Serial.print(host);
    Serial.print(':');
    Serial.println(port);
    delay(500);
    client.connect(gateway, 8080);  // Maak verbinding met de server via het IP en poort
    if (client.connected()) {

      // Wacht op de server om zichzelf te identificeren
      while (!client.available()) {
        Serial.println("Wachten tot start teken: Identificeer jezelf van de server");
        delay(500);
      }

      // Lees de ontvangen boodschap van de server
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de server
        message += c;  // Voeg het toe aan de message string
      }

      client.print("Muur");  // Geef aan dat dit apparaat een muur is
      while (!client.available()) {
        Serial.println("Wachten op ACK (of de identificatie is gelukt)");
        delay(500);  // Wacht totdat een ACK ontvangen wordt
      }

      message = "";
      while (client.available()) {
        char c = client.read();  // Lees de server respons
        message += c;  // Voeg het toe aan de message string
      }

      if (message == "ACK") {
        Serial.println("Server heeft correct ontvangen, identificatie gelukt!");  // Bevestiging van succesvolle identificatie
      }
    }
  }

  // Voer acties uit zolang de client verbonden is
  while (client.connected()) {
    delay(50);  // Kleine vertraging om de server niet te overladen

    // Controleer of er een bericht van de server beschikbaar is
    if (client.available()) {
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de server
        message += c;  // Voeg het toe aan de message string
      }

      // Als het eerste karakter '1' is, stuur een "ACK" terug naar de server
      if (message[0] == '1') {
        Serial.println("Deze message is geinitieerd door de server, dus nog een ACK sturen");
        client.write("ACK");
      }

      // Als het eerste karakter '0' is, geef een reactie weer
      if (message[0] == '0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
      }

      // Verkrijg de tweede karakter voor de LCD-besturing
      char LCDchar = message[1];  // Het tweede karakter voor LCD
      char* binaryString = &message[2];  // Start bij het derde karakter voor de LED helderheid (binair)
      uint8_t helderHeid = (uint8_t)strtol(binaryString, NULL, 2);  // Zet de binaire string om naar een uint8_t waarde

      dimLed(helderHeid);  // Dim de LED-strip op basis van de helderheid

      // Besturing voor het aansteken of uitschakelen van het LCD
      if (LCDchar == '0') {
        SluitLCD();  // Sluit LCD
      }
      if (LCDchar == '1') {
        OpenLCD();  // Open LCD
      }
      client.flush();  // Zorg ervoor dat alle berichten worden verzonden
    }

    // Controleer of er veranderingen zijn in de sensorwaarden
    updateNieuweWaardes();  // Update de nieuwe waarden van de sensoren

    // Als er veranderingen zijn en er geen bericht is om te verwerken, stuur de nieuwe waarden naar de server
    if (verandering() && !client.available()) {
      Serial.println("De data is gewijzigd");
      client.print(leesGecombineerd());  // Stuur de gecombineerde waarden naar de server

      // Wacht op een bevestiging van de server
      while (!client.available()) {
        Serial.println("Aan het wachten op een ACK bericht van de server");
        delay(25);
      }

      // Lees het bevestigingsbericht van de server
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de server
        message += c;  // Voeg het toe aan de message string
      }

      // Verstuur een ACK bericht naar de server om ontvangst te bevestigen
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

// Functie om de LED op pin D5 in te stellen als output
void setupLED() {
  pinMode(D5, OUTPUT);  // Zet pin D5 in de output-modus voor de LED-strip
}

// Functie om de LED-strip in een specifieke kleur in te stellen
// De waarden R, G, B vertegenwoordigen respectievelijk de rode, groene en blauwe kleurcomponenten
void LED(int R, int G, int B) {
  leds[0] = (R, G, B);  // Zet de eerste LED naar de opgegeven kleur
  FastLED.show();       // Update de LED-strip om de wijziging zichtbaar te maken
  leds[1] = (R, G, B);  // Zet de tweede LED naar dezelfde kleur
  FastLED.show();       // Update opnieuw om de wijziging zichtbaar te maken
  leds[2] = (R, G, B);  // Zet de derde LED naar dezelfde kleur
  FastLED.show();       // Update de LED-strip voor de laatste wijziging
}

// Functie om de helderheid van de LED-strip langzaam aan te passen
// De helderheid wordt ingesteld op de waarde x
void dimLed(uint16_t x) {
  // Als de huidige helderheid groter is dan de gewenste helderheid, verlaag de helderheid
  if (FastLED.getBrightness() > x) {
    // Terwijl de huidige helderheid niet gelijk is aan de gewenste waarde, verlaag je de helderheid
    while (x != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() - 1);  // Verlaag de helderheid
      FastLED.show();  // Update de LED-strip om de nieuwe helderheid weer te geven
      delay(3);         // Kleine vertraging om het dimmen soepel te maken
    }
  }
  
  // Als de huidige helderheid kleiner is dan de gewenste helderheid, verhoog de helderheid
  if (FastLED.getBrightness() < x) {
    // Terwijl de huidige helderheid niet gelijk is aan de gewenste waarde, verhoog je de helderheid
    while (x != FastLED.getBrightness()) {
      FastLED.setBrightness(FastLED.getBrightness() + 1);  // Verhoog de helderheid
      FastLED.show();  // Update de LED-strip om de nieuwe helderheid weer te geven
      delay(3);         // Kleine vertraging om het dimmen soepel te maken
    }
  }
}

// Functie om de helderheid van de LED-strip onmiddellijk in te stellen op een waarde x
void DimLedInstant(uint8_t x) {
  FastLED.setBrightness(x);  // Zet de helderheid onmiddellijk naar de opgegeven waarde
  Serial.print("Zet de LED op: ");  // Print een bericht naar de seriële monitor
  Serial.println(x);  // Print de nieuwe helderheidswaarde
  FastLED.show();  // Update de LED-strip met de nieuwe helderheid
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
