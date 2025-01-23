#pragma once



#include "Client.h"

class Zuil : public Client
{
public:
    Zuil (int, int, Server*);
	virtual ~Zuil();
	virtual void Update(char*);
	virtual void moetIkIetsDoen(char *); //moet ik iets doen
	virtual char* GeefData();



private:
    int Button;
	int Brandmelder;
	int StatusZoemer;
	int StatusLed;
	

};


