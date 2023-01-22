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
    sendMessage(std::string("joinGame:") + std::to_string(code));
}

void Player::run()
{
    while (true)
    {
        auto msg = receiveMessage();
        if (msg[0] == "broadcastQuestion")
        {
            broadcastQuestion();
            giveAnswer();
        }

        if (msg[0] == "joinGame")
        {
            auto newPort = receiveMessage();
            //connect to different game server
        }
        
        if (msg[0] == "broadcastPunctation")
        {
            broadcastPunctation();
        }
    }
}

void Player::giveAnswer(const Message &msg)
{
    //display the question
    //get the answer
    std::string userInput = "";
    std::string timestamp = ""; //std::chrono::high_resolution_clock::now();
    sendMessage(std::string("answerBody:") + userInput + std::string("timestamp:") + timestamp);
    sendMessage(answer);
}

void Player::displayPunctation(const Message &msg)
{

}

int Player::createGame()
{
    sendMessage(std::string("createGame"));
    auto newPortMsg = receiveMessage();
    int port = newPortMsg[1];
    //connect to different game server

    auto numOfQuestions = receiveMessage();

    for (int i = 0; i < numOfQuestions; ++i)
    {
        std::string userInput = "";
        auto sendMessage = receiveMessage();
        sendMessage(std::string("question:") + userInput);

        for (int j = 0; j < 4; ++j)
        {
            auto sendAnswer = receiveMessage();
            sendMessage(std::string("answer:") + userInput);
        }

        auto sendIndex = receiveMessage();
        sendMessage(std::string("correctAnswerIndex:") + userInput);
    }

    auto gameCodeMsg = receiveMessage();

    return gameCodeMsg[1];
}
