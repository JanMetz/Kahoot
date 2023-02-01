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
    
    virtual void run();
    bool establishConnection();
    void log(const std::string &msg);

protected:
    bool setupSocket();
    bool openConnection();
    void closeConnection();
    
    virtual void handleResponse(const int& fd);
    virtual bool acceptClient();
    virtual void removeClient(const int fd);

    void sendMessage(const int fd, const std::string& msgBody);
    std::vector<std::string> receiveMessage(const int fd, const int minSize = 1);

    int generateCode() const;

    int mSock;
    unsigned long mPort;
    sockaddr_in mAddrStruct;
    std::vector<struct pollfd> mPolls;

    std::map<int, int> mGames; //mGames[code] = port;

    std::ofstream mDebugFile;

private:
    std::vector<Server*> mCreatedGames;
};

#endif
