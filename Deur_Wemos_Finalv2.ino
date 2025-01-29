#include <Servo.h>        // Bibliotheek voor het aansturen van een servo 
#include <Wire.h>         // Bibliotheek voor I2C-communicatie
#include <ESP8266WiFi.h>  // Bibliotheek voor Wi-Fi

// Pin-definities
#define SERVO_PIN D5  // De pin waarop de servo is aangesloten
#define I2C_ADDRESS 0x38  // I2C-adres van PCA9554A

// Wi-Fi-configuratie
const char* ssid = "PiVanOli4";  // SSID van je Wi-Fi netwerk
const char* password = "WachtwoordPiVanOli4";  // Wachtwoord voor Wi-Fi netwerk
WiFiClient client;  // Client object voor Wi-Fi verbinding
const char* server = "192.168.10.1";  // IP-adres van de server
const uint16_t port = 8080;  // Poortnummer voor de server
IPAddress local_Ip(192, 168, 10, 20); // Statisch IP adres voor ESP8266
IPAddress gateway(192, 168, 10, 1); // Standaard gateway van het netwerk
IPAddress subnet(255, 255, 255, 0); // Subnetmasker

// Servo-object
Servo myServo;  // Object voor de servo

// Variabelen voor deurbesturing
bool doorOpen = false;  // Houdt bij of de deur open is
unsigned long doorTimer = 0;  // Tijd bijhouden voor het openen/sluiten van de deur
const long doorDelay = 8000;  // Vertraging voor de deur in milliseconden
unsigned long openDelay = 3000;  // Vertraging voordat de deur sluit na openen
unsigned long switchPressedTime = 0;  // Tijd van de knopdruk
bool switchPressed = false;  // Status van de knop
bool closeDoorRequested = false;  // Of de deur gesloten moet worden
String message = "";  // Opslag voor berichten van de server

// Variabelen voor LED-besturing
bool ledState = false;  // Houdt de status van de LED bij
unsigned long previousMillis = 0;  // Tijd voor het beheren van de LED knippering
const long interval = 500;  // Interval voor knipperen van de LED in milliseconden

// Variabelen voor de schakelaar
unsigned int DeurStatus = 0;  // De status van de deur
unsigned int current_input = 0;  // Huidige waarde van de input van de schakelaars

unsigned int old_input = 0;  // Oude waarde van de input
bool knopOpBit0;  // Status van knop op bit 0
bool knopOpBit1;  // Status van knop op bit 1
bool stopBlinking = false;  // Zet de knippermodus stop

// Setup functie, deze wordt maar één keer uitgevoerd bij het opstarten
void setup() {
  Serial.begin(115200);  // Zet de seriële communicatie aan met een baudrate van 115200

  // Start de Wi-Fi-verbinding
  WiFi.mode(WIFI_STA);  // Zet de ESP8266 in station mode (als Wi-Fi client)
  WiFi.config(local_Ip, gateway, subnet);  // Configureer een statisch IP
  Serial.print("Verbinding maken met Wi-Fi...");
  WiFi.begin(ssid, password);  // Maak verbinding met het Wi-Fi netwerk
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);  // Wacht 1 seconde voor het opnieuw proberen
    Serial.print(".");
  }
  Serial.println("\nVerbonden met Wi-Fi!");  // Laat weten dat de verbinding gelukt is
  Serial.print("IP-adres: ");
  Serial.println(WiFi.localIP());  // Print het lokale IP-adres van de ESP8266

  // Initialiseer de servo
  myServo.attach(SERVO_PIN);  // Koppel de servo aan de gedefinieerde pin
  myServo.write(72);  // Zet de servo in de gesloten positie

  // Initialiseer I2C
  Wire.begin();  // Start de I2C communicatie

  // Configureer de PCA9554A als output
  Wire.beginTransmission(I2C_ADDRESS);  // Begin een communicatie naar het I2C-apparaat
  Wire.write(byte(0x03));  // Stel register DDR in
  Wire.write(byte(0x0F));  // Zet alle pins als output
  Wire.endTransmission();  // Eindig de transmissie

  // Zet LEDs uit bij opstarten
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(byte(0x01));  // Output register
  Wire.write(byte(0x00));  // Zet LEDs uit
  Wire.endTransmission();
}


void loop() {
  
  // Controleer of de client niet verbonden is, zo ja, verbind met de server
  if (!client.connected()) {
    VerbindServer();
  }

  // Terwijl de client verbonden is, blijf in deze while-loop
  while (client.connected()) {

    // Controleer of er een nieuw bericht beschikbaar is van de server
    if (client.available()) {
      message = "";  // Reset de message variabele om het nieuwe bericht op te slaan
      // Lees de beschikbare karakters van de client en voeg ze toe aan de message string
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de server
        message += c;            // Voeg het karakter toe aan de message string
      }

      // Check of het eerste teken in het bericht '1' is
      if (message.charAt(0) == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        client.write("ACK");  // Stuur een bevestiging (ACK) naar de server
      }

      // Check of het eerste teken in het bericht '0' is
      if (message.charAt(0) == '0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
        client.write("ACK");  // Stuur een bevestiging (ACK) naar de server
      }
      // Print het ontvangen bericht naar de seriële monitor
      Serial.println("Dit is het ontvangen bericht van de server");
      Serial.println(message);
    }

    // Controleer de tweede teken van het bericht en voer de actie uit
    if (message.charAt(1) == '1') {
      Serial.println("message = 1");
      Deur();  // Open de deur
    }

    // Als het tweede teken '2' is, sluit de deur door de servo naar 70 graden te sturen
    if (message.charAt(1) == '2') {
      myServo.write(70);  // Zet de servo naar 70 graden (de deur sluiten)
    }

    // Als het tweede teken '3' is, voer de deur2 functie uit
    if (message.charAt(1) == '3') {
      Deur2();  // Open en sluit de deur op een andere manier
    }

    // Als het tweede teken '4' is, start de knippermodus
    if (message.charAt(1) == '4') {
      Serial.print("Hij moet nu blinken");
      blinking();  // Start de knipperfunctie
    }

    // Als het tweede teken '6' is, zet de LEDs in dagmodus
    if (message.charAt(1) == '6') {
      Wire.beginTransmission(I2C_ADDRESS);  // Start communicatie met I2C
      Wire.write(byte(0x01));  // Zet het output register
      Wire.write(byte(1 << 4));  // Zet de LEDs uit (voor dagmodus)
      Serial.print("Het is nu dag modus");
      Wire.endTransmission();  // Eindig de communicatie
    }

    // Als het tweede teken '5' is, zet de LEDs in nachtmodus
    if (message.charAt(1) == '5') {
      Wire.beginTransmission(I2C_ADDRESS);  // Start communicatie met I2C
      Wire.write(byte(0x01));  // Zet het output register
      Wire.write(byte(0));  // Zet de LEDs aan (voor nachtmodus)
      Wire.endTransmission();  // Eindig de communicatie
    }

    // Als het tweede teken '8' is, open de deur volledig
    if (message.charAt(1) == '8') {
      myServo.write(195);  // Zet de servo naar 195 graden (de deur openen)
    }

    // Als het tweede teken '9' is, sluit de deur
    if (message.charAt(1) == '9') {
      myServo.write(70);  // Zet de servo naar 70 graden (de deur sluiten)
    }

    // Kleine vertraging om de processor niet te veel te belasten
    delay(200);

    // Lees de status van de schakelaars
    LeesSchakelaars();

    // Als de client geen bericht heeft en de knop is ingedrukt (knopOpBit0 is 1)
    if (!client.available() && knopOpBit0 == 1) {
      Serial.println("De data is gewijzigd");
      int vier = 4;
      client.print(vier);  // Stuur een bericht (bijvoorbeeld 4) naar de server
      // Wacht op een bevestiging (ACK) van de server
      while (!client.available()) {
        Serial.println("Aan het wachten op een ACK bericht van de server");
        delay(10);  // Wacht 10 milliseconden voor de server om een reactie te sturen
      }
      message = "";  // Reset de message variabele
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de server
        message += c;            // Voeg het karakter toe aan de message string
      }

      // Wacht op de data die door de server wordt geretourneerd
      message = "";
      client.write("ACK");  // Stuur een bevestiging (ACK) naar de server
      client.flush();  // Zorg ervoor dat alle gegevens zijn verzonden
    }
  }
}


void LeesSchakelaars() {
  
  // Begin I2C-communicatie om de status van de schakelaars te lezen
  Wire.beginTransmission(I2C_ADDRESS);  // Start communicatie met het apparaat op I2C-adres

  // Schrijf het registeradres 0x00, wat aangeeft dat we de status van de knoppen willen lezen
  Wire.write(byte(0x00));

  Wire.endTransmission();  // Eindig de transmissie (verstuur het commando naar het apparaat)

  // Vraag 1 byte gegevens van het apparaat (de status van de knoppen)
  Wire.requestFrom(I2C_ADDRESS, 1);

  // Als er gegevens beschikbaar zijn, lees dan de status van de knoppen
  if (Wire.available()) {
    current_input = Wire.read();  // Lees de ontvangen byte en sla deze op in current_input
    
    // Controleer de status van de knop op bit 1 (tweede bit)
    knopOpBit1 = (current_input & (1 << 1)) != 0;

    // Controleer de status van de knop op bit 0 (eerste bit)
    knopOpBit0 = (current_input & (1 << 0)) != 0;

    delay(50);  // Wacht 50 milliseconden voordat je opnieuw de knoppenstatus controleert
  }
}

void VerbindServer() {

  // Print het IP-adres en poortnummer van de server waar verbinding mee wordt gemaakt
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.print(':');
  Serial.println(port);

  // Probeer verbinding te maken met de server op het opgegeven IP-adres en poortnummer
  if (!client.connect(server, port)) {
    Serial.println("Connection failed!");  // Als de verbinding mislukt, geef dan een foutmelding
    return;
  }

  Serial.println("Connected to server");  // Als de verbinding is gelukt, geef dan een succesbericht
  
  // Wacht totdat de server een bericht heeft verzonden
  while (!client.available()) {
    delay(500);  // Wacht 500 milliseconden voordat je opnieuw controleert
  }

  Serial.print("message before read ");
  Serial.println(message);
  message = "";  // Reset de message variabele om het ontvangen bericht op te slaan
  
  // Lees alle beschikbare bytes van de server
  while (client.available()) {
    char c = client.read();  // Lees een karakter van de server
    message += c;            // Voeg het karakter toe aan de message string
  }
  Serial.println(message);  // Print het ontvangen bericht naar de seriële monitor
  
  // Stuur een bericht "Deur" naar de server
  client.print("Deur");

  // Wacht totdat er een reactie van de server beschikbaar is
  while (!client.available()) {
    delay(500);  // Wacht 500 milliseconden voordat je opnieuw controleert
  }

  // Lees alle beschikbare bytes van de server
  Serial.print("message before read ");
  Serial.println(message);
  message = "";  // Reset de message variabele om het ontvangen bericht op te slaan
  while (client.available()) {
    char c = client.read();  // Lees een karakter van de server
    message += c;            // Voeg het karakter toe aan de message string
  }
  
  // Print het ontvangen bericht van de server
  Serial.print("message after read ");
  Serial.println(message);

  // Controleer of het ontvangen bericht gelijk is aan "ACK"
  if (message == "ACK") {
    Serial.println("Server heeft correct ontvangen");  // Bevestig dat de server het bericht correct heeft ontvangen
  } else {
    Serial.println("Onverwacht antwoord van server(verbind server)");  // Foutmelding als het antwoord onverwacht is
  }
}

void DataCheck() {

  // Als de client een bericht heeft ontvangen
  if (client.available()) {
    Serial.println("in datacheck Client available");
    
    message = "";  // Reset de message variabele om het ontvangen bericht op te slaan

    // Lees alle beschikbare bytes van de server
    while (client.available()) {
      char c = client.read();  // Lees een karakter van de server
      message += c;            // Voeg het karakter toe aan de message string
    }
    
    // Print het ontvangen bericht naar de seriële monitor
    Serial.print("Ontvangen bericht: ");
    Serial.println(message);

    // Stuur een bevestigingsbericht "ACK" naar de server
    client.print("ACK");

    // Controleer de tweede teken van het bericht (message.charAt(1)) en voer acties uit
    if (message.charAt(1) == '1') {
      Serial.println("message = 1");
      Deur();  // Open de deur
    }
    
    if (message.charAt(1) == '2') {
      myServo.write(70);  // Zet de servo naar 70 graden (de deur sluiten)
    }

    if (message.charAt(1) == '3') {
      // Geen actie wordt uitgevoerd als het tweede teken '3' is
    }

    if (message.charAt(1) == '4') {
      Serial.print("Hij moet nu blinken");
      blinking();  // Start de knipperfunctie
    }

    if (message.charAt(1) == '5') {
      Wire.beginTransmission(I2C_ADDRESS);  // Start I2C-communicatie
      Wire.write(byte(0x01));  // Zet het output register
      Wire.write(byte(1 << 4));  // Zet LEDs aan (dagmodus)
      Serial.print("Het is nu Dag modus");
      Wire.endTransmission();  // Eindig de communicatie
    }

    if (message.charAt(1) == '6') {
      Wire.beginTransmission(I2C_ADDRESS);  // Start I2C-communicatie
      Wire.write(byte(0x01));  // Zet het output register
      Wire.write(byte(0 << 4));  // Zet LEDs uit (nachtmodus)
      Wire.endTransmission();  // Eindig de communicatie
    }

    if (message.charAt(1) == '8') {
      myServo.write(195);  // Zet de servo naar 195 graden (de deur openen)
    }

    if (message.charAt(1) == '9') {
      myServo.write(70);  // Zet de servo naar 70 graden (de deur sluiten)
    }
  }
}
void drie(){

  // Leest de schakelaarsstatus om te controleren of een knop is ingedrukt
  LeesSchakelaars();
  
  // Als er geen clientberichten beschikbaar zijn en knopOpBit1 is ingedrukt
  if (!client.available() && knopOpBit1 == 1) {

    // Stop de knippermodus
    stopBlinking = true;

    // Begin I2C-communicatie en zet de LEDs uit
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(byte(0x01)); // Output register
    Wire.write(byte(1 << 4)); // Zet LEDs uit
    Wire.endTransmission();

    // Print bericht dat de knippermodus is gestopt
    Serial.println("Knippermodus gestopt door nieuw bericht.");

    // Print bericht dat de knop werd ingedrukt tijdens knipperen
    Serial.println("Knop tijdens knipperen ingedrukt!");

    // Print de waarde van knopOpBit1 en reset de knopstatus
    Serial.print("waarde knopopbit1: ");
    Serial.print(knopOpBit1);
    knopOpBit1 == 0;  // Reset knopstatus naar 0

    // Verstuur het bericht '3' naar de server
    client.print(3);

    // Wacht op een bevestiging (ACK) van de server
    while (!client.available()) {
      Serial.println("aan het wachten op een ACK bericht van de server");
      delay(25); // Wacht 25 milliseconden
    }

    // Lees het ontvangen bericht van de server
    message = "";
    while (client.available()) {
      char c = client.read();  // Lees een karakter
      message += c;            // Voeg het karakter toe aan de message buffer
    }

    // Verstuur een ACK naar de server
    message = "";
    client.write("ACK");
    client.flush();  // Zorg ervoor dat het bericht wordt verzonden
  }

  // Leest berichten van de client als deze beschikbaar zijn
  if (client.available()) {
    message = "";
    while (client.available()) {
      char c = client.read();  // Lees een karakter van de server
      message += c;            // Voeg het toe aan de message buffer
    }

    // Print het ontvangen bericht naar de seriële monitor
    Serial.println("Dit is het ontvangen bericht van de server");
    Serial.println(message);

    // Als de tweede karakter van het bericht niet gelijk is aan '4', stop knipperen
    if (message.charAt(1) != '4') {
      stopBlinking = true;
    }
  }
}

void blinking() {
  Serial.println("Start knippermodus...");

  stopBlinking = false;  // Zet stopBlinking op false om knipperen mogelijk te maken
  unsigned long blinkStartTime = millis(); // Begin tijd bijhouden
  const unsigned long blinkDuration = 5000; // Max knipperduur, bijv. 5 seconden

  // Blijf knipperen totdat stopBlinking op true wordt gezet of tijd is verstreken
  while (!stopBlinking) {
    unsigned long currentMillis = millis();

    // Stop automatisch na de maximale knipperduur
    if (currentMillis - blinkStartTime >= blinkDuration) {
      stopBlinking = true;  // Stop het knipperen na 5 seconden
      break;
    }

    // Wissel de LED-status (aan/uit) op basis van de interval
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;  // Update de vorige tijdstempel
      ledState = !ledState;  // Verander de LED-status

      // Begin I2C-communicatie en zet de LED aan of uit, afhankelijk van de status
      Wire.beginTransmission(I2C_ADDRESS);
      Wire.write(byte(0x01)); // Output register
      Wire.write(ledState ? byte(1 << 5) : byte(0x00)); // LED aan/uit
      Wire.endTransmission();
    }

    // Roep de functie 'drie' aan die de knopstatus controleert
    drie();

    delay(50);  // Minimal delay om CPU-belasting te voorkomen
  }

  // Verwerk berichten van de server na het knipperen
  if (message.charAt(1) == '1') {
    Serial.println("message = 1");
    Deur();  // Open de deur
  }
  if (message.charAt(1) == '2') {
    myServo.write(70);  // Zet de servo naar 70 graden (de deur sluiten)
  }
  if (message.charAt(1) == '3') {
    Deur2();  // Open de tweede deur
  }
  if (message.charAt(1) == '5') {
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(byte(0x01)); // Output register
    Wire.write(byte(1 << 5)); // Zet LEDs aan (nachtmodus)
    Wire.endTransmission();
  }
  if (message.charAt(1) == '6') {
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(byte(0x01)); // Output register
    Wire.write(byte(0)); // Zet LEDs uit (dagmodus)
    Wire.endTransmission();
    Serial.print("Het is nu dag modus");
  }
  if (message.charAt(1) == '8') {
    myServo.write(195);  // Zet de servo naar 195 graden (de deur openen)
  }
  if (message.charAt(1) == '9') {
    myServo.write(70);  // Zet de servo naar 70 graden (de deur sluiten)
  }
}

void Deur() {
  Serial.println("Deur openen...");

  myServo.write(195);  // Open de deur door de servo naar 195 graden te draaien

  // Wacht tot de deur volledig is geopend (tot 8000 ms)
  for (int a = 0; a <= 8000; a++) {
    if (message.charAt(1) == '2') {  // Controleer of message gelijk is aan '2' (de deur sluiten)
      Serial.println("Deur sluiten door Mary...");
      break;  // Breek uit de loop als de deur gesloten moet worden
    }
    DataCheck();  // Controleer of er nieuwe data van de server is
    delay(10);    // Wacht 10 milliseconden voordat de status opnieuw wordt gecontroleerd
  }

  // Sluit de deur
  Serial.println("Deur sluiten...");
  myServo.write(70);  // Zet de servo naar 70 graden (de deur sluiten)
}

void Deur2() {
  Serial.println("Deur openen2...");

  myServo.write(195); // Open de deur door de servo naar 195 graden te draaien
  delay(8000);         // Wacht tot de deur volledig geopend is (8 seconden)

  Serial.println("Deur sluiten2...");
  myServo.write(70);  // Sluit de deur door de servo naar 70 graden te draaien

  DataCheck();  // Controleer of er nieuwe data van de server is
}
