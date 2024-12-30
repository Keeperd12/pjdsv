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

#include "Muur.h" //include de muur

class Server
{
public:
    Server(int, char*, int);
    void ServerSetup();
    void ServerLoop();
    ~Server();

private:
    int leesAck(int fd);
    int stuurAck(int fd);
    int leesType(int fd);
    const int poort;
    const char *ip;
    const int backlog;
    int masterSocket, clientSocket, opt, activity, maxfd;
    int sd =0;
    fd_set readfds;
    size_t valread;
    std::vector<int> clientList;
    std::map<int, Client*> MapTypeClients; //lijst voor linker fd aan pointer van een client
    struct sockaddr_in serverAddr;
    char *welkomMessage = "Identificeer jezelf!";
    char *ackMessage = "ACK";
};


