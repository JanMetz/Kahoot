#pragma once
#ifndef AUXILARIES_HPP
#define AUXILARIES_HPP

#include <string>

class Server;

class GamePointer
{
public:
    GamePointer(const int code, const int port, Server* ptr): mCode(code), mPort(port), mPtr(ptr) {};
    int mCode;
    int mPort;

    Server *mPtr;
};

class Player
{
public:
    Player(const int fd, const double score, const std::string &nick) : mFd(fd), mScore(score), mNick(nick) {};

    int mFd;
    double mScore;
    std::string mNick;
};

#endif
