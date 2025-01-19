#include "Deur.h"
#include "Server.h"

Deur::Deur(int fd, int Type, Server *s) : Client(fd, Type,s){}
Deur::~Deur(){}

