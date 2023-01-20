#include "server.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <chrono>
#include <vector>
#include <array>
#include <thread>
#include <string>
#include <exception>
#include <fstream>

Server::Server(const unsigned long port, const std::string &addr) : mPort(port), mIpAddr(addr)
{
    mDebugFile.open("server_d.log", std::ios::app );

    if ((!openConnection()) || (!setupSocket()))
        return;

    std::thread th(&Server::run, this);
    th.join();
}

Server::~Server()
{
    closeConnection();
    mDebugFile.close();
}

void dummy()
{
    /*std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10000);

    mCode = distrib(gen);
    printf("Your game code is %ld", mCode);*/
}

bool Server::setupSocket()
{
    mAddrStruct.sin_family = AF_INET;
    mAddrStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    mAddrStruct.sin_port = htons((uint16_t)mPort);

    mSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(mSock == -1)
    {
        mDebugFile << "socket failed";
        return false;
    }

    const int optval = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    return true;
}

bool Server::openConnection()
{
    if(bind(mSock, (sockaddr*) &mAddrStruct, sizeof(mAddrStruct)))
    {
        mDebugFile << "bind failed";
        return false;
    }

    const int connectionsNum = 1;
    if(listen(mSock, connectionsNum))
    {
        mDebugFile << "listen failed";
        return false;
    }

    pollfd poll;
    poll.fd = mSock;
    poll.events = POLLIN;

    mPolls.push_back(poll);

    return true;
}

void Server::closeConnection()
{
    close(mSock);

    for (auto &client : mPolls)
    {
        shutdown(client.fd, SHUT_RDWR);
        close(client.fd);
    }
}

bool Server::acceptClient()
{
    int clientFd = accept(mSock, nullptr, nullptr);
    if(clientFd == -1)
    {
        mDebugFile << "accept failed";
        return false;
    }

    pollfd poll;
    poll.fd = clientFd;
    poll.events = POLLIN;

    mPolls.push_back(poll);

    return true;
}

void Server::handleResponse(const sock fd)
{
    //read(fd, );
}

void Server::run()
{
    while(true)
    {
        int res = poll(mPolls.data(), mPolls.size(), -1);

        for (auto &client : mPolls)
        {
            if (client.revents & POLLIN)
            {
                if (client.fd == mSock)
                    bool x = acceptClient();
                else
                    handleResponse(client.fd);
            }
        }
       
    }
}