#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
public:
    Client(const long port, const std::string &addr);
    void run();
    bool openConnection();
    void closeConnection();
    void handleResponse();
    void sendMessage();
};

#endif