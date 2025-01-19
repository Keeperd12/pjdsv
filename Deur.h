#pragma once //Voorkomen van meervoudige inclusie van de header file

#include "Client.h"

class Deur : public Client //Overerven van de Client klasse
{
public:
    Deur(int, int, Server*); //Constructor
    ~Deur(); //Destructor

//Functies nog toevoegen
private:
unsigned int status; 
//Variabelen nog toevoegen
};

