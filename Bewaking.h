#pragma once

#include "Client.h"
#include <iostream>
#include <string.h> 
#include <map>

class Bewaking : public Client

{

public:

	Bewaking (int , int , Server*);
	~Bewaking();
	void Update(char *message) override;
	virtual char* GeefData() override;
	void DeurOpen(const std::map<int, Client*>& MapTypeClients);
    void DeurDicht(const std::map<int, Client*>& MapTypeClients);
	
private:

};