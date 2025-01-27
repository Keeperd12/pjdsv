#pragma once // Voorkomen van meervoudige inclusie van de header file

#include "Client.h" 

class Schemerlamp : public Client // Overerven van de Client klasse
{
public:
    Schemerlamp(int, int, Server *); // Constructor
    virtual ~Schemerlamp();                  // Destructor
    virtual void Update(char *);     // Functie voor het updaten van de Schemerlamp
    virtual char *GeefData();        // Functie voor het geven van de data van de Schemerlamp, doet niks in .cpp is alleen hier van toepassing voor eventuele toekomstige uitbreiding
    virtual void moetIkIetsDoen(char *bericht); //Functie voor het uitvoeren van een signaal adhv het bericht

private:
    int waardeLed;
    int beweging;
};
