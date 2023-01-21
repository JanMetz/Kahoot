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

class Game : public Client
{
public:
    Game(const long port, const std::string& addr);
    void runTheGame();

private:
    void addQuestion(const Question &q);
    void waitForAnswers(const Question& question) const;
    double calculatePoints(const Message &msg, const Question& question) const;
    void addPlayer(const std::string& nick);

    void sendStartSignal();
    void sendEndSignal();
    void blockIncomingPlayers();
    void broadcastNewQuestion(const Question &question);
    void broadcastPunctation();

    std::chrono::time_point mBroadcastTimepoint;
    int mTimePerQuestion;
    int mCode;
    std::vector<Questions> mQuestions;
    std::vector<std::map<std::string, double>> mPunctation;
};

#endif