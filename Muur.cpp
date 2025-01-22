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

        std::string LDRDeel(bericht, 10);
        this->StatusLDR = std::bitset<10>(LDRDeel).to_ulong();

        std::string PotDeel(bericht + 10, 10); 
        this->StatusPotmeter = std::bitset<10>(PotDeel).to_ulong();

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
    
    UpdateSchemerlamp();
}
void Muur::moetIkIetsDoen(char *bericht)
{

    std::bitset<8> binaryStatusLED(StatusLED);      
    std::string str1 = binaryStatusLED.to_string(); 

    std::string str2;
    if (StatusLDR >= 300)
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

    char temp[20];
    temp[0] = '0';
    temp[1] = str2[0];
    strcpy(temp + 2, str1.c_str());

    server->stuurBericht(GeefFD(), temp);


}

char *Muur::GeefData()
{
    char *data = new char[50];                    
    std::string str1 = std::to_string(StatusLED);
    std::string str2 = std::to_string(StatusLDR); 
    std::string combined = str1 + "," + str2;     
    strcpy(data, combined.c_str());               
    return data;
}
void Muur::UpdateSchemerlamp()
{
    std::bitset<8> binaryStatusLED(StatusLED); 
    std::string str1 = binaryStatusLED.to_string();
    char *data = new char[50];
    strcpy(data, str1.c_str());
    for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
    {
        if (it->second->GeefType() == 2) //is het object een type schemerlamp?
        {
            it->second->Update(data); //verstuur de helderheid waarde naar de klasse schemerlamp
        }
    }
}
