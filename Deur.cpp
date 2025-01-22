#include "Deur.h"
#include "Server.h"

#include <bitset>

Deur::Deur(int fd, int Type, Server *s) : Client(fd, Type,s){}
Deur::~Deur(){}

void Deur::Update(char *bericht){
    std::cout<< "De verzonden string lengte is: " << strlen(bericht) << std::endl;
    //handel hier het bericht af
    if(strlen(bericht) == 2){
        std::string KnopDeel1(bericht, 1); //pak het eerste bit voor de knop
        std::string KnopDeel2(bericht+1, 2); //pak het tweede bitje voor de andere knop
        //zet de variabelen van het object naar de ingelezen waardes.
        this->Button1 = std::bitset<1>(KnopDeel1).to_ulong();
        this->Button2 = std::bitset<1>(KnopDeel1).to_ulong();
        MoetIkIetsDoen(bericht);

    }
}

void Deur::MoetIkIetsDoen(char *bericht){
    //wanneer button 1 ingedrukt is 
    if(Button1){
        std::cout<< "Button 1 is ingedrukt!" << std::endl;
        //hier iets uitvoeren
    }
    //wanneer button 2 is ingedrukt
    if(Button2){
        std::cout << "Button 2 is ingedrukt!" << std::endl;
        //hier iets uitvoeren
    }
}