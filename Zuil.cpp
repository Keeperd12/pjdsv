#include "Zuil.h"
#include "Server.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>

Zuil::Zuil(int fd, int type, Server *s) : Client(fd, type, s), Button(0), Brandmelder(0)
{

    // std::cout << "Object gemaakt" << std::endl;
}

Zuil::~Zuil() {}

void Zuil::SetWaarde(int Waarde)
{
    unsigned int Brandmelder = (Waarde >> 1) & 0x03FF;
    unsigned int Button = Waarde & 0x01;
    // std::cout << " Waarde Brandmelder " << Brandmelder << std::endl;
    // std::cout << " Waarde Button " << Button << std::endl;
    // return;
}
char *Zuil::GeefData()
{
    return nullptr;
}

int Zuil::GetValueBrandmelder() const
{
    return Brandmelder;
}

int Zuil::GetValueButton() const
{
    return Button;
}
void Zuil::Update(char *bericht)
{
    std::cout << strlen(bericht) << std::endl;
    if (strlen(bericht) == 3)
    {
        std::cout << "Het bericht is van de zuil zelf" << std::endl;

        int data = atoi(bericht);

        this->Brandmelder = (data >> 1) & 0x03FF;
        this->Button = data & 0x01;

        MoetIkIetsDoen(bericht);
    }
}
void Zuil::MoetIkIetsDoen(char *bericht)
{
    // check of er brand is
    std::string brand;
    std::string button;
    if (this->Brandmelder >= 300)
    {
        brand = "1";
        StatusZoemer = 1;
        // hier de
    }
    else
    {
        StatusZoemer = 0;
        brand = '0';
    }
    if (this->Button == 1)
    {
        button = '1';
        StatusLed = 1;
    }
    else{
        button = '0';
        StatusLed = 0;
    }
    char temp[4];
    temp[0] = '0';
    temp[1] = brand[0];
    temp[2] = button[0];
    temp[3] = '\0';
    server->stuurBericht(GeefFD(), temp);
}