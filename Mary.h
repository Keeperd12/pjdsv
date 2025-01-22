#pragma once

#include "Client.h"
#include <iostream>
#include <string.h> 
#include <map>

class Mary : public Client

{

public:

    Mary (int, int, Server*);

    ~Mary();

    void Update(char *message) override;
    virtual char* GeefData() override;

    void SetHulpStatus(int Waarde);

    void SetDeurStatus(int Waarde);
    
    int GetDeurStatus();

    int GetHulpStatus() ;

    virtual void LCDopen(const std::map<int, Client*>& MapTypeClients);
    virtual void LCDsluiten(const std::map<int, Client*>& MapTypeClients);
    virtual void LedHelderheid(const std::map<int, Client*>& MapTypeClients);

    void printStatusMuur(const std::map<int, Client*>& MapTypeClients);

    

 

private:

    unsigned int StatusHulp;
    unsigned int StatusDeur;

};



