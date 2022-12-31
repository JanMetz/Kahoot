#pragma once
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player
{
public:
    void setNick(const std::string &nick);
    void getNick() const;
    bool joinGame(const int code);
    void createGame();
    void leaveGame();

    void run();

protected:
    std::string mNick;

private:
    double mPoints;
    Game *mJoinedGame;
    bool mIfHost;
};

#endif