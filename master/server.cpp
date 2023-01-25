#include "server.hpp"
#include "game.hpp"

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

Server::Server(const long port) : mPort(port), mPolls({})
{
    log("opening server");

    if ((!setupSocket()) || (!openConnection()))
        return;
}

Server::~Server()
{
    closeConnection();
}

bool Server::isUp()
{
	return mSock != -1 && mPolls.size() >= 1;
}

bool Server::setupSocket()
{
    mAddrStruct.sin_family = AF_INET;
    mAddrStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    mAddrStruct.sin_port = htons((uint16_t)mPort);

    mSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(mSock == -1)
    {
        log("Socket failed");
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
        log("bind failed");
        return false;
    }

    const int connectionsNum = 5;
    if(listen(mSock, connectionsNum))
    {
        log("listen failed");
        return false;
    }

    pollfd poll;
    poll.fd = mSock;
    poll.events = POLLIN;

    mPolls.push_back(poll);

     log("Connection opened");

    return true;
}

void Server::closeConnection()
{
	log("closing server");
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
        log("accept failed");
        return false;
    }

    pollfd poll;
    poll.fd = clientFd;
    poll.events = POLLIN;

    mPolls.push_back(poll);

     log("New client accepted");

    return true;
}

void Server::handleResponse(const int& fd)
{
    auto message = receiveMessage(fd);
    const int msgCode = std::stoi(message[1]);

    if ((message[0] == "joinGame") && (mGames.find(msgCode) != mGames.end()))
    {
        sendMessage(std::string("gamePort:") + std::to_string(mGames[msgCode]), fd);
         log("Join request received");
    }

    if (message[0] == "createGame")
    {
        bool success = false;
        int port = mPort;

        Server* game;
        while (!success)
        {
            port++;
            int code = generateCode();

            try
            {
                game = new Game(port);
                if (!game->isUp())
                {
					delete game;
					continue;
				}
				
                std::thread th(&Server::run, game);
                th.detach();

                mGames[code] = port;

                sendMessage(std::string("gameCode:") + std::to_string(code), fd);
                sendMessage(std::string("gamePort:") + std::to_string(port), fd);
                
                success = true;

                log("New game created");
            }
            catch (...)
            {
                delete game;
                log(std::string("Cannot assign port ") + std::to_string(port));
            }
        }

        delete game;
    }
}

void Server::sendMessage(const std::string& msgBody, const int fd)
{
    send(fd, msgBody.data(), sizeof(msgBody), 0);
}

void Server::broadcastMessage(const std::string& msg)
{
    for (const auto& client : mPolls)
    {
        sendMessage(msg, client.fd);
    }
}

std::vector<std::string> Server::receiveMessage(const int fd, const int minSize)
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

    if (ret.size() < minSize)
    {
        log("Received message has invalid size");
        return {"1","2","3","4","5","6","7","8"}; // so it will not crash
    }

    return ret;
}

void Server::run()
{
    while(true)
    {
        int res = poll(mPolls.data(), mPolls.size(), -1);
        if (res == -1)
			log("Poll failed\n");

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

void Server::log(const std::string &msg)
{
    mDebugFile.open("server_d.log", std::ios::app);
    mDebugFile << msg << std::endl;
    mDebugFile.close();
}