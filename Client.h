#pragma once

class Server; //forward declaration

class Client
{
public:
    Client(int, int, Server*);
    virtual int GeefType() const;
    virtual int GeefFD() const;
    virtual void Update(char*)=0;
    virtual char* GeefData()=0;
    virtual ~Client();

private:
    int clientFD;
    int type;

protected:
    Server* server;
};