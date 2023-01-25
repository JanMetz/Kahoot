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
    Server(const long port);
    ~Server();

	bool isUp();
    bool setupSocket();
    virtual void run();

    bool openConnection();
    void closeConnection();
    
    virtual void handleResponse(const int& fd);
    virtual bool acceptClient();

    void sendMessage(const std::string& msgBody, const int fd);
    void broadcastMessage(const std::string& msgBody);
    std::vector<std::string> receiveMessage(const int fd, const int minSize = 1);

    int generateCode() const;

protected:
    int mSock;
    unsigned long mPort;
    sockaddr_in mAddrStruct;
    std::vector<struct pollfd> mPolls;

    std::map<int, int> mGames; //mGames[code] = port;

    std::ofstream mDebugFile;
};

#endif
