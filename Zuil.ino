#include <Wire.h>         // Bibliotheek voor I2C communicatie
#include <ESP8266WiFi.h>  // Bibliotheek voor Wi-Fi functionaliteit op de ESP8266

// Wi-Fi verbindingsgegevens
const char* ssid = "Ramon's";         // Naam van het Wi-Fi netwerk
const char* password = "ramondirksen"; // Wachtwoord voor het Wi-Fi netwerk

// Servergegevens
const char* host = "server";  // Vervang door het IP-adres van je server
const uint16_t port = 8080;   // Poortnummer van de server

// I2C pinnen voor communicatie met I2C-apparaten
#define I2C_SDL    D1  // Kloklijn van I2C (SCL)
#define I2C_SDA    D2  // Datallijn van I2C (SDA)

// Netwerkinstellingen
IPAddress local_Ip(192, 168, 10, 50); // Lokale IP van de ESP8266
IPAddress gateway(192, 168, 122, 41); // Gateway van het netwerk
IPAddress subnet(255, 255, 255, 0);   // Subnetmasker

// Variabelen voor sensoren en inputs
unsigned int anin0 = 0;              // Waarde van de analoge ingang 0
unsigned int old_anin0 = 0;          // Vorige waarde van analoge ingang 0
unsigned int false_input = 0;         // Onjuiste input waarde
unsigned int old_i = 0;              // Vorige waarde van de input
String i;                            // Input als string
unsigned int input = 0;              // Waarde van de input
String message;                      // Ontvangen bericht
unsigned int gecombineerd = 0;        // Gecombineerde waarde van sensoren

// Variabelen voor de deur LED
unsigned long previousMillis = 0;   // Tijd in milliseconden
const long interval = 500;          // Interval in milliseconden (bijvoorbeeld voor knipperen)
bool ledState = false;              // Huidige status van de LED (aan/uit)
bool stopBlinking = true;           // Vlag om het knipperen van de LED te stoppen

// Wi-Fi client object voor communicatie met de server
WiFiClient client;

void setup() {
  Wire.begin();                  // Start de I2C communicatie
  Serial.begin(115200);           // Begin seriële communicatie op 115200 baudrate
  delay (1000);                   // Wacht 1 seconde voor stabiliteit

  // Configureer I2C-apparaten via schrijfcommando's
  Wire.beginTransmission(0x38);  // Start communicatie met I2C-apparaat op adres 0x38
  Wire.write(byte(0x03));        // Schrijf byte 0x03 naar het apparaat
  Wire.write(byte(0x01));        // Schrijf byte 0x01 naar het apparaat
  Wire.endTransmission();        // Eindig de I2C-communicatie

  Wire.beginTransmission(0x36);  // Start communicatie met I2C-apparaat op adres 0x36
  Wire.write(byte(0xA2));        // Schrijf byte 0xA2 naar het apparaat
  Wire.write(byte(0x03));        // Schrijf byte 0x03 naar het apparaat
  Wire.endTransmission();        // Eindig de I2C-communicatie

  Wire.beginTransmission(0x38);  // Start communicatie met I2C-apparaat op adres 0x38
  Wire.write(byte(0x01));        // Schrijf byte 0x01 naar het apparaat
  Wire.write(byte(0));           // Zet de waarde van de tweede byte op 0
  Wire.endTransmission();        // Eindig de I2C-communicatie

  // Start de Wi-Fi verbinding
  Serial.println("Connecting to Wi-Fi...");

  WiFi.mode(WIFI_STA);                   // Zet de ESP8266 in de station-modus
  WiFi.config(local_Ip, gateway, subnet); // Stel het statische IP, gateway en subnet in
  WiFi.begin(ssid, password);            // Verbind met het opgegeven Wi-Fi netwerk

  // Wacht totdat de ESP8266 verbonden is met het netwerk
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);  // Wacht 1 seconde tussen pogingen
    Serial.print("."); // Print punt om voortgang aan te geven
  }

  // Als verbonden, geef de informatie weer in de seriële monitor
  Serial.println();
  Serial.println("Wi-Fi connected!");  // Print bericht wanneer de verbinding is gelukt
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());      // Print het lokale IP-adres van de ESP8266
}


void loop() {
  // Controleer of de client verbonden is met de server
  if (!client.connected()) {
    VerbindServer();  // Verbind met de server als de verbinding is verbroken
  }

  // Als de client verbonden is met de server, voer de volgende taken uit
  while (client.connected()) {
    // Als er geen beschikbare gegevens zijn van de client en er een verandering is in de analoge waarde en de knopstatus
    if (!client.available() && CheckButtonAndAnalog()) {
      // Combineer de analoge waarde en de knopstatus
      gecombineerd = (anin0 << 1) | input;
      client.print(gecombineerd);  // Stuur de gecombineerde waarde naar de server

      // Wacht totdat er een antwoord van de server komt
      while (!client.available()) {
        Serial.println("zit in functie VerstuurData (loop).");
        delay(25);  // Wacht 25 ms voor de server om een reactie te geven
      }

      message = "";  // Reset de boodschap buffer

      // Lees het bericht van de server
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de server
        message += c;            // Voeg het karakter toe aan het bericht
      }

      Serial.print("ontvangen bericht na verturen data: ");
      Serial.println(message);  // Print het ontvangen bericht

      // Controleer of het bericht een "ACK" is, wat betekent dat de server de data heeft ontvangen
      if (message == "ACK") {
        Serial.println("Server heeft correct ontvangen");
      }

      message = "";  // Reset de boodschap buffer na verwerking
    }

    // Als er wel gegevens van de client beschikbaar zijn
    if (client.available()) {
      message = "";  // Reset de boodschap buffer

      // Lees alle beschikbare bytes van de client
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de client
        message += c;            // Voeg het karakter toe aan de buffer
      }

      // Als het eerste karakter van het bericht '1' is, stuur een ACK
      if (message.charAt(0) == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        //client.write("ACK"); // (Dit is uitgeschakeld in de code)
      }

      // Als het eerste karakter van het bericht '0' is, stuur een ACK
      if (message.charAt(0) == '0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
        client.write("ACK");  // Stuur een bevestigingsbericht terug naar de server
      }

      // Toon het volledige ontvangen bericht in de seriële monitor
      Serial.print("Ontvangen bericht: ");
      Serial.println(message);

      // Voer een functie uit op basis van het ontvangen bericht
      LedZoemer();  // Roep de functie aan die de LED en de zoemer aanstuurt
    }

    delay(200);  // Wacht 200 ms voordat de loop opnieuw wordt uitgevoerd
  }
}
//delay(800);


void LedZoemer() {
  int zoemer;
  int led;
  int outputs;

  // Als het tweede karakter van het bericht '1' is, zet zoemer op 1 (aan)
  if (message.charAt(1) == '1') {
    zoemer = 1;
  } else {
    zoemer = 0;
  }

  // Als het derde karakter van het bericht '1' is, zet led op 1 (aan)
  if (message.charAt(2) == '1') {
    led = 1;
  } else {
    led = 0;
  }

  // Als het vierde karakter van het bericht '1' is, voer de blinkZoemer functie uit
  if (message.charAt(3) == '1') {
    blinkZoemer();  // Functie om de zoemer te laten knipperen
  }

  // Print de waarden van zoemer en led naar de seriële monitor
  Serial.print("zoemer: ");
  Serial.println(zoemer);
  Serial.print("led: ");
  Serial.println(led);

  // Stel de uitvoerwaarde in op basis van de zoemer- en ledstatus
  if (zoemer == 0 && led == 1) {
    outputs = 32;  // LED aan, zoemer uit
  }
  if (zoemer == 1 && led == 0) {
    outputs = 16;  // Zoemer aan, LED uit
  }
  if (zoemer == 1 && led == 1) {
    outputs = 48;  // Zoemer en LED aan
  }
  if (zoemer == 0 && led == 0) {
    outputs = 0;   // Zoemer en LED uit
  }

  // Stuur de uitvoerwaarde naar een I2C-apparaat (bijvoorbeeld voor een display of actuator)
  Wire.beginTransmission(0x38);  // Begin I2C-communicatie met apparaat op adres 0x38
  Wire.write(byte(0x01));        // Schrijf registeradres 0x01
  Wire.write(byte(outputs));     // Schrijf de berekende uitvoerwaarde
  Wire.endTransmission();        // Eindig de I2C-communicatie
}

void blinkZoemer() {
  delay(20);  // Wacht 20 ms voordat de knippermodus start
  Serial.println("Start knippermodus...");
  stopBlinking = false;  // Zet de stopBlinking variabele op false om de knippermodus te starten

  // Zolang stopBlinking nog niet is ingesteld op true, blijft de zoemer knipperen
  while (!stopBlinking) {
    unsigned long currentMillis = millis();  // Verkrijg de huidige tijd in milliseconden

    // Controleer of het interval van 500ms verstreken is
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;  // Update de vorige tijd naar de huidige tijd
      ledState = !ledState;  // Wissel de status van de LED (aan of uit)

      // Start een I2C-communicatie met het apparaat op adres 0x38
      Wire.beginTransmission(0x38);
      Wire.write(byte(0x01));  // Schrijf naar register 0x01 (bijvoorbeeld de status van de LED)

      // Zet de LED aan of uit afhankelijk van de waarde van ledState
      if (ledState) {
        Wire.write(byte(16));  // Zet LED aan (16 is een specifieke waarde voor het aanzetten)
      } else {
        Wire.write(byte(0));   // Zet LED uit
      }
      Wire.endTransmission();  // Eindig de I2C-communicatie
    }

    // Als er geen beschikbare data is van de client, maar er is verandering in de knop en analoge input
    if (!client.available() && CheckButtonAndAnalog()) {
      gecombineerd = (anin0 << 1) | input;  // Combineer de analoge input en de knopstatus
      client.print(gecombineerd);  // Stuur de gecombineerde waarde naar de server

      // Wacht tot er een reactie van de server komt
      while (!client.available()) {
        Serial.println("zit in functie VerstuurData (loop).");
        delay(25);  // Wacht 25 ms voor de server om een reactie te geven
      }

      message = "";  // Reset de boodschap buffer

      // Lees het ontvangen bericht van de server
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de server
        message += c;            // Voeg het toe aan de buffer
      }
      Serial.print("ontvangen bericht na verturen data: ");
      Serial.println(message);  // Print het ontvangen bericht naar de seriële monitor

      // Controleer of de server een "ACK" heeft gestuurd (bevestiging van ontvangst)
      if (message == "ACK") {
        Serial.println("Server heeft correct ontvangen");
      }

      message = "";  // Reset de boodschap buffer na verwerking
    }

    // Als er wel gegevens beschikbaar zijn van de client
    if (client.available()) {
      message = "";  // Reset de boodschap buffer

      // Lees alle beschikbare bytes van de client
      while (client.available()) {
        char c = client.read();  // Lees een karakter van de client
        message += c;            // Voeg het toe aan de buffer
      }

      // Als het eerste karakter van het bericht '1' is, stuur een ACK
      if (message.charAt(0) == '1') {
        Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
        //client.write("ACK");  // (Dit is uitgeschakeld in de code)
      }

      // Als het eerste karakter van het bericht '0' is, stuur een ACK
      if (message.charAt(0) == '0') {
        Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
        client.write("ACK");  // Stuur een bevestigingsbericht terug naar de server
      }

      // Toon het volledige ontvangen bericht in de seriële monitor
      Serial.print("Ontvangen bericht: ");
      Serial.println(message);

      // Stop de knippermodus en stuur de LED en zoemer aan
      stopBlinking = true;
      LedZoemer();
    }

    delay(1);  // Wacht 1 ms voordat de loop opnieuw wordt uitgevoerd
  }
}

void VerbindServer() {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Probeer verbinding te maken met de server op het opgegeven adres en poort
  if (!client.connect(gateway, port)) {
    Serial.println("Connection failed!");  // Foutmelding als de verbinding niet lukt
    return;  // Stop de functie als de verbinding mislukt
  }

  Serial.println("Connected to server");
  
  // Wacht totdat er gegevens van de server beschikbaar zijn
  while (!client.available()) {
    delay(500);
  }

  // Lees alle beschikbare bytes van de server
  message = "";  // Reset de boodschap buffer
  while (client.available()) {
    char c = client.read();  // Lees een karakter van de server
    message += c;            // Voeg het toe aan de buffer
  }
  Serial.println(message);  // Toon het ontvangen bericht in de seriële monitor

  // Stuur een bericht naar de server om de client te identificeren
  client.print("Zuil");

  // Wacht op een antwoord van de server
  while (!client.available()) {
    delay(500);
  }

  // Lees het antwoord van de server
  message = "";
  while (client.available()) {
    char c = client.read();  // Lees een karakter van de server
    message += c;            // Voeg het toe aan de buffer
  }

  // Controleer of de server de boodschap heeft ontvangen
  if (message == "ACK") {
    Serial.println("Server heeft correct ontvangen");
  }
}

int CheckButtonAndAnalog() {
  // Lees analoge 10-bit input op kanaal 0
  if (old_anin0 == 0) {
    Wire.requestFrom(0x36, 4);  // Vraag 4 bytes aan van het apparaat op adres 0x36 via I2C
    anin0 = Wire.read() & 0x03; // Lees de lage 2 bits
    anin0 = anin0 << 8;          // Verschuif deze bits naar de hogere byte
    anin0 = anin0 | Wire.read(); // Voeg de volgende byte toe aan de waarde
    old_anin0 = anin0 - 30;     // Sla de waarde op voor latere vergelijking (30 is een correctie)
    return 0;  // Retourneer 0 om aan te geven dat de analoge waarde is gelezen
  }

  Wire.requestFrom(0x36, 4);  // Vraag opnieuw 4 bytes aan via I2C
  anin0 = Wire.read() & 0x03; // Lees de lage 2 bits
  anin0 = anin0 << 8;          // Verschuif de bits naar de hogere byte
  anin0 = anin0 | Wire.read(); // Voeg de volgende byte toe aan de waarde

  // Controleer of de analoge waarde meer dan 20 eenheden is veranderd ten opzichte van de vorige waarde
  // en of de waarde tussen 10 en 450 ligt (filter om te voorkomen dat ongewenste waardes worden geteld)
  if (((anin0 > old_anin0 + 20) || (anin0 < old_anin0 - 20)) && (anin0 > 10 && anin0 < 450 )) {
    old_anin0 = anin0;  // Werk de vorige waarde bij
    return 1;           // Retourneer 1 om aan te geven dat de analoge waarde significant is veranderd
  }

  // Vraag de digitale inputstatus van het apparaat op adres 0x38 via I2C
  Wire.beginTransmission(0x38);  // Start I2C-communicatie
  Wire.write(byte(0x00));        // Schrijf naar register 0x00
  Wire.endTransmission();        // Eindig de transmissie
  Wire.requestFrom(0x38, 1);     // Vraag 1 byte aan van apparaat 0x38
  int current_input = Wire.read();  // Lees de digitale inputstatus

  // Vergelijk de huidige input met de vorige input
  input = (current_input & (1 << 0)) != 0;  // Haal de waarde van de eerste bit (knopstatus) op

  // Print de inputstatus naar de seriële monitor
  Serial.print("data input: ");
  Serial.println(input);

  // Als de status is veranderd, retourneer dan 1 om aan te geven dat de input is veranderd
  if (input != false_input) {
    return 1;
  }

  return 0;  // Retourneer 0 als er geen verandering is
}
void checkData() {
  if (client.available()) {  // Als er gegevens beschikbaar zijn van de server
    message = "";  // Reset de boodschap buffer

    // Lees alle beschikbare bytes van de server
    while (client.available()) {
      char c = client.read();  // Lees een karakter van de server
      message += c;            // Voeg het toe aan de buffer
    }

    // Controleer het eerste karakter van het bericht
    if (message.charAt(0) == '1') {
      Serial.println("deze message is ge initiseerd door de server, dus nog een ack sturen");
      //client.write("ACK");  // (Dit is uitgeschakeld in de code)
    }

    if (message.charAt(0) == '0') {
      Serial.println("Dit bericht is een reactie op een zelf verzonden bericht");
      client.write("ACK");  // Stuur een bevestiging naar de server
    }

    // Toon het volledige ontvangen bericht in de seriële monitor
    Serial.print("Ontvangen bericht: ");
    Serial.println(message);

    // Stop de knippermodus en voer de LedZoemer() functie uit
    stopBlinking = true;
    LedZoemer();
  }
}
