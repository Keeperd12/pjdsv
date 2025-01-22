#pragma once //Voorkomen van meervoudige inclusie van de header file

#include "Client.h"

class Deur : public Client //Overerven van de Client klasse
{
public:
    Deur(int, int, Server*); //Constructor
    virtual void Update(char*) override;
    virtual void MoetIkIetsDoen(char*);
    ~Deur(); //Destructor

//Functies nog toevoegen
private:
unsigned int Button1; 
unsigned int Button2;
//Variabelen nog toevoegen
};

