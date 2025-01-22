#include <unistd.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> //IIRC this could be used to track a hostname instead of a constantly changing IP address.
#include <FastLED.h>  //Version 3.3.0 installed to not run into certain problems like last time.

int UpdateHandler(); // Een functie die alleen naar de server schrijft bij statuswijzigingen.
int RegistrateMovement(); // Registrating movement when someone enters the apartment, for the signal to the security.
void ToggleLamp(); // Aan- of uitzetten van de lamp op basis van triggers.
void TurnLampOn(uint8_t helderheid);
void TurnLampOff();

volatile int prevMovement = 0;
volatile int currentMovement = 0;
volatile int lampState = 0;
volatile int prevLampState = 0;

#define LED_PIN D5 // De geregistreerde pin van de lamp.
#define LED 1
CRGB led[LED]; // To be able to change the color of the lamp.

#define I2C_SDL    D1 // Kloklijn
#define I2C_SDA    D2 // Datlijn

#ifndef STASSID
#define STASSID "PiVanOli4"
#define STAPSK "WachtwoordPiVanOli4"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const uint16_t port = 8080;
const char *host = "Server Pi van Oli4";
IPAddress local_Ip(192, 168, 10, 5);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);


WiFiClient client;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  // Setup for Wire bitshifting: (From WIB-Test)
  Wire.beginTransmission(0x38); // Starting I2C communication to this specific device.
  Wire.write(byte(0x03));  // Initializing the first 4 bits, 0-3, to input.
  Wire.write(byte(0x0F));  // Initializing the last 4 bits, 4-7, to output.
  Wire.endTransmission(); // Stop condition, ending the communication.

  // Setup settings for the lamp:
  pinMode(D5, OUTPUT); // Anders kan je de lamp niet aansturen.
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(led, LED); // Setup for controlling the lamp.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); // Maximum power draw.
  FastLED.clear(); // Clear the remaining buffer.

  // Server setup
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(local_Ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Verbonden met het netwerk!");

  prevMovement = RegistrateMovement();
  led[0] = CRGB::Yellow; // Turn the lamp yellow when it's turned on.
  FastLED.show(); // Update the lamp with the current data.  
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
      client.print("Schemerlamp");
      while (!client.available()) {
        Serial.println("Wachten tot start teken: Identificeer jezelf van de server");
        delay(50);
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

      Serial.println("Het bericht is geinitiseerd door de server dus nog een ack sturen");
      client.print("ACK");

      char *binaryString = &message[0];
      uint8_t helderheid = (uint8_t)strtol(binaryString, NULL, 2);
      /*
        //Serial.println(strlen(message));
        if (message[0] == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        client.write("ACK");
        }
        if (message[0] == '0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
        }
        //unsigned int temp = atoi(message.c_str());
        Serial.println("Dit is het ontvangen bericht van de server");*/
      Serial.println(helderheid);
      TurnLampOn(helderheid);
      //client.flush();
    }

    //is er verandering en staat er geen bericht om eerst uit te lezen?
    currentMovement = RegistrateMovement();
    //Serial.println("Waardes geupdate!");
    if (UpdateHandler() && !client.available()) {
      Serial.println("De data is gewijzigd");
      //client.print(leesGecombineerd());
      client.print(currentMovement);
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
      //client.write("ACK");
      client.flush();
    }
  }
}
int UpdateHandler()
{
  // Checking the movement sensor, only a status change should lead to a message to the server.
  if (currentMovement != prevMovement)
  {
    prevMovement = currentMovement; //De verandering opslaan als de nieuwe waarde om mee te verg
    return 1;
  }
  return 0;
}

/*void ToggleLamp()
{
  if (lampState == 0)
  {
    TurnLampOn();
    lampState = 1;
  }
  else if (lampState == 1)
  {
    TurnLampOff();
    lampState = 0;
  }

  delay(500);
} */

int RegistrateMovement()
{
  Wire.beginTransmission(0x38); // PIR sensor is DI0 on address 38.
  Wire.write(byte(0x00)); // Write the data of the very first bit on this address.
  Wire.endTransmission(); // End the data transmission.
  Wire.requestFrom(0x38, 1); // Request one byte from this address.
  int readValue = Wire.read(); // Read the current value from the movement sensor.
  return readValue;
}

void TurnLampOn(uint8_t helderheid)
{
  FastLED.setBrightness(helderheid);
  FastLED.show();
}

void TurnLampOff()
{
  FastLED.setBrightness(0); // Brightness of 0 is equivalent to the lamp being off.
  FastLED.show(); //Portray the lamp being turned off.
}

void MovementHandler() // Afhandelen van bewegingsregistratie
{
  int newMovement = RegistrateMovement();
  //Serial.println(newMovement);

  if (newMovement != prevMovement)
  {
    Serial.println("Movement state changed. Someone is home!");
    prevMovement = newMovement;
  }
  else
    Serial.println("Nothing suspicious.");
}
