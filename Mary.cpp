#include "Mary.h"
#include "Server.h"

Mary::Mary(int fd, int type, Server *s) : Client(fd, type, s)
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
void Mary::DeurOpen(const std::map<int, Client*>& MapTypeClients)
{
   std::cout << "Deur open" << std::endl;
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      if (it->second->GeefType() == 3)
      {
         std::cout << "Deur gevonden" << std::endl;
         //std::cout << it->second->GeefData() << std::endl;
         it->second->Update("1");
         //it->second->Update("11111111111111111111");
      }
   }
}
void Mary::DeurDicht(const std::map<int, Client*>& MapTypeClients)
{
   std::cout << "Deur dicht" << std::endl;
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      if (it->second->GeefType() == 3)
      {
         std::cout << "Deur gevonden" << std::endl;
         //std::cout << it->second->GeefData() << std::endl;
         it->second->Update("2");
         //it->second->Update("11111111111111111111");
      }
   }
}
