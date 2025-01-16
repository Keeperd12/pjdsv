#include "Mary.h"

 

Mary::Mary(int fd, int type): Client(fd, type), StatusHulp(0), StatusDeur(0)  {

   

    std::cout<< "Object gemaakt" << std::endl;

   

    }

 

Mary::~Mary() {}

 

void Mary:: SetHulpStatus(int Waarde){

   StatusHulp = Waarde;
std::cout<< StatusHulp << std::endl;
   return;

}


void Mary:: SetDeurStatus(int Waarde){

   StatusDeur = Waarde;
std::cout<< StatusHulp << std::endl;
   return;

}

 

int Mary:: GetHulpStatus()  {

    return StatusHulp;

}

int Mary:: GetDeurStatus()  {

    return StatusDeur;

}

