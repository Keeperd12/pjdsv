#include "Muur.h"
#include <iostream>
#include <bitset>
#include "Server.h"
#include <string>
#include <memory>

Muur::Muur(int fd, int Type, Server *s) : Client(fd, Type, s) {}
Muur::~Muur() {}
void Muur::Update(char *bericht)
{
    // deze instructie is wanneer het bericht van de muur (wemos) komt
    if (strlen(bericht) == 20)
    {
        //isoleer eerste 10 bits en zet om naar een int in statusLDR
        std::string LDRDeel(bericht, 10);
        this->StatusLDR = std::bitset<10>(LDRDeel).to_ulong();
        //isoleer de laatste 10 bits en zet om naar een int in StatusPotmeter
        std::string PotDeel(bericht + 10, 10);
        this->StatusPotmeter = std::bitset<10>(PotDeel).to_ulong();

        // reken de status potmeter om naar de genormaliseerde waarde statusLed (0-255)
        this->StatusLED = (StatusPotmeter * 255) / 1023;
        
        moetIkIetsDoen(bericht);
    }
    // deze instructie is om de LCD aan te sturen
    if (strlen(bericht) == 1)
    {
        if (bericht[0] == '1')
        {
            this->StatusLCD = 1; //statusLCD variabel update
            std::bitset<8> binaryStatusLED(StatusLED); // converteer naar binair
            std::string str1 = binaryStatusLED.to_string(); //zet binair naar een string
            char temp[20]; //tijdelijke char om te vertsturen
            temp[0] = '1'; // bericht voor wemos nog ack terugsturen want mary.
            temp[1] = bericht[0]; //bericht voor wemos gewenste stand lcd
            strcpy(temp + 2, str1.c_str()); //kopieer binair naar temp vanaf [2]
            server->stuurBericht(GeefFD(), temp); //verstuur data in juist formaat 
        }
        if (bericht[0] == '0')
        {
            this->StatusLCD = 0; //statusLCD variabel update
            std::bitset<8> binaryStatusLED(StatusLED);//converteer naar binair
            std::string str1 = binaryStatusLED.to_string(); //zet binair in een string
            char temp[20]; //tijdelijke char om te vertsturen
            temp[0] = '1'; // bericht voor wemos nog ack terugsturen
            temp[1] = bericht[0]; //bericht voor wemos gewenste stand lcd
            strcpy(temp + 2, str1.c_str()); //kopieer binair naar temp vanaf [2]
            server->stuurBericht(GeefFD(), temp); //verstuur data in juist formaat 
        }
    }
    //instructie van mary om led strip naar sfeer stand te zetten
    if (strlen(bericht) == 2)
    {
        this->StatusLED = std::stoi(bericht);// update de waarde van de LED strip gebruik stoi (string to integer)
        std::bitset<8> binaryStatusLED(StatusLED); // Converteer naar binair
        std::string str1 = binaryStatusLED.to_string(); //zet binair in een string
        char temp[20]; //tijdelijke char om te versturen
        temp[0] = '1'; // bericht voor wemos nog ack terugsturen

        //kijk of het dag of nacht is
        if (StatusLDR >= 300)
        {
            temp[1] = '1'; // het is dag venster open
        }
        else
        {
            temp[1] = '0'; // het is nacht venster dicht
        }
        strcpy(temp + 2, str1.c_str()); //kopieer binair naar temp vanaf [2]
        server->stuurBericht(GeefFD(), temp); //verstuur data in juist formaat
    }
    //synchroniseer de schemerlamp altijd met de LEDHelderheid
    UpdateSchemerlamp();
    //update alle deuren of het nacht of dag is
    UpdateDoor();
}
void Muur::moetIkIetsDoen(char *bericht)
{
    std::bitset<8> binaryStatusLED(StatusLED); //converteer statusLED naar binair
    std::string str1 = binaryStatusLED.to_string(); //zet binair in string
    std::string str2; //initaliseer een string voor waarde dag of nacht

    //check of dag of nacht is
    if (StatusLDR >= 300) //het is dag
    {
        str2 = "1"; //waarde 1: venster open
        this->StatusLCD = 1; //update variabel StatusLCD
    }
    else //het is geen dag, maar nacht
    {
        str2 = "0"; //waarde 0: venster dicht
        this->StatusLCD = 0; //update variabel StatusLCD
    }

    char temp[20]; //initiseer tijdelijke char om juiste formaat in te kunnen zenden
    temp[0] = '0'; //bericht is een reactie
    temp[1] = str2[0]; //data voor aansturen lcd
    strcpy(temp + 2, str1.c_str()); 

    server->stuurBericht(GeefFD(), temp); //verstuur data in juist formaat
}

char *Muur::GeefData()
{
    // converteer alle int statussen naar een string
    std::string str0 = std::to_string(StatusPotmeter);
    std::string str1 = std::to_string(StatusLED);
    std::string str2 = std::to_string(StatusLDR);
    std::string str3 = std::to_string(StatusLCD);
    // combineer alle strings in een
    std::string combined = "\r\nMuur Potmeter= " + str0 + ", LDR: " + str2 + " waarde helderheid LED strip: " + str1 + " Status LCD=  " + str3 + "\r\n";
    // unique smart pointer aanmaken voor de char array
    std::unique_ptr<char[]> data(new char[combined.size() + 1]); //maakt ruimte voor een nieuwe char met de lengte van combined
    strcpy(data.get(), combined.c_str()); // kopieer de string naar de char array

    return data.release(); // geef de functie die de pointer ontvangt "ownership" en laat deze die zelf deleten
}

void Muur::UpdateSchemerlamp()
{
    std::bitset<8> binaryStatusLED(StatusLED); //converteer statusLed naar binar
    std::string str1 = binaryStatusLED.to_string(); //zet binair in string
    for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
    {
        if (it->second->GeefType() == 2) // is het object een type schemerlamp?
        {
            it->second->Update(str1.c_str()); //geef direct de string mee
        }
    }
}
void Muur::UpdateDoor()
{
    std::cout << "UpdateDoor" << std::endl;
    if (this->StatusLDR >= 300)
    {
        // het is dag, geef een 5 aan de deuren
        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 3) // is het object een type Deur?
            {
                it->second->Update("5"); // verstuur een 5 (Geen idee waarom 5, maar dat wilde Devesh)
            }
        }
    }
    else
    {
        // het is nacht, geef een 6 aan de deuren
        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 3) // is het object een type Deur?
            {
                it->second->Update("6"); // verstuur een 6 (Geen idee waarom 6, maar dat wilde Devesh)
            }
        }
    }
}

