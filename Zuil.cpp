#include "Zuil.h"
#include <iostream>

Zuil::Zuil(int fd, int type,Server* s) : Client(fd, type, s), Button(0), Brandmelder(0)
{

    //std::cout << "Object gemaakt" << std::endl;
}

Zuil::~Zuil() {}

void Zuil::SetWaarde(int Waarde)
{
    Brandmelder = (Waarde >> 1) & 0x03FF;
    Button = Waarde & 0x01;
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
    std::cout << "Het bericht is ontvangen: " << bericht << std::endl;
}
