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
   server->stuurBericht(GeefFD(), "Mary opent het venster\r\n");
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
   server->stuurBericht(GeefFD(), "Mary sluit het venster\r\n");
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
   server->stuurBericht(GeefFD(), "Mary vraagt data van de muren aan\r\n");
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      //check of het object een muur is en voer de actie uit
      if (it->second->GeefType() == 1)
      {
         std::cout << "Muur gevonden" << std::endl;
         //sla de pointer naar de string van de data op
         char *data = it->second->GeefData();
         //verstuur de data naar Mary
         server->stuurBericht(GeefFD(), data);
         //voorkom memory leak door pointer te deleten geen normale delete want het gaat om een array
         delete[]data;
      }
   }
}

void Mary::LedHelderheid(const std::map<int, Client*>& MapTypeClients)
{
   server->stuurBericht(GeefFD(), "Mary past de LED helderheid aan naar 90\r\n");
   for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
   {
      if (it->second->GeefType() == 1)
      {
         it->second->Update("90");
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
//flip de aanwezigheid van Mary
void Mary::MaryThuis()
{
   if(this->isMaryThuis == 1)
   {
      server->stuurBericht(GeefFD(), "Mary meldt zich af\r\n");
      this->isMaryThuis = 0;
   }
   else
   {
      this->isMaryThuis = 1;
      server->stuurBericht(GeefFD(), "Mary meldt zich aan\r\n");
   }
}