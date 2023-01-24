#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include <vector>
#include <string>
#include <thread>
#include <map>
#include <fstream>

class Server
{
public:
    Server(const long port, const std::string &addr);
    ~Server();

    bool setupSocket();
    virtual void run();

    bool openConnection();
    void closeConnection();
    
    virtual void handleResponse(const int& fd);
    virtual bool acceptClient();

    void sendMessage(const std::string& msgBody, const int fd);
    void broadcastMessage(const std::string& msgBody);
    std::vector<std::string> receiveMessage(const int fd);

    int generateCode() const;

protected:
    int mSock;
    unsigned long mPort;
    std::string mIpAddr;
    sockaddr_in mAddrStruct;
    std::vector<struct pollfd> mPolls;

    std::map<int, int> mGames; //mGames[code] = port;

    std::ofstream mDebugFile;
};

#endif