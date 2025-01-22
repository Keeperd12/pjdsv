#include "Client.h"

Client::Client(int getal, int Type, Server *s) : clientFD(getal), type(Type), server(s){}

Client::~Client(){}

int Client::GeefType()const{
    return type;
}
int Client::GeefFD()const{
    return clientFD;
}
