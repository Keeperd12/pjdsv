#include "Client.h"

Client::Client(int getal, int Type) : clientFD(getal), type(Type){}

Client::~Client(){}

int Client::GeefType()const{
    return type;
}
int Client::GeefFD()const{
    return clientFD;
}
