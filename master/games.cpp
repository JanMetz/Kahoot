#include "games.hpp"
#include "game.hpp"

#include <vector>


void Games::addGame(Game *gm)
{
    mGames.push_back(gm);
}

void Games::removeDeletedGame()
{

}

Game* Games::getGame(const int code) const
{
    for (Game* game : mGames)
    {
        if (game->getGameCode() == code)
            return game;
    }

    return nullptr;
}


