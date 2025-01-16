#pragma once

#include "Client.h"
#include <iostream>
#include <string.h> 

class Mary : public Client

{

public:

    Mary (int, int);

    ~Mary();

    void SetHulpStatus(int Waarde);

    void SetDeurStatus(int Waarde);
    
    int GetDeurStatus();

    int GetHulpStatus() ;

    

 

private:

    unsigned int StatusHulp;
    unsigned int StatusDeur;

};



