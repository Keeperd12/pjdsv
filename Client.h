#pragma once

class Client
{
public:
    Client(int);
    int GeefNummer() const;
    ~Client();

private:
    int nummer;
};


