#include "Schemerlamp.h"
#include <iostream>

Schemerlamp::Schemerlamp(int fd, int Type, Server*s) : Client(fd, Type,s){}
Schemerlamp::~Schemerlamp(){}
void Schemerlamp::Update(char* bericht) {
    std::cout << "Het bericht voor de Schemerlamp is succesvol ontvangen: " << bericht << std::endl;
}
char* Schemerlamp::GeefData() {
    return nullptr;
}