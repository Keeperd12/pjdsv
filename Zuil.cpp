               #include "Zuil.h"

 

Zuil::Zuil(int fd, int type): Client(fd, type), Button(0), Brandmelder(0)  {

   

    std::cout<< "Object gemaakt" << std::endl;

   

    }

 

Zuil::~Zuil() {}

 

void Zuil::SetWaarde (int Waarde) {

   unsigned int Brandmelder = (Waarde >> 1) & 0x03FF;

   unsigned int Button = Waarde & 0x01;

   return;

}

 

int Zuil::GetValueBrandmelder() const {

    return Brandmelder;

}

 

int Zuil::GetValueButton() const {

    return Button;

}