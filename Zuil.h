#pragma once



#include "Client.h"

class Zuil : public Client
{
public:
    Zuil (int, int, Server*);
	~Zuil();
	virtual void Update(char*);
	virtual void MoetIkIetsDoen(char*);
	virtual char* GeefData();

private:
    unsigned int Button;
	unsigned int Brandmelder;
	unsigned int StatusZoemer;
	unsigned int StatusLed;
	unsigned int HulpMary;
	unsigned int ZoemerBlink;

};


