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
    if(strlen(bericht) ==8){
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
    // Er is beweging gedetecteerd
    if (beweging == 255)
    {
        std::cout << "Er is beweging, signaal moet nog doorgegeven worden aan de bewaker als dit niet Mary is." << std::endl;

        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++) //Voor alle clients in de map, zoek naar Mary.
        {
            if (it->second->GeefType() == 5) //Voor elke client met het type van Mary
            {
                Client *client = it->second; //Creër een pointer naar een Client object uit de base class.
                Mary *mary = dynamic_cast<Mary*>(client); //Zet pointer via dynamic cast om naar de afgeleide klasse Mary, om toegang te krijgen tot alle functies specifiek van Mary.
                if(mary->isMaryThuis() == 0) //Vraag op of Mary thuis is, en als dat niet het geval is:
                {
                    for(auto its = server->GeefPointerMap().begin(); its != server->GeefPointerMap().end(); its++) //Zoek naar de bewaker in de map
                    {
                        if(its->second->GeefType() == 6) //Als de bewaker gevonden is:
                        {
                            server->stuurBericht(its->first, "Inbraak! Beweging in Mary haar appartement!\n"); //Stuur een bericht dat er inbraak is.
                        }
                    }
                }
            }
        }
    }
    if (beweging == 254) //De beweging is gestopt
    {
        std::cout << "Er is geen beweging meer, signaal moet nog doorgegeven worden aan Mary" << std::endl;

        for (auto it = server->GeefPointerMap().begin(); it != server->GeefPointerMap().end(); it++) //Zoek naar Mary in de clients map
        {
            if (it->second->GeefType() == 5)
            {
                Client *client = it->second; //Creër een pointer naar een Client object uit de base class.
                Mary *mary = dynamic_cast<Mary*>(client); //Zet pointer via dynamic cast om naar de afgeleide klasse Mary, om toegang te krijgen tot alle functies specifiek van Mary.
                if(mary->isMaryThuis() == 0) //Vraag op of Mary thuis is, en als dat niet het geval is:
                {
                    server->stuurBericht(it->first, "Er is bij je ingebroken! Er is nu geen beweging meer!"); //Laat aan Mary weten dat er geen beweging in haar appartement meer is.
                }
            }
        }
    }
}