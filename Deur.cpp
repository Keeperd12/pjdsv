#include "Deur.h"
#include "Server.h"
#include <iostream>

Deur::Deur(int fd, int Type, Server *s) : Client(fd, Type, s) {}
Deur::~Deur() {}

void Deur::Update(char *bericht)
{
    if (strcmp(bericht, "1") == 0)
    {
        std::cout<< "De deur heeft ontvangen (deur open)" << std::endl;
       //mary bewaking
        SetWaarde(1);
    }
    if (strcmp(bericht, "2") == 0)
    {
        std::cout<< "De deur heeft ontvangen (deur sluiten)" << std::endl;
       //mary bewaking
        SetWaarde(2);
    }
      if (strcmp(bericht, "3") == 0)
    {
        std::cout<< "De deur heeft ontvangen (deur openen)" << std::endl;
       //mary bewaking
        SetWaarde(3);
    }
    if (strcmp(bericht, "4") == 0)
    {
        SetWaarde(4);
        //deur
    }
    if(strcmp(bericht, "5")== 0){
        std::cout<< "De deur heeft ontvangen dat het dag is" << std::endl;
        SetWaarde(5);
        //muur
    }
    if(strcmp(bericht, "6")== 0){
        std::cout<< "De deur heeft ontvangen dat het nacht is" << std::endl;
        SetWaarde(6);
    }
    if(strcmp(bericht, "8")== 0){
        std::cout<< "De deur heeft ontvangen dat er brand is deur gaat open" << std::endl;
        SetWaarde(8);
    }
    if(strcmp(bericht, "9")== 0){
        std::cout<< "Geen brand meer deur gaat dicht" << std::endl;
        SetWaarde(9);
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
        temp[1] = '1';
        temp[2] = '\0';
        std::cout << temp << std::endl;
        server->stuurBericht(GeefFD(), temp);
    }
    if (this->Button == 2)
    {
        char temp[20];
        temp[0] = '1';
        temp[1] = '2';
        temp[2] = '\0';
        std::cout << temp << std::endl;
        server->stuurBericht(GeefFD(), temp);
    }
     if (this->Button == 3)
    {

        char temp[20];
        temp[0] = '0';
        temp[1] = '3';
        temp[2] = '\0';
        std::cout << temp << std::endl;
        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 4) // is het object een type Bewaking
            {
                it->second->Update("3");
            }
        }
        server->stuurBericht(GeefFD(), temp);
    }
    if (this->Button == 4)
    {

        char temp[20];
        temp[0] = '0';
        temp[1] = '4';
        temp[2] = '\0';
        std::cout << temp << std::endl;
        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 4) // is het object een type Bewaking
            {
                it->second->Update("2");
            }
        }
        server->stuurBericht(GeefFD(), temp);
    }
    if(this->Button ==5){
        char temp[20];
        temp[0] = '1';
        temp[1] = '5';
        temp[2] = '\0';
        server->stuurBericht(GeefFD(), temp);
    }
    if(this->Button ==6){
        char temp[20];
        temp[0] = '1';
        temp[1] = '6';
        temp[2] = '\0';
        server->stuurBericht(GeefFD(), temp);
    }
     if(this->Button ==8){
        char temp[20];
        temp[0] = '1';
        temp[1] = '8';
        temp[2] = '\0';
        server->stuurBericht(GeefFD(), temp);
    }
    if(this->Button ==9){
        char temp[20];
        temp[0] = '1';
        temp[1] = '9';
        temp[2] = '\0';
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

void Deur::SetWaarde(int Waarde)
{
    Button = Waarde;
}