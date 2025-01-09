#include <Servo.h>        // Bibliotheek voor het aansturen van een servo
#include <Wire.h>         // Bibliotheek voor I2C-communicatie
#include <ESP8266WiFi.h>  // Bibliotheek voor Wi-Fi

// Pin-definities
#define SERVO_PIN D5
#define I2C_ADDRESS 0x38  // I2C-adres van PCA9554A

// Wi-Fi-configuratie
const char* ssid = "PiVanOli4";
const char* password = "WachtwoordPiVanOli4";
WiFiClient client;
const char* server = "192.168.10.1";  // IP-adres van de server
const uint16_t port = 8080;
IPAddress local_Ip(192,168,10,30); //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen
IPAddress gateway(192,168,10,1);
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

// Variabelen voor LED-besturing
bool ledState = false;
bool blinking = false;
unsigned long previousMillis = 0;
const long interval = 500;

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
    Serial.print("connecting to ");
    Serial.print("Host");
    Serial.print(':');
    Serial.println(port);
    delay(500);
    client.connect(gateway, 8080);  
  }
  
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
    delay(800);
    client.print("Test Deur");
    
  
  }



  
  // Lees de knopstatus via I2C
  Wire.beginTransmission(I2C_ADDRESS); 
  Wire.write(byte(0x00));      
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDRESS, 1);   
  unsigned int input = Wire.read(); 
  
  // Controleer op knoppeninvoer
  if (input != old_input) {  
    if (input & 0x0F) {  // Als een schakelaar is ingedrukt
      Serial.println("Knop ingedrukt");

      if (!switchPressed) {
        switchPressedTime = millis();
        switchPressed = true;
      } else if (doorOpen && !closeDoorRequested) {
        closeDoorRequested = true;
      }
    } else {
      Serial.println("Knop niet ingedrukt");
    }
  }
  old_input = input;

  // LED-knipperlogica
  if (blinking) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01));
      Wire.write(byte(3 << 4)); // waarde schrijven
      Wire.endTransmission();
    }
  }

  // Servo-logica voor deur openen
  unsigned long currentMillis = millis();
  if (switchPressed && currentMillis - switchPressedTime >= openDelay && !doorOpen) {
    myServo.write(195);  // Deur openen
    doorOpen = true;
    doorTimer = currentMillis;
    blinking = true;
    sendStatusToServer("Deur geopend");
  }

  // Deur sluiten
  if (closeDoorRequested && currentMillis - doorTimer >= doorDelay) {
    myServo.write(72);  // Deur sluiten
    doorOpen = false;
    blinking = false;
    closeDoorRequested = false;
    switchPressed = false;
    
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(byte(0x01));
    Wire.write(byte(0x00));  // LEDs uit
    Wire.endTransmission();

    sendStatusToServer("Deur gesloten");
  }

  delay(50);  // Vermijd snelle herhaling
}

// Functie om de status naar de server te sturen
void sendStatusToServer(const char* status) {
//  if (client.connect(server, port)) {
    //client.print("Test Deur");
   
    Serial.print("Status verzonden: ");
    //delay(800);
    Serial.println(status);
//  } else {
   // Serial.println("Verbinding met server mislukt.");
  //}
}
