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
    std::vector<int> getGamesCodes() const;

private:
    std::vector<int> mGameCodes;
    std::vector<Game*> mGames;

};

#endif
