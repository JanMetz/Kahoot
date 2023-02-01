#include "server.hpp"
#include "game.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

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
#include <algorithm>

Server::Server(const long port) : mPort(port), mPolls({})
{
    //cleaning up log file
    mDebugFile.open("server_d.log");
    mDebugFile.close(); 
}

Server::~Server()
{
    closeConnection();
    for (auto &game : mCreatedGames)
        if (game)
            delete game;
}

bool Server::isUp()
{
	return mSock != -1 && mPolls.size() >= 1;
}

bool Server::establishConnection()
{
    log("opening server");
    return setupSocket() && openConnection();
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
    const int bufforSize = 1024;
    //setsockopt(mSock, SOL_SOCKET, SO_RCVBUF, &bufforSize, sizeof(bufforSize));

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
    auto message = receiveMessage(fd, 1);
    
    if ((message.size() > 1) && (message[0] == "joinGame") && (mGames.find(std::stoi(message[1])) != mGames.end()))
    {
        sendMessage(fd, std::string("gamePort:") + std::to_string(mGames[std::stoi(message[1])]));
        log("Join request received");
    }

    if ((message.size() >= 1) && (message[0] == "createGame"))
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
                    log(std::string("Unable to set the game up"));
					continue;
				}
				
                std::thread th(&Server::run, game);
                th.detach();

                mGames[code] = port;

                sendMessage(fd,  std::to_string(code) + std::string(":"));
                sendMessage(fd, std::to_string(port) + std::string(":"));

                removeClient(fd);

                success = true;

                log("New game created");
                mCreatedGames.push_back(game);
            }
            catch (...)
            {
                delete game;
                log(std::string("Cannot assign port ") + std::to_string(port));
            }
        }
    }
}

void Server::removeClient(const int fd)
{
    shutdown(fd, SHUT_RD);
    close(fd);

    auto it = std::find_if(mPolls.begin(), mPolls.end(), [&](const pollfd &poll){return poll.fd == fd;});
    if (it != mPolls.end())
        mPolls.erase(it);
    else
        log("Error while removing client from polling list");
}

void Server::sendMessage(const int fd, const std::string& msgBody)
{
    char* char_array = new char[msgBody.length() + 1];
    strcpy(char_array, msgBody.data());
    if (write(fd, char_array, strlen(char_array)) == -1)
        log("Error while sending data.");
}

std::vector<std::string> Server::receiveMessage(const int fd, const int minSize)
{
    char answer[1024];
    std::memset(answer, '\0', sizeof(answer));
    int len = 1024;
    if ((len = read(fd, answer, len)) == -1)
        log("Error while receiving message");
	
    auto s = std::string(answer, len);

    auto tokenize = [&](std::vector<std::string>& vec, const std::string& delimiter) {
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos)
        {
            token = s.substr(0, pos);
            vec.push_back(token);
            s.erase(0, pos + delimiter.length());
        }

        if (s.length() > 0)
            vec.push_back(s);
    };
    
    std::vector<std::string> ret;
    tokenize(ret, ":");

    if (ret.size() < minSize)
    {
        std::string concat = "";
        for (auto &part : ret)
            concat += part;

        log(std::string("Received message: ") + concat + std::string(" has invalid size"));
    }

    return ret;
}

void Server::run()
{
    while(true)
    {
        if (poll(mPolls.data(), mPolls.size(), -1) == -1)
        {
			log("Poll failed");
            continue;
        }

        for (auto &client : mPolls)
        {
            if (client.revents & POLLIN)
            {
                if (client.fd == mSock)
                    bool x = acceptClient();
                else
                    handleResponse(client.fd);
            }

            if (client.revents & POLLHUP)
                removeClient(client.fd);
        }
    }
}

int Server::generateCode() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1000, 9999);

    int code = distrib(gen);
    while (mGames.find(code) != mGames.end())
        code = distrib(gen);

    return code;
}

void Server::log(const std::string &msg)
{
    auto timepoint = std::chrono::system_clock::now();
    time_t now = std::chrono::system_clock::to_time_t(timepoint);

    mDebugFile.open("server_d.log", std::ios::app);
    mDebugFile << ctime(&now) << '\t' << msg << '\t' << std::to_string(mPort) << std::endl;
    mDebugFile.close();
}
