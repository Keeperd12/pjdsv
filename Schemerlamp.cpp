#include "Schemerlamp.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "Server.h"
#include <bitset>

Schemerlamp::Schemerlamp(int fd, int Type, Server *s) : Client(fd, Type, s) {}
Schemerlamp::~Schemerlamp() {}

void Schemerlamp::Update(char *bericht)
{
    std::cout << "De lengte van het bericht is: " << strlen(bericht) << std::endl;
    // bericht is van de schemerlamp zelf!
    if (strlen(bericht) == 3)
    {
        this->beweging = atoi(bericht);
        moetIkIetsDoen(bericht);
    }
    //dit is een update van de potmeter van de muur
    if(strlen(bericht) ==8)
    {
        char temp[20];
        strcpy(temp, bericht);
        //std::string TempWaardeLed(bericht);
       // this->waardeLed = std::bitset<8>(TempWaardeLed).to_ulong();
        //char* temp;
        //temp[0] = 1;
        //strcpy(temp + 1, TempWaardeLed.c_str());
        server->stuurBericht(GeefFD(), temp);
    }
}

char *Schemerlamp::GeefData()
{
    return nullptr;
}

void Schemerlamp::moetIkIetsDoen(char *bericht)
{
    // kijken of er beweging is
    if (beweging == 255)
    {
        std::cout << "Er is beweging, signaal moet nog doorgegeven worden aan de bewaker als dit niet Mary is." << std::endl;

        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 5)
            {
                Client *client = it->second;
                Mary *mary = dynamic_cast<Mary*>(client);
                if(mary->isMaryThuis() == 0)
                {
                    for(auto its = server->GeefPointerMap().begin(); its != server->GeefPointerMap().end(); its++)
                    {
                        if(its->second->GeefType() == 6)
                        {
                            server->stuurBericht(its->first, "Inbraak! Beweging in Mary haar appartement!\n" );
                        }
                    }
                }
                //server->stuurBericht(it->first, "Er is beweging!");
            }
        }
    }
    if (beweging == 254)
    {
        std::cout << "Er is geen beweging meer, signaal moet nog doorgegeven worden aan Mary" << std::endl;

        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++)
        {
            if (it->second->GeefType() == 5)
            {
                Client *client = it->second;
                Mary *mary = dynamic_cast<Mary*>(client);
                if(mary->isMaryThuis() == 0)
                {
                    server->stuurBericht(it->first, "Er is bij je ingebroken! Er is nu geen beweging meer!");
                }
            }
        }
    }
}
