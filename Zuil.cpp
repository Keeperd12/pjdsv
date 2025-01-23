#include "Zuil.h"
#include <iostream>
#include "Server.h"
#include <bitset> 
#include <string>

Zuil::Zuil(int fd, int type,Server* s) : Client(fd, type, s), Button(0), Brandmelder(0)
{

    //std::cout << "Object gemaakt" << std::endl;
}

Zuil::~Zuil() {}

void Zuil::Update(char *bericht)
{
    std::cout << "strlen van het bericht: " << strlen(bericht) << std::endl;
    if (strlen(bericht) == 3)
    {
        // Convert the first 10 bits to an integer
        int data = atoi(bericht);
        this->Brandmelder = (data >> 1) & 0x03FF;
        this->Button = data & 0x01;

        std::cout << "Brandmelder: " << this->Brandmelder << std::endl;
        std::cout << "Button: " << this->Button << std::endl;
        moetIkIetsDoen(bericht);
    }
 
}

void Zuil::moetIkIetsDoen(char *bericht)
{
    sleep(1);
    std::string brand;
    std::string button;
    if (this->Brandmelder >= 300){
         brand = "1";
        StatusZoemer = 1;
        std::cout << "er is brand" << std::endl;
    }
    else{ 
        StatusZoemer = 0;
         brand = "0";
    }

    if (this->Button == 1){
         button = "1";
        StatusLed = 1;
        std::cout << "knop is ingedrukt" << std::endl;
    }
    else {
         button = "0";
        StatusLed = 0;
    }
        char temp[4]; // Extra ruimte voor null-terminator
    temp[0] = '0'; 
    temp[1] = brand[0];
    temp[2] = button[0];
    temp[3] = '\0'; // Null-terminator

    std::cout << "dit is de fd "<< GeefFD() << std::endl;
    std::cout << "dit is de verstuurde waarde "<< temp << std::endl;
     server->stuurBericht(GeefFD(), temp);
}

char *Zuil::GeefData()
{
    return nullptr;
}

