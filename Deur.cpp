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
}


void Deur::moetIkIetsDoen(char *bericht)
{
    if (this->Button == 1)
    {
        char temp[20];
        temp[0] = '1';
        temp[1] = '\0';
        std::cout << temp << std::endl;
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
}

void Deur::SetWaarde(int Waarde)
{
    Button = Waarde;

}
