#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Message;

class Client
{
public:
    Client(const long port, const std::string &addr);
    ~Client();

    virtual void run() = 0;
    bool setupSocket();
    Message receiveMessage();
    void sendMessage(const Message &msg);
};

#endif