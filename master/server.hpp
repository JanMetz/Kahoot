#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "auxilaries.hpp"

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

class GamePointer;

class Server
{
public:
    Server(const long port);
    virtual ~Server();

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
    bool receiveMessage(const int fd, const size_t minSize,  std::vector<std::string> &vec);

    int generateCode() const;

    int mSock;
    unsigned long mPort;
    sockaddr_in mAddrStruct;
    std::vector<struct pollfd> mPolls;
    std::ofstream mDebugFile;

private:
    std::vector<GamePointer>::const_iterator findGame(const int code) const;

    std::vector<GamePointer> mCreatedGames;
};

#endif
