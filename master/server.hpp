#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "games.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
#include <string>
#include <thread>
#include <fstream>

class Server
{
public:
    Server(const long port, const std::string &addr);
    ~Server();

    bool setupSocket();
    void run();

    bool openConnection();
    void closeConnection();
    
    virtual void handleResponse(const int& fd);
    bool acceptClient();

    void sendMessage(const std::string& msgBody, const int fd);
    std::string receiveMessage(const int fd);

    static int generateCode();

private:
    socket mSock;
    unsigned long mPort;
    std::string mIpAddr;
    sockaddr_in mAddrStruct;
    std::vector<pollfd> mPolls;
    std::vector<int> mCodes;

    std::map<int, Game> mGames;

    std::ofstream mDebugFile;
};

#endif