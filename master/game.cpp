#include "game.hpp"

#include <vector>
#include <chrono>
#include <string>
#include <thread>
#include <array>

Game::Game(Players host)
{
    mHost = host;
    setUp();
    auto th = std::thread(&Game::run, this);
    th.join();
}

void Game::setUp()
{
    const int questionsNum = std::stoi(readIni("config.ini", "questions_per_game"));
    const int timePerQuestion = std::stoi(readIni("config.ini", "seconds_per_question"));

    for (int i = 0; i < questionsNum; ++i)
    {
        std::string question;
        printf("Question #%d:\n", i+1);
        scanf("%s100", question);

        std::array<4, std::string> answers;
        for (int j = 0; j < 4; ++j)
        {
            printf("Question #%d, answer #%d:\t", i + 1, j+1);
            scanf("%s100", &answers[j]);
        }

        printf("Which of the provided answers is correct?\t");
        int num = 0;
        scanf("%d", &num);

        Question q(question, answers);
        q.setCorrectAnswerIndex(num);

        mQuestions.push_back(q);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10000);

    mCode = distrib(gen);
    printf("Your game code is %ld", mCode);
}

void Game::run()
{
    while (true)
    {
        ;
    }
}

int Game::getGameCode() const
{
    return mCode;
}

std::vector<Players> Game::getRanking() const
{
    return std::vector<Players>();
}

double Game::calculatePoints(const std::chrono::time_point &timestamp, Player &player)
{
    return 0.0;
}

void Game::addPlayer(Players player)
{
    mPlayers.push_back(player);
}
