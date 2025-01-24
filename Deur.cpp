#include "Deur.h"
#include "Server.h"
#include <iostream>

Deur::Deur(int fd, int Type, Server *s) : Client(fd, Type, s) {}
Deur::~Deur() {}

void Deur::Update(char *bericht)
{
    if (strcmp(bericht, "1") == 0)
    {
        SetWaarde(1);
    }
    if (strcmp(bericht, "2") == 0)
    {
        SetWaarde(2);
    }
    if (strcmp(bericht, "4") == 0)
    {
        SetWaarde(4);
    }
    if(strcmp(bericht, "5")== 0){
        std::cout<< "De deur heeft ontvangen dat het nacht is" << std::endl;
        SetWaarde(5);
    }
    if(strcmp(bericht, "6")== 0){
        std::cout<< "De deur heeft ontvangen dat het nacht is" << std::endl;
        SetWaarde(6);
    }
    moetIkIetsDoen(bericht);
}

void Deur::moetIkIetsDoen(char *bericht)
{
    //sleep(1);
    if (this->Button == 1)
    {
        char temp[20];
        temp[0] = '1';
        temp[1] = '\0';
        std::cout << temp << std::endl;
        server->stuurBericht(GeefFD(), temp);
    }
    if (this->Button == 2)
    {
        char temp[20];
        temp[0] = '2';
        temp[1] = '\0';
        std::cout << temp << std::endl;
        server->stuurBericht(GeefFD(), temp);
    }
    if (this->Button == 4)
    {

        char temp[20];
        temp[0] = '4';
        temp[1] = '\0';
        std::cout << temp << std::endl;
        server->stuurBericht(GeefFD(), temp);
    }
    if(this->Button ==5){
        char temp[20];
        temp[0] = '5';
        temp[1] = '\0';
        server->stuurBericht(GeefFD(), temp);
    }
    if(this->Button ==6){
        char temp[20];
        temp[0] = '6';
        temp[1] = '\0';
        server->stuurBericht(GeefFD(), temp);
    }
}

char *Deur::GeefData()
{
    // char *data = new char[50]; // Allocate enough space for concatenated string
    // std::string str1 = std::to_string(StatusLED); // Convert to std::string
    //  std::string str2 = std::to_string(StatuLDR); // Convert to std::string
    // std::string combined = str1 + "," + stsr2; // Combine with a delimiter
    //  strcpy(data, combined.c_str()); // Copy combined string to data
    //  std::cout << "De data van de muur is: " << data << std::endl;
    return "1";
}

int Deur::GetValueButton() const
{
    return Button;
    // server->stuurBericht(GeefType(), "test");
}

void Deur::SetWaarde(int Waarde)
{
    Button = Waarde;
}