#include "player.hpp"

#include <string>

void Player::setNick(const std::string &nick)
{
    mNick = nick;
}

void Player::getNick() const
{
    return mNick;
}

bool Player::joinGame(const int code)
{
    mJoinedGame = Games::getGame(code);
    if (!mJoinedGame)
    {
        printf("No such game");
        return false;
    }
    else
    {
        printf("Connected");
        return true;
    }
}

void Player::createGame()
{
    mJoinedGame = Game(this);
    Games::addGame(mJoinedGame);
}

void Player::leaveGame()
{

}

void Player::run()
{

}
