#include "Muur.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <bitset>
#include "Server.h"
#include <string>

Muur::Muur(int fd, int Type, Server *s) : Client(fd, Type, s) {}
Muur::~Muur() {}
void Muur::Update(char *bericht)
{
     std::cout << "strlen van het bericht: " << strlen(bericht) << std::endl;
    if (strlen(bericht) == 20)
    {
        // std::cout << "Het bericht voor de muur is succesvol ontvangen: " << bericht << std::endl;
        // Converteer de eerste 10 bits naar LDRDeel
        std::string LDRDeel(bericht, 10); // Neem de eerste 10 tekens
        unsigned int LDR = std::bitset<10>(LDRDeel).to_ulong();
        // Converteer de tweede 10 bits naar PotDeel
        std::string PotDeel(bericht + 10, 10); // Neem de volgende 10 tekens
        unsigned int Potmeter = std::bitset<10>(PotDeel).to_ulong();

        // voor evt visueel printen van de waarde voor debug redenen
        // std::cout << "Waarde1: " << LDR << std::endl;
        // std::cout << "Waarde2: " << Potmeter << std::endl;

        // toezeggen van bericht waarde aan het object
        this->StatusLDR = LDR;
        this->StatusPotmeter = Potmeter;
        // reken de status potmeter om naar de waarde statusLed (0-255)
        this->StatusLED = (StatusPotmeter * 255) / 1023;
        moetIkIetsDoen(bericht);
    }
    if (strlen(bericht) == 1)
    {
        std::cout << "Dit bericht is om de lcd aan te sturen" << std::endl;
        if (bericht[0] == '1')
        {
            this->StatusLCD = 1;
            std::bitset<8> binaryStatusLED(StatusLED); // Convert to binary
            std::string str1 = binaryStatusLED.to_string();
            std::cout << "De lcd moet open" << std::endl;
            char temp[20];
            temp[0] = '1'; //bericht voor wemos nog ack terugsturen
            temp[1] = bericht[0];
            strcpy(temp + 2, str1.c_str());
            server->stuurBericht(GeefFD(), temp);
        }
        if (bericht[0] == '0')
        {
            this->StatusLCD = 0;
            std::bitset<8> binaryStatusLED(StatusLED); // Convert to binary
            std::string str1 = binaryStatusLED.to_string();
            std::cout << "De lcd moet open" << std::endl;
            char temp[20];
            temp[0] = '1'; //bericht voor wemos nog ack terugsturen
            temp[1] = bericht[0];
            strcpy(temp + 2, str1.c_str());
            server->stuurBericht(GeefFD(), temp);
            std::cout << "De lcd moet sluiten" << std::endl;
        }
    }
}
void Muur::moetIkIetsDoen(char *bericht)
{
    // reken en normaliseer de waarde voor de helderheid uit de potmeter

    // std::cout << "Helderheid: " << StatusLED << std::endl;
    //  std::string str1 = std::to_string(StatusLED); // Zet om naar std::string
    std::bitset<8> binaryStatusLED(StatusLED);      // Convert to binary
    std::string str1 = binaryStatusLED.to_string(); // Convert to std::string
    // str1 = str1.substr(str1.size() - 3);
    // std::string str2 = std::to_string(StatusPotmeter); // Zet om naar std::string
    std::string str2;
    if (StatusLDR >= 200)
    {
        std::cout << "het is dag"
                     ""
                  << std::endl;
        str2 = "1";
        StatusLCD = 1;
    }
    else
    {
        std::cout << "het is nacht" << std::endl;
        str2 = "0";
        StatusLCD = 0;
    }
    // int lengteBericht = strlen(str);
    char temp[20];
    temp[0] = '0'; 
    temp[1] = str2[0];
    strcpy(temp + 2, str1.c_str());
    // std::cout << "De inhoudt van temp: " << temp << std::endl;
    // std::cout << "De lengte van de str1 is : " << strlen(temp) << std::endl;
    server->stuurBericht(GeefFD(), temp);
    // sleep(2500);
}

char *Muur::GeefData()
{
    char *data = new char[50]; // Allocate enough space for concatenated string
    std::string str1 = std::to_string(StatusLED); // Convert to std::string
    std::string str2 = std::to_string(StatusLDR); // Convert to std::string
    std::string combined = str1 + "," + str2; // Combine with a delimiter
    strcpy(data, combined.c_str()); // Copy combined string to data
    std::cout << "De data van de muur is: " << data << std::endl;
    return data;
}