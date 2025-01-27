#include "Muur.h"
#include <iostream>
// #include <stdlib.h>
// #include <stdio.h>
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

        std::string LDRDeel(bericht, 10);
        this->StatusLDR = std::bitset<10>(LDRDeel).to_ulong();

        std::string PotDeel(bericht + 10, 10);
        this->StatusPotmeter = std::bitset<10>(PotDeel).to_ulong();

        // reken de status potmeter om naar de waarde statusLed (0-255)
        this->StatusLED = (StatusPotmeter * 255) / 1023;
        moetIkIetsDoen(bericht);
    }
    // deze instructie is om de LCD aan te sturen
    if (strlen(bericht) == 1)
    {

        if (bericht[0] == '1')
        {
            this->StatusLCD = 1;
            std::bitset<8> binaryStatusLED(StatusLED); // Convert to binary
            std::string str1 = binaryStatusLED.to_string();
            char temp[20];
            temp[0] = '1'; // bericht voor wemos nog ack terugsturen
            temp[1] = bericht[0];
            strcpy(temp + 2, str1.c_str());
            server->stuurBericht(GeefFD(), temp);
        }
        if (bericht[0] == '0')
        {
            this->StatusLCD = 0;
            std::bitset<8> binaryStatusLED(StatusLED); // Convert to binary
            std::string str1 = binaryStatusLED.to_string();
            char temp[20];
            temp[0] = '1'; // bericht voor wemos nog ack terugsturen
            temp[1] = bericht[0];
            strcpy(temp + 2, str1.c_str());
            server->stuurBericht(GeefFD(), temp);
        }
    }
    // deze instructie is om de LED helderheid aan te sturen vanaf terminal marry
    if (strlen(bericht) == 2)
    {
        // update de waarde van de LED strip
        this->StatusLED = std::stoi(bericht);

        std::bitset<8> binaryStatusLED(StatusLED); // Convert to binary
        std::string str1 = binaryStatusLED.to_string();
        char temp[20];
        temp[0] = '1'; // bericht voor wemos nog ack terugsturen
        if (StatusLDR >= 300)
        {
            // het is dag venster open
            temp[1] = '1';
        }
        else
        {
            // het is nacht venster dicht
            temp[1] = '0';
        }
        strcpy(temp + 2, str1.c_str());
        server->stuurBericht(GeefFD(), temp);
    }
    //sync de schemerlamp LED
    UpdateSchemerlamp();
    //geef door of het dag of nacht is aan de deur
    UpdateDoor();
}
void Muur::moetIkIetsDoen(char *bericht)
{

    std::bitset<8> binaryStatusLED(StatusLED);
    std::string str1 = binaryStatusLED.to_string();

    std::string str2;
    if (StatusLDR >= 300)
    {
        std::cout << "het is dag" << std::endl;
        str2 = "1";
        this->StatusLCD = 1;
    }
    else
    {
        std::cout << "het is nacht" << std::endl;
        str2 = "0";
        this->StatusLCD = 0;
    }

    char temp[20];
    temp[0] = '0';
    temp[1] = str2[0];
    strcpy(temp + 2, str1.c_str());

    server->stuurBericht(GeefFD(), temp);
}

char *Muur::GeefData()
{
    // converteer alle int naar een string
    std::string str0 = std::to_string(StatusPotmeter);
    std::string str1 = std::to_string(StatusLED);
    std::string str2 = std::to_string(StatusLDR);
    std::string str3 = std::to_string(StatusLCD);
    // combineer de strings
    std::string combined = "\r\nMuur Potmeter= " + str0 + ", LDR: " + str2 + " waarde helderheid LED strip: " + str1 + " Status LCD=  " + str3 + "\r\n";
    // unique smart pointer aanmaken voor de char array
    std::unique_ptr<char[]> data(new char[combined.size() + 1]); //+1 voor \0
    strcpy(data.get(), combined.c_str());                        // kopieer de string naar de char array

    return data.release(); // geef de pointer terug en release data
}

void Muur::UpdateSchemerlamp()
{
    std::cout << "UpdateSchemerlamp" << std::endl;
    std::bitset<8> binaryStatusLED(StatusLED);
    std::string str1 = binaryStatusLED.to_string();
    char *data = new char[50];
    strcpy(data, str1.c_str());
    for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
    {
        if (it->second->GeefType() == 2) // is het object een type schemerlamp?
        {
            it->second->Update(data); // verstuur de helderheid waarde naar de klasse schemerlamp
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
            if (it->second->GeefType() == 3) // is het object een type schemerlamp?
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
            if (it->second->GeefType() == 3) // is het object een type schemerlamp?
            {
                it->second->Update("6"); // verstuur een 6 (Geen idee waarom 6, maar dat wilde Devesh)
            }
        }
    }
}

void Muur::updateMuur(){
    for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
    {
        if (it->second->GeefType() == 1) // is het object een type schemerlamp?
        {
            //it->second->Update // verstuur de helderheid waarde naar de klasse schemerlamp
        }
    }
}