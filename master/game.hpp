#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include "players.hpp"
#include "questions.hpp"

#include <vector>
#include <map>
#include <string>
#include <chrono>

class Message;

class Game : public Server
{
public:
    Game(const long port, const std::string& addr);
    void runTheGame();

private:
    void extractAnswer(const std::vector<std::string>& msg) const
    double calculatePoints(const std::vector<std::string>& msg, const std::string& corrAnswer) const;
    void addPlayer(const std::string& nick);

    void broadcastPunctation();

    Question createQuestion(const int questionNum) const;
    void handleResponse(const int& fd) override;
    bool acceptClient() override;

    std::chrono::time_point mBroadcastTimepoint;
    int mTimePerQuestion;

    std::vector<Questions> mQuestions;
    std::map<std::string, double> mPunctation;
    bool mTrafficClosed;
    bool mGotAllAnswers;
    int mAnswersNum;
    std::string mCurrentCorrectAnswer;
};

#endif