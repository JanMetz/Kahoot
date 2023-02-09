#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include "questions.hpp"
#include "server.hpp"
#include "auxilaries.hpp"

#include <poll.h>

#include <vector>
#include <map>
#include <string>
#include <chrono>

class Game : public Server
{
public:
    Game(const long port, const long motherPort);
    ~Game();

    void runTheGame();
    void run() override;

private:
    void extractAnswer(const std::vector<std::string>& msg);

    bool addPlayer(const int clientFd);
    void sendAllNicks();

    void setupGame();
    Questions createQuestion();
    void broadcastPunctation();
   
    void handleResponse(const int& fd) override;
    void broadcastMessage(const std::string& msgBody);
    std::vector<std::string> receiveMessage_correctSizeOnly(const int hostFd, const int size);
    bool acceptClient() override;
    void removeClient(const int fd) override;
    void notifyOwnerAboutClosing();
    
    unsigned long mBroadcastTimepoint;
    int mTimePerQuestion;

    std::vector<Questions> mQuestions;
    std::vector<Player> mPlayers;

    bool mTrafficClosed;
    volatile bool mGotAllAnswers;
    bool mRun;
    int mAnswersNum;
    long mMotherPort;
    std::string mCurrentCorrectAnswer;
};

#endif
