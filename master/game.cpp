#include "game.hpp"
#include "message.hpp"

#include <vector>
#include <map>
#include <array>

#include <chrono>
#include <string>
#include <thread>

Game::Game(const long port, const std::string& addr) : Client(port, addr)
{
}

void Game::addQuestion(const Question &q)
{
    mQuestions.push_back(q);
}

void Game::runTheGame()
{
    sendStartSignal();

    for (const auto question : mQuestions)
    {
        broadcastNewQuestion(question);
        mBroadcastTimepoint = std::chrono::high_resolution_clock::now();
        waitForAnswers(question);
        broadcastPunctation();
    }

    sendEndSignal();
}

void Game::sendStartSignal()
{
    blockIncomingPlayers();

    Message msg;
    msg.mAction = START_GAME;
    msg.mSender = this;
    sendMessage(msg);
}

void Game::sendEndSignal()
{
    Message msg;
    msg.mAction = END_GAME;
    msg.mSender = this;
    sendMessage(msg);
}

void Game::blockIncomingPlayers()
{
    Message msg;
    msg.mAction = CLOSE_TRAFFIC;
    msg.mSender = this;
    sendMessage(msg);
}

void Game::broadcastNewQuestion(const Question &question)
{
    Message msg;
    msg.mAction = BROADCAST_QUESTION;
    msg.mQuestion = &question;
    msg.mSender = this;
    sendMessage(msg);
}

void Game::broadcastPunctation()
{
    Message msg;
    msg.mAction = BROADCAST_PUNCTATION;
    msg.mPunctation = &mPunctation;
    msg.mSender = this;
    sendMessage(msg);
}

void Game::waitForAnswers(const Question& question) const
{
    using namespace std::chrono;

    const int twoThirds = static_cast<int>(static_cast<double>(2 * mPlayers.size()) / 3);
    int answersNum = 0;

    const auto start = high_resolution_clock::now();
    int elapsed = 0;

    while (elapsed < mTimePerQuestion && answersNum < twoThirds)
    {
        std::chrono::duration<int> dur = high_resolution_clock::now() - start;
        elapsed = dur.count();
        auto msg = receiveMessage();
        if (msg)
        {
            mPunctation[msg.mSender.getNick()] += calculatePoints(msg, question);
            answersNum++;
        }

        std::this_thread::sleep_for(milliseconds(mTimePerQuestion * 1000 / 100));
    }
}

double Game::calculatePoints(const Message &msg, const Question& question) const
{
    Question question;
    if (question.isCorrectAnswer(msg.mAnswerBody))
    {
        std::chrono::duration<int> dur = msg.mTimestamp - mBroadcastTimepoint;

        if (dur < 0)
            return 0.0;

        return ((mTimePerQuestion - dur.count()) / mTimePerQuestion) * 1000;
    }

    return 0.0;
}

void Game::addPlayer(const std::string &nick)
{
    mPunctation[nick] = 0;
}
