#pragma once

#include "Client.h"
#include <iostream>
#include <string.h> 
#include <map>

class Mary : public Client

{

public:

    Mary (int, int, Server*);
    virtual ~Mary();

    //over geerfde functies die overidden evt kunnen worden
    virtual void Update(char *message) override;
    virtual char* GeefData() override;

    //alles met betrekking tot de muur
    virtual void LCDopen(const std::map<int, Client*>& MapTypeClients);
    virtual void LCDsluiten(const std::map<int, Client*>& MapTypeClients);
    virtual void LedHelderheid(const std::map<int, Client*>& MapTypeClients);
    virtual void printStatusMuur(const std::map<int, Client*>& MapTypeClients);

    virtual void MaryThuis();

    //alles met betrekking tot de deur
    void DeurOpen(const std::map<int, Client*>& MapTypeClients);
    void DeurDicht(const std::map<int, Client*>& MapTypeClients);

    void SetHulpStatus(int Waarde){};
    void SetDeurStatus(int Waarde){};

private:
    int isMaryThuis=1; // de variabel die gezet wordt om te kijken of Mary thuis, ze is standaard
};



