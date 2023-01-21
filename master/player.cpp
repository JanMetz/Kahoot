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

void Player::run()
{
    while (true)
    {
        auto msg = receiveMessage();
        switch (msg.mAction)
        {
        case Server::BROADCAST_QUESTION: giveAnswer(msg); break;
        case Server::JOIN_GAME: mJoinedGameCode = msg.mGameCode; break;
        case Server::BROADCAST_PUNCTATION: displayPunctation(msg); break;
        case Server::CREATE_GAME: createGame(); break;
        default: break;
        }
       
        
    }
}

void Player::giveAnswer(const Message &msg)
{
    //display the question
    //get the answer
    Message answer;
    answer.mAction = Server::QUESTION_ANSWER;
    answer.mSender = this;
    answer.mTimestamp = std::chrono::high_resolution_clock::now();
    answer.mAnswerBody = "";
    answer.mGameCode = mJoinedGameCode;
    sendMessage(answer);
}

void Player::displayPunctation(const Message &msg)
{

}

void Player::createGame()
{
    Message msg;
    msg.mAction = CREATE_GAME;
    msg.mSender = this;
    sendMessage(msg);

    auto msg = receiveMessage();
    switch (msg.mAction)
    {
    case Server::BROADCAST_QUESTION: giveAnswer(msg); break;
    case Server::JOIN_GAME: mJoinedGameCode = msg.mGameCode; break;
    case Server::BROADCAST_PUNCTATION: displayPunctation(msg); break;
    case Server::CREATE_QUESTION: displayPunctation(msg); break;
    case Server::CREATE_ANSWER: displayPunctation(msg); break;
    default: break;
    }
}
