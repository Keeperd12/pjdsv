#include "Bewaking.h"
#include "Server.h"

Bewaking::Bewaking(int fd, int type, Server *s) : Client(fd, type, s)
{
    std::cout << "Object gemaakt" << std::endl;
}

Bewaking::~Bewaking() {}

void Bewaking::Update(char *message)
{
    std::cout << "client stuurt message" << std::endl;
}

char *Bewaking::GeefData()
{
    std::cout << "GeefData" << std::endl;
    return nullptr;
}

void Bewaking::DeurOpen(const std::map<int, Client *> &MapTypeClients)
{
    std::cout << "Deur open" << std::endl;
    for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
    {
        if (it->second->GeefType() == 3)
        {
            std::cout << "Deur gevonden" << std::endl;
            // std::cout << it->second->GeefData() << std::endl;
            it->second->Update("1");
        }
    }
}

void Bewaking::DeurDicht(const std::map<int, Client *> &MapTypeClients)
{
    std::cout << "Deur dicht" << std::endl;
    for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
    {
        if (it->second->GeefType() == 3)
        {
            std::cout << "Deur gevonden" << std::endl;
            // std::cout << it->second->GeefData() << std::endl;
            it->second->Update("2");
        }
    }
}

