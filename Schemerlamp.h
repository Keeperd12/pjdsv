#pragma once // Voorkomen van meervoudige inclusie van de header file

#include "Client.h"

class Schemerlamp : public Client // Overerven van de Client klasse
{
public:
    Schemerlamp(int, int, Server *); // Constructor
    ~Schemerlamp();                  // Destructor
    virtual void Update(char *);     // Functie voor het updaten van de Schemerlamp
    virtual char *GeefData();        // Functie voor het geven van de data van de Schemerlamp
    void moetIkIetsDoen(char *bericht);

    // Functies nog toevoegen
private:
    int waardeLed;
    int beweging;
    // Variabelen nog toevoegen
};
