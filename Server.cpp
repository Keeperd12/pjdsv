#include "Server.h"

Server::Server(int poortNmr, char *Ip, int Backlog) : poort(poortNmr), ip(Ip), backlog(Backlog), opt(1)
{
    std::cout << "Object gemaakt" << std::endl;
    ServerSetup();
    ServerLoop();
}

Server::~Server()
{
    close(masterSocket);
}

int Server::stuurAck(int fd)
{
    if (send(fd, ackMessage, strlen(ackMessage), MSG_NOSIGNAL) != strlen(ackMessage))
    {
        std::cout << "verbinding verbroken" << std::endl;
        // zoek de bijhorende pointer naar het object en verwijder
        // nog doen ^
        std::cout << "Object vewijderd" << std::endl;
        // verwijder de client uit de map
        MapTypeClients.erase(fd);
        std::cout << "fd uit map verwijderd" << std::endl;
    }
    // verwijder socketverbinding wanneer niet meer verbonden is

    return 0; // return onsuccesvol
}
int Server::leesType(int fd)
{
    char messages[1024];
    // lees het bericht uit

    valread = read(fd, messages, 1024);
    while (valread == 0)
    {
        valread = read(fd, messages, 1024);
    } // doe niks en wacht dus tot er iets wordt gestuurd
    messages[valread] = '\0';
    if (strcmp(messages, "Muur") == 0)
    {
        // het is een muur
        /*std::cout << "De client is een muur" << std::endl;
        // maak een muur aan
        std::pair<int, Client *> TempToevoegenClient;
        TempToevoegenClient.first = fd;
        TempToevoegenClient.second = new Muur(fd, 1); // object aangemaakt van de muur
        // voeg toe aan map
        MapTypeClients.insert(TempToevoegenClient);*/
        return 1;
    }
    if (strcmp(messages, "Schemerlamp") == 0)
    {
        // het is een Schemerlamp
        std::cout << "De client is een Schemerlamp" << std::endl;
        // Aanmaken van een Schemerlamp object.
        std::pair<int, Client *> TempToevoegenClient; // Het binden van de unieke fd per object aan de pointer naar het client object.
        TempToevoegenClient.first = fd;               // Het eerste lid van de pair gelijkzetten aan de unieke fd.
        // Het daadwerkelijk aanmaken van de Schemerlamp op de heap door het aanroepen van de constructor, wat resulteert in de pointer dat het tweede lid van de pair is.
        TempToevoegenClient.second = new Schemerlamp(fd, 2);
        MapTypeClients.insert(TempToevoegenClient);
        // Nieuwe schemerlamp toevoegen aan de clients map via insert
        return 1;
    }
    if (strcmp(messages, "Deur") == 0)
    {
        // het is een Deur // het is een Deur
        std::cout << "De client is een Deur" << std::endl;
        // Aanmaken van een Schemerlamp object.
        std::pair<int, Client *> TempToevoegenClient; // Het binden van de unieke fd per object aan de pointer naar het client object.
        TempToevoegenClient.first = fd;               // Het eerste lid van de pair gelijkzetten aan de unieke fd.
        // Het daadwerkelijk aanmaken van de Schemerlamp op de heap door het aanroepen van de constructor, wat resulteert in de pointer dat het tweede lid van de pair is.
        TempToevoegenClient.second = new Deur(fd, 3);
        MapTypeClients.insert(TempToevoegenClient);
        // Nieuwe schemerlamp toevoegen aan de clients map via insert
        return 1;
    }
    if (strcmp(messages, "Zuil") == 0)
    {
        // het is een Zuil
        std::cout << "De client is een Zuil" << std::endl;
        // maak een muil aan
        std::pair<int, Client *> TempToevoegenClient;
        TempToevoegenClient.first = fd;
        TempToevoegenClient.second = new Zuil(fd, 4); // object aangemaakt van de muur
        // voeg toe aan map
        MapTypeClients.insert(TempToevoegenClient);
        return 1;
    }
    if (strcmp(messages, "m") == 0)
    {
        // het is een Zuil
        std::cout << "De client is mary" << std::endl;
        // maak een muil aan
        std::pair<int, Client *> TempToevoegenClient;
        TempToevoegenClient.first = fd;
        TempToevoegenClient.second = new Mary(fd, 5); // object aangemaakt van de muur
        send(fd, Ackmary, strlen(Ackmary), MSG_NOSIGNAL) != strlen(Ackmary);
        // voeg toe aan map
        MapTypeClients.insert(TempToevoegenClient);
        return 1;
    }
}
int Server::leesAck(int fd)
{
    char messages[1024];
    // lees het bericht uit
    valread = read(fd, messages, 1024);
    while (valread == 0)
    {
        valread = read(fd, messages, 1024);
    } // doe niks en wacht dus tot er iets wordt gestuurd
    messages[valread] = '\0';
    if (strcmp(messages, "ACK") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Server::ServerLoop()
{
    while (true)
    {
        std::cout << "waiting for activity\n"
                  << std::endl;
        // maak de readfds leeg
        FD_ZERO(&readfds);
        // voeg de master socket toe aan de readfds set
        FD_SET(masterSocket, &readfds);
        maxfd = masterSocket;
        // kopieer de clientlist naar de readfds
        // zodat we naar alle client kunnen luisteren
        for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
        {
            sd = it->first;
            FD_SET(sd, &readfds);
            if (sd > maxfd)
            {
                maxfd = sd;
            }
        }
        // check nog eens
        /*if (sd > maxfd)
        {
            maxfd = sd;
        }*/
        // gebruik select om te luisteren naar meedere clients
        activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            std::cerr << "select error\n"
                      << std::endl;
        }
        // wanneer er iets met de master socket gebeurt is er een niew connectie request
        if (FD_ISSET(masterSocket, &readfds))
        {
            clientSocket = accept(masterSocket, (struct sockaddr *)NULL, NULL);
            // error handeling
            if (clientSocket < 0)
            {
                std::cerr << "accept error\n";
            }

            // adding client to list deze willen we weg werken
            clientList.push_back(clientSocket);

            // welkomst bericht dat de client zich moet identificeren
            if (send(clientSocket, welkomMessage, strlen(welkomMessage), 0) != strlen(welkomMessage))
            {
                perror("send");
            }

            // lees het type uit van de socket en maak hiervan een object
            // plaat socketverbinding met pointer naar het object in map
            leesType(clientSocket);
            stuurAck(clientSocket);

            // zoek of nummer van fd in de lijst staat en roep funcie geefNummer aan en print dit dit zou het zelfde moeten zijn als de fd
            auto it = MapTypeClients.find(clientSocket);
            if (it != MapTypeClients.end())
            {
                std::cout << "Dit is het type client: " << it->second->GeefType() << std::endl;
                std::cout << "Dit is de FD van de client " << it->second->GeefFD() << std::endl;
            }

            // hier verder gaan volgende keer -> vervang de list voor een map met een pair van fds voor socketverbinding en de pointer naar het client object
            // zodat er nog maar een lijst is. Let op er is nog niets gedaan met de authenticatie tussen de client en server
            // Dit is los van daarvan
            std::cout << "einde functie" << std::endl;
        }
        // wanneer we iets willen ontvangen van een client dit nog in functie zetten
        char message[1024];
        for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
        {
            sd = it->first;
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
                    // clientList.erase(clientList.begin() + i);
                    MapTypeClients.erase(sd);
                }
                else
                {
                    message[valread] = '\0';
                    auto it = MapTypeClients.find(sd);
                    if (it != MapTypeClients.end())
                    {

                        Client *client = it->second;
                        int type = client->GeefType();
                        if (type == 4) // Zuil
                        {
                            std::cout << " Zuil gekoppeld aan bericht van zuil" << std::endl;

                            stuurAck(client->GeefFD());
                            std::cout << message << "\n";
                            Zuil *zuil = dynamic_cast<Zuil *>(client);

                            if (!zuil)
                            {
                                std::cerr << "Dynamic cast naar Zuil mislukt. Controleer of client een Zuil-object is." << std::endl;
                                return;
                            }
                            else
                            {
                                VerwerkDataZuil(client, message);
                                std::cout << " Waarde Button " << zuil->GetValueButton() << std::endl;
                                std::cout << " Waarde Brandmelder " << zuil->GetValueBrandmelder() << std::endl;
                            }
                        }
                        if (type == 5) // Mary
                        {
                            std::cout << "  gekoppeld aan bericht van Mary" << std::endl;

                            stuurAck(client->GeefFD());
                            std::cout << message << "\n";
                            Mary *mary = dynamic_cast<Mary *>(client);

                            if (!mary)
                            {
                                std::cerr << "Dynamic cast naar Mary mislukt. Controleer of client een Zuil-object is." << std::endl;
                                return;
                            }
                            else
                            {
                                VerwerkDataMary(client, message);
                                std::cout << " Waarde Hulp " << mary->GetHulpStatus() << std::endl;
                                std::cout << " Waarde Deur " << mary->GetDeurStatus() << std::endl;
                            }
                        }
                        // if (type == 2) // Zuil
                        //{
                        std::cout << " bericht van schemerlamp" << std::endl;

                        // stuurAck(client->GeefFD());
                        std::cout << message << "\n";
                        /*Zuil* zuil = dynamic_cast<Zuil *>(client);


                            if (!zuil)
                            {
                                std::cerr << "Dynamic cast naar Zuil mislukt. Controleer of client een Zuil-object is." << std::endl;
                                return;
                            }
                        else{
                        VerwerkDataZuil (client, message);
                        std::cout << " Waarde Button " << zuil->GetValueButton() << std::endl;
                        std::cout << " Waarde Brandmelder " << zuil->GetValueBrandmelder() << std::endl;
                        }*/
                    }
                }
            }
        }
        for (auto it = MapTypeClients.begin(); it != MapTypeClients.end(); it++)
        {
            Client *client = it->second;
            sd = it->first;
            int type = client->GeefType();
            if(type == 5){
                Mary *mary = dynamic_cast<Mary *>(client);
                if(mary->GetDeurStatus()== 1 ){
                    const char* message = "1";
                    send(clientSocket, message, strlen(message), 0);
                    mary->SetDeurStatus(0);
                }
            }
        
        }
    }
}

void Server::VerwerkDataMary(Client *client, char *message)
{

    int waarde;
    std::cout << message << "\n";
    Mary *mary = dynamic_cast<Mary *>(client);
    if (strcmp(message, "y")==0)
    {
        waarde = 1;
        if (mary)
    {
        // zuil->SetButton(message);

        mary->SetHulpStatus(waarde);
    }
    }
    if (strcmp(message,  "d")==0)
    {
        waarde = 1;
        if (mary)
    {
        mary->SetDeurStatus(waarde);
    }
    }
    if (strcmp(message,  "x")==0)
    {
        waarde = 0;
        if (mary)
    {
        mary->SetDeurStatus(waarde);
    }
    }
}

void Server::VerwerkDataZuil(Client *client, char *message)
{

    stuurAck(client->GeefFD());
    std::cout << message << "\n";
    Zuil *zuil = dynamic_cast<Zuil *>(client);
    if (zuil)
    {
        // zuil->SetButton(message);
        int buttonValue = atoi(message); // Converteer string naar integer
        zuil->SetWaarde(buttonValue);
    }
}


void Server::VerwerkDataDeur(Client *client, char *message)
{

    stuurAck(client->GeefFD());
    std::cout << message << "\n";
    Deur *deur = dynamic_cast<Deur *>(client);
    if (deur)
    {
        // zuil->SetButton(message);
        int buttonValue = atoi(message); // Converteer string naar integer
        //deur->SetDeurStatus(buttonValue);
    }
}

void Server::ServerSetup()
{
    // creeer de master socket

    if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Socket created\n"
                  << std::endl;
    }
    // zet het server addres
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(poort);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);
    // bind de socket
    if (bind(masterSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", poort);
    // 10 maximun aantal socketverbindingen tergelijkertijd
    if (listen(masterSocket, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "De server is nu aan het luisteren" << std::endl;
    }
}
