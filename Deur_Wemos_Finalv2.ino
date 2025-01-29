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

IPAddress local_Ip(192, 168, 10, 20); //wanneer de pi dhcp zou hebben zou dit niet nodig zijn en is de code helemaal identiek op beide wemossen

IPAddress gateway(192, 168, 10, 1);

IPAddress subnet(255, 255, 255, 0);



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

String message = "";

// Variabelen voor LED-besturing

bool ledState = false;

// bool blinking = false;

unsigned long previousMillis = 0;

const long interval = 500;

unsigned int DeurStatus = 0;

unsigned int current_input = 0;

// Variabelen voor de schakelaar

unsigned int old_input = 0;

bool knopOpBit0;

bool knopOpBit1;
bool stopBlinking = false;
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



  if (!client.connected()) {

    VerbindServer();
  }

  while (client.connected()) {

    //eerst kijken of er een bericht is:
    
    //Serial.println("Debug punt 1");

    if (client.available()) {
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer

      }


      //Serial.println(strlen(message));

      if (message.charAt(0) == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        client.write("ACK");
      }

      if (message.charAt(0)=='0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
         client.write("ACK");
      }
      //unsigned int temp = atoi(message.c_str());
      Serial.println("Dit is het ontvangen bericht van de server");
      Serial.println(message);
       
     // Serial.print(knopOpBit1);
   
    }

    if (message.charAt(1) == '1' ) {
      Serial.println("message = 1");
      Deur();
    }

    if (message.charAt(1) == '2') {
      myServo.write(70);
    }

    if (message.charAt(1) == '3') {
      Deur2();

    }

    if (message.charAt(1) == '4') {
      Serial.print("Hij moet nu blinken");
      blinking();
   
    }

    if (message.charAt(1) == '6') {
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(byte(1 << 4 )); // Zet LEDs uit
      Serial.print("Het is nu dag modus");
      Wire.endTransmission();

    }
    if (message.charAt(1) == '5') {
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(byte(0)); // Zet LEDs aan
      Wire.endTransmission();
    //  Serial.print("Het is nu nachtmodus");
    }
    if (message.charAt(1) == '8') {
      myServo.write(195);
    }
     if (message.charAt(1) == '9') {
      myServo.write(70);
    }
    //client.flush();
    //is er verandering en staat er geen bericht om eerst uit te lezen?
    delay(200);
    LeesSchakelaars();
    //Serial.println("Waardes geupdate!");
    if (!client.available() && knopOpBit0 == 1) {
      Serial.println("De data is gewijzigd");
      int vier = 4;
      client.print(vier);
      //nu wachten op een ack van de server
      while (!client.available()) {
        Serial.println("Aan het wachten op een ACK bericht van de server");
        delay(10);
      }
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter

        message += c;
      }
      // wacht op de data die geretouneerd wordt
      message = "";
      client.write("ACK");
      client.flush();

    }

  }

}

void LeesSchakelaars() {

  // Lees de knopstatus via I2C

  Wire.beginTransmission(I2C_ADDRESS);

  Wire.write(byte(0x00));

  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDRESS, 1); // Vraag 1 byte aan

  if (Wire.available()) {
    current_input = Wire.read(); // Lees de volledige byte
    // Toon de binaire waarde van current_input
  ///  Serial.println("Dit is de current input in binaire vorm:");
    //Serial.println(current_input, BIN);
    // Controleer een specifieke knopstatus (bijv. bit 2)
    knopOpBit1 = (current_input & (1 << 1)) != 0;

    knopOpBit0 = (current_input & (1 << 0)) != 0;
 //   Serial.print("Waarde button: ");

   // Serial.println(knopOpBit1);

 //   Serial.print("Waarde button0: ");

  //  Serial.println(knopOpBit0);

    delay(50);
  }
}

void VerbindServer() {

  Serial.print("connecting to ");
  Serial.print(server);
  Serial.print(':');
  Serial.println(port);
  if (!client.connect(server, port)) {
    Serial.println("Connection failed!");
    return;
  }
  Serial.println("Connected to server");
  //Serial.print('1');
  while (!client.available()) {
    delay(500);
  }

  Serial.print("message before read ");
  Serial.println(message);
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

  Serial.print("message before read ");
  Serial.println(message);
  message = "";
  while (client.available()) {
    char c = client.read();  // Lees een karakter
    message += c;            // Voeg het toe aan de buffer

  }
  Serial.print("message after read ");
  Serial.println(message);
  if (message == "ACK") {
    Serial.println("Server heeft correct ontvangen");
  } else {
    Serial.println("Onverwacht antwoord van server(verbind server)");
  }

}
void DataCheck() {

  //Serial.println("Welkom in datacheck");

  //Serial.println(client.available());

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
    if (message.charAt(1) == '1') {

      Serial.println("message = 1");

      Deur();
    }
    if (message.charAt(1) == '2') {

      myServo.write(70);
    }

    if (message.charAt(1) == '3') {
      Deur2();
    }

    if (message.charAt(1) == '4') {
      Serial.print("Hij moet nu blinken");
      blinking();
    }

    if (message.charAt(1) == '6') {
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(byte(1 << 4 )); // Zet LEDs uit
      Serial.print("Het is nu Dag modus");
      Wire.endTransmission();

    }
    if (message.charAt(1) == '5') {
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(byte(0)); // Zet LEDs uit
      Wire.endTransmission();
     // Serial.print("Het is nu Nachtmodus modus");

    }
      if (message.charAt(1) == '8') {
      myServo.write(195);
    }
     if (message.charAt(1) == '9') {
      myServo.write(70);
    }

  }

}

void drie(){
 
   
    // writing
    LeesSchakelaars();
    if (!client.available() && knopOpBit1 == 1) {
      
   stopBlinking = true;
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(byte(1 << 4 )); // Zet LEDs uit
     
      Wire.endTransmission();
        // Stop knipperen als er een nieuw bericht is
        Serial.println("Knippermodus gestopt door nieuw bericht.");
      // Eventueel eenmalig actie ondernemen bij knopdruk
      Serial.println("Knop tijdens knipperen ingedrukt!");
      int drie = 3;
      Serial.print("waarde knopopbit1: ");
        Serial.print(knopOpBit1);
        knopOpBit1 == 0 ; // Reset knopstatus
      client.print(drie);
     
      while(!client.available()){
        Serial.println("aan het wachten op een ACK bericht van de server");
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
      client.flush();
    }

      //reading
      if (client.available()) {
      message = "";
      while (client.available()) {
        char c = client.read();  // Lees een karakter
        message += c;            // Voeg het toe aan de buffer
      }
      //Serial.println(strlen(message));
      if (message.charAt(0) == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        client.write("ACK");
      }
      if (message.charAt(0)=='0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
         client.write("ACK");
      }
      //unsigned int temp = atoi(message.c_str());
      Serial.println("Dit is het ontvangen bericht van de server");
    
      Serial.println(message);

      if (message.charAt(1) != 4){
      stopBlinking = true;
      }
      }
      }
    
void blinking() {
  Serial.println("Start knippermodus...");

   stopBlinking = false;
  unsigned long blinkStartTime = millis(); // Begin tijd bijhouden
  const unsigned long blinkDuration = 8000; // Max knipperduur, bijv. 5 seconden

  while (!stopBlinking) {
    unsigned long currentMillis = millis();

    // Stop automatisch na verloop van tijd
    if (currentMillis - blinkStartTime >= blinkDuration) {
      stopBlinking = true;
      break;
    }
    // Wissel de LED-status
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(ledState ? byte(1 << 5) : byte(0x00)); // LED aan/uit
      Wire.endTransmission();
    }
      drie();
    }
    delay(50); // Minimal delay om CPU-belasting te voorkomen
  if (message.charAt(1) == '1' ) {
      Serial.println("message = 1");
      Deur();
    }
    if (message.charAt(1) == '2') {

      myServo.write(70);
    }
    if (message.charAt(1) == '3') {
      
    }
    if (message.charAt(1) == '6') {
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(byte(1 << 4 )); // Zet LEDs aan
   //   Serial.print("Het is nu nacht modus");
      Wire.endTransmission();
    }
    if (message.charAt(1) == '5') {
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(byte(0)); // Zet LEDs uit
      Wire.endTransmission();
      Serial.print("Het is nu dag modus");
    }
    if (message.charAt(1) == '8') {
      myServo.write(195);
}
  if (message.charAt(1) == '9') {
      myServo.write(70);
}
}
void Deur() {
  Serial.println("Deur openen...");
  myServo.write(195);
  for (int a = 0; a <= 8000; a++) {
    if (message.charAt(1) == '2') {  // Controleer of message gelijk is aan "1"
      Serial.println("Deur sluiten door Mary...");
      break;
    }
    DataCheck();
    delay(10);

  }
  Serial.println("Deur sluiten...");
  myServo.write(70);
}

void Deur2() {
    Serial.println("Deur openen2...");
    myServo.write(195); // Open de deur
    delay(8000);        // Wacht 3 seconden (of de gewenste tijd)
    Serial.println("Deur sluiten2...");
    myServo.write(70);  // Sluit de deur
   DataCheck();
}
