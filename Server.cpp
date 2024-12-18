#include "Server.h"

Server::Server(int poortNmr, char *Ip, int Backlog) : poort(poortNmr), ip(Ip), backlog(Backlog), opt(1)
{
    std::cout<< "Object gemaakt" << std::endl;
    ServerSetup();
    ServerLoop();
}

Server::~Server()
{
}
void Server::ServerLoop(){
    while(true){
        std::cout << "waiting for activity\n" << std::endl;
        //maak de readfds leeg
        FD_ZERO(&readfds);
        //voeg de master socket toe aan de readfds set
        FD_SET(masterSocket, &readfds);
        maxfd = masterSocket;
        //kopieer de clientlist naar de readfds
        //zodat we naar alle client kunnen luisteren
        for(auto sd : clientList){
            FD_SET(sd, &readfds);
            if(sd > maxfd){
                maxfd = sd;
            }
        }
        //check nog eens
        if (sd > maxfd)
        {
            maxfd = sd;
        }
        //gebruik select om te luisteren naar meedere clients
        activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            std::cerr << "select error\n"<< std::endl;
        }
        //wanneer er iets met de master socket gebeurt is er een niew connectie request
        if (FD_ISSET(masterSocket, &readfds))
        {
            clientSocket = accept(masterSocket, (struct sockaddr *)NULL, NULL);
            if (clientSocket < 0)
            {
                std::cerr << "accept error\n";
            }
            // adding client to list
            clientList.push_back(clientSocket);
            std::cout << "new client connected\n";
            std::cout << "new connection, socket fd is " << clientSocket << ", ip is: "
                      << inet_ntoa(serverAddr.sin_addr) << ", port: " << ntohs(serverAddr.sin_port) << "\n";
            //welkom de client
            if( send(clientSocket, welkomMessage, strlen(welkomMessage), 0) != strlen(welkomMessage) ) 
            {
                perror("send");
            }
              
            puts("Welcome message sent successfully");
        }
        //wanneer we iets willen ontvangen van een client dit nog in functie zetten
        char message[1024];
        for (int i = 0; i < clientList.size(); ++i)
        {
            sd = clientList[i];
            if (FD_ISSET(sd, &readfds))
            {
                valread = read(sd, message, 1024);
                // check if client disconnected
                if (valread == 0)
                {
                    std::cout << "client disconnected\n";

                    getpeername(sd, (struct sockaddr *)&serverAddr, (socklen_t *)&serverAddr);
                    // getpeername name return the address of the client (sd)

                    std::cout << "host disconnected, ip: " << inet_ntoa(serverAddr.sin_addr) << ", port: " << ntohs(serverAddr.sin_port) << "\n";
                    close(sd);
                    /* remove the client from the list */
                    clientList.erase(clientList.begin() + i);
                }
                else
                {
                    message[valread] = '\0';
                    std::cout << "message from client: "<< sd << " lengte valread van buffer is: " << valread << "  "<< message << "\n";
                    if( send(sd, message, strlen(message), 0) != strlen(message) )
                    {
                        perror("send");
                        }
                    puts("bericht terug ge echoed");
                    /*
                     * handle the message in new thread
                     * so that we can listen to other client
                     * in the main thread
                     * std::thread t1(handleMessage, client, message);
                     * // detach the thread so that it can run independently
                     * t1.detach();
                     */
                }
            }
        }
    }
}
void Server::ServerSetup(){
    //creeer de master socket
    if((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout<<"Socket created\n" << std::endl;
    }
    //zet het server addres
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(poort);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);
    //bind de socket 
    if (bind(masterSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))<0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", poort);
    //10 maximun aantal socketverbindingen tergelijkertijd
    if (listen(masterSocket, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout<< "De server is nu aan het luisteren" << std::endl;
    }
}
