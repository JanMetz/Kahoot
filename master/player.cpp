#include "player.hpp"
#include "message.hpp"

#include <string>

Player::Player(const long port, const std::string& addr) : Client(port, addr)
{
    auto th = std::thread(&Player::run, this);
    th.join()
}

void Player::setNick(const std::string &nick)
{
    mNick = nick;
}

void Player::getNick() const
{
    return mNick;
}

void Player::requestToJoinAGame(const int code)
{
    Message msg;
    msg.mAction = JOIN_GAME;
    msg.mGameCode = code;
    msg.mSender = this;
    sendMessage(msg);
}

void Player::createGame()
{
    Message msg;
    msg.mAction = CREATE_GAME;
    msg.mSender = this;
    sendMessage(msg);
}

void Player::leaveGame()
{
    mJoinedGame = nullptr;
}

void Player::run()
{
    while (true)
    {
        ;
    }
}
