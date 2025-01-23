#include "Zuil.h"
#include "Server.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>

Zuil::Zuil(int fd, int type,Server* s) : Client(fd, type, s), Button(0), Brandmelder(0)
{

    //std::cout << "Object gemaakt" << std::endl;
}

Zuil::~Zuil() {}

void Zuil::SetWaarde(int Waarde)
{
    unsigned int Brandmelder = (Waarde >> 1) & 0x03FF;
    unsigned int Button = Waarde & 0x01;
    //std::cout << " Waarde Brandmelder " << Brandmelder << std::endl;
    //std::cout << " Waarde Button " << Button << std::endl;
    //return;
}
char* Zuil::GeefData()
{
    return nullptr;
}

int Zuil::GetValueBrandmelder() const
{
    return Brandmelder;
}

int Zuil::GetValueButton() const
{
    return Button;
}
void Zuil::Update(char *bericht)
{
    std::cout << strlen(bericht) << std::endl;
    if (strlen(bericht) == 11){
        std::cout << "Het bericht is van de zuil zelf" << std::endl;

         std::string BrandDeel(bericht + 1, 11); //zet het brandmeldt gedeelte in een string
         std::string KnopDeel(bericht,1);

         this->Brandmelder = std::bitset<10>(BrandDeel).to_ulong();
         this->Button = std::bitset<10>(KnopDeel).to_ulong();
        
         MoetIkIetsDoen(bericht);


    }
}
void Zuil::MoetIkIetsDoen(char *bericht){
    //check of er brand is
    if(Brandmelder >= 300){
        std::cout<< "Er is brand!!" << std::endl;
        std::cout<< Brandmelder << std::endl;
        //hier de 
    }
    if(Button){
        std::cout << "De button is ingedrukt!" << std::endl;
        std::cout << Button << std::endl;
    }

}