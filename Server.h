#pragma once

#include <iostream>     // for cout/cerr
#include <arpa/inet.h>  // for ip inet_pton()
#include <netinet/in.h> // for address
#include <sys/select.h> // for io multiplexing (select)
#include <sys/socket.h> // for socket
#include <unistd.h>     // for close()
#include <vector>       // for storing client
#include <string.h> 

class Server
{
public:
    Server(int, char*, int);
    void ServerSetup();
    void ServerLoop();
    ~Server();

private:
    const int poort;
    const char *ip;
    const int backlog;
    int masterSocket, clientSocket, opt, activity, maxfd;
    int sd =0;
    fd_set readfds;
    size_t valread;
    std::vector<int> clientList;
    struct sockaddr_in serverAddr;
    char *welkomMessage = "He je bent verbonden!\r\n";
};


