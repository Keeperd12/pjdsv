#pragma once



#include <iostream>     // for cout/cerr
#include <arpa/inet.h>  // for ip inet_pton()
#include <netinet/in.h> // for address
#include <sys/select.h> // for io multiplexing (select)
#include <sys/socket.h> // for socket
#include <unistd.h>     // for close()
#include <vector>       // for storing client
#include <map>          //for storing clientfd with pointer object
#include <string.h> 

#include "Muur.h"
#include "Deur.h"
#include "Schemerlamp.h"
#include "Mary.h"
#include "Zuil.h" //include de muur
#include "Bewaking.h"

class Server
{
public:
    Server(int, char*, int);
    void ServerSetup();
    void ServerLoop();
    void stuurBericht(int fd, char *);
    int leesAck(int fd);
    int stuurAck(int fd);
    int leesType(int fd);
    void VerwerkDataMary(Client *client, char *message);
    void VerwerkDataBewaking(Client *client, char *message);
    std::map<int, Client*>& GeefPointerMap(); //fix voor het versturen van schemerlamp naar mary
    ~Server();

private:
    
    
    //void VerwerkDataZuil(Client *client, char *message);
    const int poort;
    const char *ip;
    const int backlog;
    int masterSocket, clientSocket, opt, activity, maxfd;
    int sd =0;
    fd_set readfds;
    size_t valread;
    std::map<int, Client*> MapTypeClients; //lijst voor linker fd aan pointer van een client
    struct sockaddr_in serverAddr;
    char *welkomMessage = "Identificeer jezelf!";
    char *ackMessage = "ACK";
    char *Menumary = "\nDeur open = y, hulp = h, deur dicht = x, muur venster openen = z, muur venster sluiten =y, informatie muur opvragen = w";
    char *welcomeMary = "\nWelkom mary";
    char *welcomeBewaking = "\nWelkom Bewaking";
    char *MenuBewaking = "\nDeur open = y, deur dicht = x";
    
};
