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

char *Zuil::GeefData()
{
    return nullptr;
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

    }

    if (strlen(bericht) == 1)
    {
        if (strcmp(bericht, "1") == 0){
            this->HulpMary = 1;
        }
        if (strcmp(bericht, "2") == 0){
            this->ZoemerBlink = 1;
        }
         if (strcmp(bericht, "3") == 0){
            this->ZoemerBlink = 0;
        }
    }
    MoetIkIetsDoen(bericht);
}
void Zuil::MoetIkIetsDoen(char *bericht)
{
    //sleep(1);
    //  check of er brand is
    std::cout << "in MoetIkIetsDoen" << std::endl;
    std::string brand;
    std::string led;
    std::string blink;
    if (this->Brandmelder >= 300)
    {   
        brand = "1";
        StatusZoemer = 1;
        StatusLed = 1;

        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 6) // is het object een type Bewaking
            {
                server->stuurBericht(it->first, "Brand bij Bewoner Mary!\n");
            }
        }
        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 3) // is het object een type Bewaking
            {
                it->second->Update("8");
            }
        }

    }
    else
    {
        StatusZoemer = 0;
        brand = '0';
        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 3) // is het object een type Bewaking
            {
                it->second->Update("9");
            }
        }
    }
    if (this->Button == 1)
    {
        led = '1';
        StatusLed = 1;
    }
    else
    {

        if (this->HulpMary != 1)
        {
            led = '0';
            StatusLed = 0;
        }
    }
    if (this->HulpMary == 1)
    {
        led = '1';
        StatusLed = 1;
    }
    else
    {
        if (this->Button != 1)
        {
            led = '0';
            StatusLed = 0;
        }
    }
    if ((this->Button == 1 || this->HulpMary == 1))
    {
        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 6) // is het object een type Bewaking
            {
                server->stuurBericht(it->first, "Bewoner Mary heeft hulp nodig\n");
            }
        }
    }
    if (this->ZoemerBlink == 1)
        {
            blink = '1';
        }
    else{
        blink = '0';
    }

    char temp[4];
    temp[0] = '0';
    temp[1] = brand[0];
    temp[2] = led[0];
    temp[3] = blink[0];
    temp[4] = '\0';
    std::cout << "data dat verstuurd wordt " << temp << std::endl;

    server->stuurBericht(GeefFD(), temp);
}
