#pragma once
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "server.hpp"

#include <map>
#include <string>
#include <vector>
#include <chrono>

class Player;
class Questions;

class Message
{
public:
    Server::Actions mAction;
    std::vector<std::map<std::string, double>> mPunctation;
    int mGameCode;
    Questions* mQuestion;
    Client* mSender;
    std::chrono::time_point& mTimestamp;
    std::string mAnswerBody;
};

#endif