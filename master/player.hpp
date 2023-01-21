#pragma once
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player : public Client
{
public:
    Player(const long port, const std::string& addr);

    void setNick(const std::string &nick);
    void getNick() const;
    void requestToJoinAGame(const int code);
    void createGame();
    void leaveGame();

    void run() override;

private:
    std::string mNick;
    int mJoinedGameCode;
    bool mIfHost;
};

#endif