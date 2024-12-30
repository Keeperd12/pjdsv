#pragma once


class Client
{
public:
    Client(int, int);
    virtual int GeefType() const;
    virtual int GeefFD() const;
    virtual ~Client();

private:
    int clientFD;
    int type;
};


