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
#include <fstream>

class Server
{
public:
    enum Actions
    {
        START_GAME = 1,
        END_GAME = 2,
        CLOSE_TRAFFIC = 3,
        BROADCAST_QUESTION = 4,
        BROADCAST_PUNCTATION = 5,
        HANDLE_ANSWER = 6,
        GET_GAME_CODE = 7,
        JOIN_GAME = 8,
        LEAVE_GAME = 9,
        CREATE_GAME = 10,
        SUCCESS = 11,
        FAIL = 12
    };

    Server(const long port, const std::string &addr);
    ~Server();

    bool setupSocket();
    void run();
    bool openConnection();
    void closeConnection();
    void handleResponse(const sock fd);
    void sendMessage();

    bool acceptClient();

private:
    socket mSock;
    unsigned long mPort;
    std::string mIpAddr;
    sockaddr_in mAddrStruct;
    std::vector<pollfd> mPolls;

    Games mGames;

    std::ofstream mDebugFile;
};

#endif