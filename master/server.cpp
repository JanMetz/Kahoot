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

void Server::handleResponse(const int& fd)
{
    auto message = receiveMessage(fd);

    /*size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) 
    {
        token = s.substr(0, pos);
        std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
    }
    if (message)*/

    //createGame(fd);
    //handleAnswer(fd);
}

void Server::sendMessage(const std::string& msgBody, const int fd)
{
    std::string message = ;
    send(fd, messageBody, sizeof(messageBody), 0);
}

std::string Server::receiveMessage(const int fd)
{
    char answer[200];
    int len;
    recv(fd, answer, len, 0);

    return std::string(answer);
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

int Server::generateCode()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10000);

    int code = distrib(gen);
    while (std::find(mCodes.begin(), mCodes.end(), code) != mCodes.end())
        code = distrib(gen);

    mCodes.push_back(code);
    return code;
}