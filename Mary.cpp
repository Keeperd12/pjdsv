#include "Mary.h"
#include "Server.h"

Mary::Mary(int fd, int type, Server *s) : Client(fd, type, s), StatusHulp(0), StatusDeur(0)
{

   std::cout << "Object gemaakt" << std::endl;
}

Mary::~Mary() {}

void Mary::Update(char *message)
{

   std::cout << "message from client: " << std::endl;
}
char* Mary::GeefData()
{
   std::cout << "GeefData" << std::endl;
}

void Mary::SetHulpStatus(int Waarde)
{

   StatusHulp = Waarde;
   std::cout << StatusHulp << std::endl;
   return;
}

void Mary::SetDeurStatus(int Waarde)
{

   StatusDeur = Waarde;
   std::cout << StatusHulp << std::endl;
   return;
}

int Mary::GetHulpStatus()
{

   return StatusHulp;
}

int Mary::GetDeurStatus()
{

   return StatusDeur;
}

void Mary::LCDopen(const std::map<int, Client*>& MapTypeClients)
{
   std::cout << "Deur open" << std::endl;
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      if (it->second->GeefType() == 1)
      {
         std::cout << "Muur gevonden" << std::endl;
         //std::cout << it->second->GeefData() << std::endl;
         //server->stuurBericht(GeefFD(), it->second->GeefData());
         //
         it->second->Update("0");
      }
   }
}
void Mary::LCDsluiten(const std::map<int, Client*>& MapTypeClients)
{
   std::cout << "Deur open" << std::endl;
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      if (it->second->GeefType() == 1)
      {
         std::cout << "Muur gevonden" << std::endl;
         //std::cout << it->second->GeefData() << std::endl;
         //server->stuurBericht(GeefFD(), it->second->GeefData());
         //
         it->second->Update("1");
      }
   }
}
void Mary::printStatusMuur(const std::map<int, Client*>& MapTypeClients)
{
   std::cout << "Deur open" << std::endl;
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      if (it->second->GeefType() == 1)
      {
         std::cout << "Muur gevonden" << std::endl;
         //std::cout << it->second->GeefData() << std::endl;
         server->stuurBericht(GeefFD(), it->second->GeefData());
         //it->second->Update("11111111111111111111");
      }
   }
}

void Mary::LedHelderheid(const std::map<int, Client*>& MapTypeClients)
{
   std::cout << "Deur open" << std::endl;
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      if (it->second->GeefType() == 1)
      {
         std::cout << "Muur gevonden" << std::endl;
         //std::cout << it->second->GeefData() << std::endl;
         //server->stuurBericht(GeefFD(), it->second->GeefData());
         //
         it->second->Update("50");
      }
   }
}