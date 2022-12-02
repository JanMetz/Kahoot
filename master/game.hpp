#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include "players.hpp"
#include "questions.hpp"

#include <vector>
#include <chrono>

class Game
{
public:
    Game(Players host);
    void setUp();
    int getGameCode() const;
    std::vector<Players> getRanking() const;
    double calculatePoints(const std::chrono::time_point &timestamp, const std::string &playersNick);
private:
    std::chrono::time_point mQuestionDisplayedOn;
    int mCode;
    std::vector<Questions> mQuestions;
    std::vector<Players> mPlayers;

    Players mHost;
};

#endif