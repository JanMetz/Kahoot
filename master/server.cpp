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
#include <random>

Server::Server(const long port, const std::string &addr) : mPort(port), mIpAddr(addr)
{
    mDebugFile.open("server_d.log", std::ios::app );

    if ((!openConnection()) || (!setupSocket()))
        return;
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
    setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

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
    const int msgCode = std::stoi(message[1]);

    if ((message[0] == "joinGame") && (mGames.find(msgCode) != mGames.end()))
    {
        sendMessage(std::string("gamePort:") + std::to_string(mGames[msgCode]), fd);
    }

    if (message[0] == "createGame")
    {
        bool success = false;
        int port = mPort;

        while (!success)
        {
            port++;
            int code = generateCode();

            try
            {
                Game game(port, "localhost");
                std::thread th(&Server::run, game);

                mGames[code] = port;

                sendMessage(std::string("gameCode:") + std::to_string(code), fd);
                sendMessage(std::string("gamePort:") + std::to_string(port), fd);
                
                success = true;
            }
            catch (...)
            {
                mDebugFile << "Cannot assign port " << port << std::endl;
            }
        }
    }
}

void Server::sendMessage(const std::string& msgBody, const int fd)
{
    send(fd, msgBody.data(), sizeof(msgBody), 0);
}

void Server::broadcastMessage(const std::string& msg)
{
    for (auto& client : mPolls)
    {
        sendMessage(msg, client.fd);
    }
}

std::vector<std::string> Server::receiveMessage(const int fd)
{
    char answer[200];
    int len;
    recv(fd, answer, len, 0);
    auto s = std::string(answer);

    auto tokenize = [&](std::vector<std::string>& vec, const std::string& delimiter) {
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos)
        {
            token = s.substr(0, pos);
            vec.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
    };
    
    std::vector<std::string> tmp;
    tokenize(tmp, ";");
    std::vector<std::string> ret;
    for (auto& s : tmp)
        tokenize(ret, ":");

    return ret;
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

int Server::generateCode() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10000);

    int code = distrib(gen);
    while (mGames.find(code) != mGames.end())
        code = distrib(gen);

    return code;
}