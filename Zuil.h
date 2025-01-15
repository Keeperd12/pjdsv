#pragma once

#include <iostream>     // for cout/cerr
#include <arpa/inet.h>  // for ip inet_pton()
#include <netinet/in.h> // for address
#include <sys/select.h> // for io multiplexing (select)
#include <sys/socket.h> // for socket
#include <unistd.h>     // for close()
#include <vector>       // for storing client
#include <map>          //for storing clientfd with pointer object
#include <string.h> 

#include "Client.h"

class Zuil : public Client
{
public:
    Zuil (int, int);
	~Zuil();
	void SetWaarde(int Waarde);
	int GetValueButton() const;
	int GetValueBrandmelder() const;

private:
    unsigned int Button;
	unsigned int Brandmelder;

};


