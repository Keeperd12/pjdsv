#pragma once // Voorkomen van meervoudige inclusie van de header file

#include "Client.h"

class Deur : public Client // Overerven van de Client klasse
{
public:
    Deur(int, int, Server *); // Constructor
    ~Deur();                  // Destructor
    virtual void Update(char *);
    virtual char *GeefData();
    virtual void moetIkIetsDoen(char *); // moet ik iets doen
    void SetWaarde(int Waarde);
    int GetValueButton() const;

    // void SetWaarde(int Waarde);
    // int GetValueButton() const;

    // Functies nog toevoegen
private:
    unsigned int Button;
    unsigned int Statusdeur;
    // Variabelen nog toevoegen
};