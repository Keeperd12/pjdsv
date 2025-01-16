#include <Wire.h>
#include <ESP8266WiFi.h>

const char* ssid = "Ramon";      // Wi-Fi SSID
const char* password = "12345678";

const char* host = "192.168.10.1";  // Replace with your server's IP address
const uint16_t port = 8080; 

#define I2C_SDL    D1
#define I2C_SDA    D2
IPAddress local_Ip(192,168,10,20); //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

unsigned int anin0=0;
unsigned int old_anin0=0;
unsigned int old_input=0;
unsigned int old_i=0;
unsigned int i=0;
unsigned int outputs =0;
unsigned int input =0;
String message;
unsigned int gecombineerd=0;

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
  if(!client.connected()){
    VerbindServer();
  }
  
  if (client.connected()) {
    //ontvangenData();
    //LedZoemer();  
    CheckButton();
    ReadAnalog();
  }

  delay(800); 
}

void LedZoemer(){
  if (Serial.available() > 0) { // Check if data is available to read
            i = Serial.parseInt();
            Serial.println(i);
        }
  if (old_i != i){
if (i == 1) {
    outputs = 32;
}
if (i == 2) {
    outputs = 16;
}
if (i == 3) {
    outputs = 48;
}
if (i == 0) {
    outputs = 0;
}
  }
  old_i = i;
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x01));            
  Wire.write(byte(outputs));            
  Wire.endTransmission(); 
}

void CheckButton(){
  Wire.beginTransmission(0x38); 
  Wire.write(byte(0x00));      
  Wire.endTransmission();
  Wire.requestFrom(0x38, 1);   
  input = Wire.read(); 
  Serial.println(input);
  //Serial.println(input);
  if(input != old_input){  
  VerstuurData();
  }
  old_input = input;
}

void ontvangenData(){
    if (client.available() > 0) {
      message = ""; // Buffer om het bericht op te slaan
      // Lees alle beschikbare bytes
      while (client.available()) {
        char c = client.read(); // Lees een karakter
        message += c;           // Voeg het toe aan de buffer
      }

    // Toon het volledige bericht
      Serial.print("Ontvangen bericht: ");
      Serial.println(message);
    }
    //client.print("test zuil");
}

void ReadAnalog(){
  //Read analog 10bit inputs 0&1
  if (old_anin0 == 0){
    Wire.requestFrom(0x36, 4);   
    anin0 = Wire.read()&0x03;  
    anin0=anin0<<8;
    anin0 = anin0|Wire.read(); 
    old_anin0 = anin0;
    return;
  }
  Wire.requestFrom(0x36, 4);   
  anin0 = Wire.read()&0x03;  
  anin0=anin0<<8;
  anin0 = anin0|Wire.read(); 
  //Serial.println(anin0);      
  if ((anin0 > old_anin0 + 5) || (anin0 < old_anin0 - 5)){
    VerstuurData();
  }
  Serial.println(anin0);
}

void VerbindServer(){
    Serial.print("connecting to ");
    Serial.print(host);
    Serial.print(':');
    Serial.println(port);
     
    if(!client.connect(host, port)) {
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

void VerstuurData(){
  Serial.println("zit in functie VerstuurData.");
  gecombineerd = (anin0 << 1) | input;
  Serial.print(gecombineerd);
  client.print(gecombineerd);
  while (!client.available()) {
       Serial.println("zit in functie VerstuurData.");
        delay(500);
      }
  message = ""; // Buffer om het bericht op te slaan
    // Lees alle beschikbare bytes
  while (client.available()) {
        char c = client.read(); // Lees een karakter
        message += c;           // Voeg het toe aan de buffer
    }
      if (message == "ACK") {
        Serial.println("Server heeft correct ontvangen");
}
  
  old_anin0 = anin0;
}
