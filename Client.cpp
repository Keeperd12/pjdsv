#include "Client.h"

Client::Client(int getal) : nummer(getal){}
Client::~Client(){}
int Client::GeefNummer()const{
    return nummer;
}