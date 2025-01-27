#include <Wire.h>
#include <ESP8266WiFi.h>

const char* ssid = "Ramon's";
const char* password = "ramondirksen";

const char* host = "server";  // Replace with your server's IP address
const uint16_t port = 8080;

#define I2C_SDL    D1
#define I2C_SDA    D2
IPAddress local_Ip(192, 168, 10, 50); //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen
IPAddress gateway(192, 168, 122, 41);
IPAddress subnet(255, 255, 255, 0);

unsigned int anin0 = 0;
unsigned int old_anin0 = 0;
unsigned int false_input = 0;
unsigned int old_i = 0;
String i;
//unsigned int outputs = 0;
unsigned int input = 0;
String message;
unsigned int gecombineerd = 0;

WiFiClient client;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay (1000);

  Wire.beginTransmission(0x38);
  Wire.write(byte(0x03));
  Wire.write(byte(0x01));
  Wire.endTransmission();

  Wire.beginTransmission(0x36);
  Wire.write(byte(0xA2));
  Wire.write(byte(0x03));
  Wire.endTransmission();

  Wire.beginTransmission(0x38);
  Wire.write(byte(0x01));
  Wire.write(byte(0));
  Wire.endTransmission();

  Serial.println("Connecting to Wi-Fi...");


  WiFi.mode(WIFI_STA);
  WiFi.config(local_Ip, gateway, subnet);
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!client.connected()) {
    VerbindServer();
  }

  while (client.connected()) {
    delay(800);
    //Serial.println("in datacheck Client available");
    // Buffer om het bericht op te slaan
    if (client.available()) {
      message = "";
      // Lees alle beschikbare bytes
      while (client.available()) {
        char c = client.read(); // Lees een karakter
        message += c;           // Voeg het toe aan de buffer
      }
      if (message.charAt(0) == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        client.write("ACK");
      }
      if (message.charAt(0) == '0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
        client.write("ACK");
      }

      // Toon het volledige bericht
      Serial.print("Ontvangen bericht: ");
      Serial.println(message);
      //client.write("ACK");

      LedZoemer();
      //client.flush();
    }
    if (!client.available() && CheckButtonAndAnalog()) {
      gecombineerd = (anin0 << 1) | input;
      //Serial.print(gecombineerd);
      client.print(gecombineerd);
      while (!client.available()) {
        Serial.println("zit in functie VerstuurData (loop).");
        delay(25);
      }
      message = ""; // Buffer om het bericht op te slaan
      // Lees alle beschikbare bytes
      while (client.available()) {
        char c = client.read(); // Lees een karakter
        message += c;           // Voeg het toe aan de buffer
      }
      Serial.print("ontvangen bericht na verturen data: ");
      Serial.println(message);
      if (message == "ACK") {
        Serial.println("Server heeft correct ontvangen");
        }
      message = "";
      //old_anin0 = anin0;
    }
    //CheckData();


  }
}
//delay(800);


void LedZoemer() {
  int zoemer;
  int led;
  int outputs;
  
  if (message.charAt(1) == '1') {
    zoemer = 1;
  } else {
    zoemer = 0;
  }

  if (message.charAt(2) == '1') {
    led = 1;
  } else {
    led = 0;
  }
  if (message.charAt(3) == '1') {
    blinkZoemer();
  }
  Serial.print("zoemer: ");
  Serial.println(zoemer);
  Serial.print("led: ");
  Serial.println(led);
  if (zoemer == 0 && led == 1) {
    outputs = 32;
  }
  if (zoemer == 1 && led == 0) {
    outputs = 16;
  }
  if (zoemer == 1 && led == 1) {
    outputs = 48;
  }
  if (zoemer == 0 && led == 0) {
    outputs = 0;
  }
  Wire.beginTransmission(0x38);
  Wire.write(byte(0x01));
  Wire.write(byte(outputs));
  Wire.endTransmission();
}

void blinkZoemer(){
  
}

void VerbindServer() {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  if (!client.connect(gateway, port)) {
    Serial.println("Connection failed!");
    return;
  }

  Serial.println("Connected to server");
  //Serial.print('1');
  while (!client.available()) {
    delay(500);
  }

  //Serial.print('2');
  message = ""; // Buffer om het bericht op te slaan
  // Lees alle beschikbare bytes
  while (client.available()) {
    char c = client.read(); // Lees een karakter
    message += c;           // Voeg het toe aan de buffer
  }
  Serial.println(message);

  client.print("Zuil");

  while (!client.available()) {
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

int CheckButtonAndAnalog() {
  //Read analog 10bit inputs 0&1
  if (old_anin0 == 0) {
    Wire.requestFrom(0x36, 4);
    anin0 = Wire.read() & 0x03;
    anin0 = anin0 << 8;
    anin0 = anin0 | Wire.read();
    old_anin0 = anin0 - 30;
    return 0;
  }
  Wire.requestFrom(0x36, 4);
  anin0 = Wire.read() & 0x03;
  anin0 = anin0 << 8;
  anin0 = anin0 | Wire.read();
  anin0 = anin0 - 30;
  Serial.print("data anin0: ");
      Serial.println(anin0);
  if ((anin0 > old_anin0 + 20) || (anin0 < old_anin0 - 20)) {
    old_anin0 = anin0;
    return 1;
  }
  //Serial.println(anin0);

  Wire.beginTransmission(0x38);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1);
  int current_input = Wire.read();
  input = (current_input & (1 << 0)) != 0;      
  Serial.print("data input: ");
      Serial.println(input);
  if (input != false_input) {
    return 1;
  }
  return 0;
}
