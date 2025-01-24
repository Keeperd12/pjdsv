#pragma once

#include "Client.h"


class Muur : public Client
{
public:
    Muur(int, int, Server*);
    virtual void Update(char*); //updaten variabelen
    virtual void moetIkIetsDoen(char *); //moet ik iets doen
    virtual char* GeefData() override; //geef data
    virtual void UpdateSchemerlamp();
    virtual void UpdateDoor();
    virtual void updateMuur();
    virtual ~Muur();
private:
    int StatusPotmeter;
    int StatusLED;
    int StatusLDR;
    int StatusLCD;

};