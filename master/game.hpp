#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include "questions.hpp"
#include "server.hpp"
#include <poll.h>

#include <vector>
#include <map>
#include <string>
#include <chrono>

class Game : public Server
{
public:
    Game(const long port);
    void runTheGame();

private:
    void extractAnswer(const std::vector<std::string>& msg);
    double calculatePoints(const std::vector<std::string>& msg) const;

    bool addPlayer(const int clientFd);
    void removePlayer(const int fd, const std::string& nick);

    void setupGame();
    Questions createQuestion(const int questionNum);
    void broadcastPunctation();
   
    void handleResponse(const int& fd) override;
    void broadcastMessage(const std::string& msgBody);
    std::vector<std::string> receiveMessage_correctSizeOnly(const int hostFd, const int size);
    bool acceptClient() override;
    void waitForAnswer(const int clientFd);
    
    std::chrono::time_point<std::chrono::high_resolution_clock> mBroadcastTimepoint;
    int mTimePerQuestion;

    std::vector<Questions> mQuestions;
    std::map<std::string, double> mPunctation;

    bool mTrafficClosed;
    bool mGotAllAnswers;
    bool mRun;
    int mAnswersNum;
    std::string mCurrentCorrectAnswer;
};

#endif
