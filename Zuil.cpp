#include "Zuil.h"

Zuil::Zuil(int fd, int type) : Client(fd, type), Button(0), Brandmelder(0)
{

    std::cout << "Object gemaakt" << std::endl;
}

Zuil::~Zuil() {}

void Zuil::SetWaarde(int Waarde)
{
     Brandmelder = (Waarde >> 1) & 0x03FF;
     Button = Waarde & 0x01;
    std::cout << " Waarde Brandmelder " << Brandmelder << std::endl;
    std::cout << " Waarde Button " << Button << std::endl;
    return;
}

int Zuil::GetValueBrandmelder() const
{
    return Brandmelder;
}

int Zuil::GetValueButton() const
{
    return Button;
}
