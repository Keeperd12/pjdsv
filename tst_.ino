#include <Servo.h>        // Bibliotheek voor het aansturen van een servo
#include <Wire.h>         // Bibliotheek voor I2C-communicatie
#include <ESP8266WiFi.h>  // Bibliotheek voor Wi-Fi

// Pin-definities
#define SERVO_PIN D5
#define I2C_ADDRESS 0x38  // I2C-adres van PCA9554A

// Wi-Fi-configuratie
const char* ssid = "Ramon's";
const char* password = "ramondirksen";
WiFiClient client;
const char* server = "192.168.222.41";  // IP-adres van de server
const uint16_t port = 8080;
IPAddress local_Ip(192,168,10,20); //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen
IPAddress gateway(192,168,222,41);
IPAddress subnet(255,255,255,0);

// Servo-object
Servo myServo;

// Variabelen voor deurbesturing
bool doorOpen = false;
unsigned long doorTimer = 0;
const long doorDelay = 8000;
unsigned long openDelay = 3000;
unsigned long switchPressedTime = 0;
bool switchPressed = false;
bool closeDoorRequested = false;
String message;
// Variabelen voor LED-besturing
bool ledState = false;
bool blinking = false;
unsigned long previousMillis = 0;
const long interval = 500;
unsigned int DeurStatus = 0;

// Variabelen voor de schakelaar
unsigned int old_input = 0;

void setup() {
  Serial.begin(115200);
  


  
  // Start de Wi-Fi-verbinding
  WiFi.mode(WIFI_STA);
  WiFi.config(local_Ip, gateway, subnet);
  Serial.print("Verbinding maken met Wi-Fi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nVerbonden met Wi-Fi!");
  Serial.print("IP-adres: ");
  Serial.println(WiFi.localIP());

  // Initialiseer de servo
  myServo.attach(SERVO_PIN);
  myServo.write(72);  // Deur gesloten

  // Initialiseer I2C
  Wire.begin();
  
  // Configureer PCA9554A als output
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(byte(0x03)); // Register DDR
  Wire.write(byte(0x0F)); // Stel alle pins in als output
  Wire.endTransmission();
  
  // Zet LEDs uit bij opstarten
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(byte(0x01));  // Output register
  Wire.write(byte(0x00));  // Zet LEDs uit
  Wire.endTransmission();
}

void loop() {
  
   if(!client.connected()){

 VerbindServer();     

   }
   if(client.connected()){
   LeesSchakelaars();
   VerstuurData();
   DataCheck();
   }

  delay(50);  // Vermijd snelle herhaling
}

void LeesSchakelaars() {
  // Lees de knopstatus via I2C
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDRESS, 1);
  current_input = Wire.read();
}



void VerstuurData() {
  if (current_input != old_input) {  // Alleen versturen bij verandering
    Serial.println("Knopstatus gewijzigd, versturen naar server...");

    // Combineer de status van de schakelaars
    client.print(current_input & 0x0F);
    Serial.print("Nieuwe knopstatus verstuurd: ");
    Serial.println(current_input & 0x0F);

    // Wacht op een ACK van de server
    while (!client.available()) {
      Serial.println("Wacht op ACK van de server...");
      delay(500);
    }

  // Lees het antwoord van de server
  message = "";
  while (client.available()) {
    char c = client.read(); // Lees een karakter
    message += c;           // Voeg het toe aan de buffer
  }

  // Controleer of de server ACK heeft gestuurd
  if (message == "ACK") {
    Serial.println("Server heeft correct ontvangen");
  } else {
    Serial.println("Fout bij ontvangstbevestiging van server: " + message);
  }
}

void VerbindServer(){ 
    Serial.print("connecting to ");
    Serial.print(server);
    Serial.print(':');
    Serial.println(port);
     
    if(!client.connect(server, port)) {
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

      client.print("Deur");
     
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

void DataCheck(){
  //Serial.println("Welkom in datacheck");
  Serial.println(client.available());
    if (client.available()) {
      Serial.println("in datacheck Client available");
       // Buffer om het bericht op te slaan
       message = "";
      // Lees alle beschikbare bytes
      while (client.available()) {
        char c = client.read(); // Lees een karakter
        message += c;           // Voeg het toe aan de buffer
      }

    // Toon het volledige bericht
      Serial.print("Ontvangen bericht: ");
      Serial.println(message);
      client.print("ACK");
      
    
    if (message == "1"){
          Serial.println("message = 1");
         
      Deur();
      }
    if (message=="0"){
      myServo.write(72);
    }
}
}

void Deur(){
   Serial.println("Deur openen...");
  myServo.write(195);
  for (int a = 0; a = 8000; a++){
    DataCheck();
    
    delay(1);
  }
  Serial.println("Deur sluiten...");
  myServo.write(72);
}
