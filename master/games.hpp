#pragma once
#ifndef GAMES_HPP
#define GAMES_HPP

#include "game.hpp"

#include <vector>

class Games
{
public:
    void addGame(Game *gm);
    void removeDeletedGame();
    Game* getGame(const int code) const;

private:
    std::vector<Game*> mGames;
};

#endif
