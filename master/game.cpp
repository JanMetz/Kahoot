#include "game.hpp"

#include <vector>
#include <chrono>
#include <string>

Game::Game(Players host)
{

}

void Game::setUp()
{

}

int Game::getGameCode() const
{
    return 0;
}

std::vector<Players> Game::getRanking() const
{
    return std::vector<Players>();
}

double Game::calculatePoints(const std::chrono::time_point &timestamp, const std::string &playersNick)
{
    return 0.0;
}
