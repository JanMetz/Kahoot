#include "game.hpp"

#include <vector>
#include <map>
#include <array>
#include <poll.h>

#include <chrono>
#include <string>
#include <thread>
#include <algorithm>

Game::Game(const long port) : Server(port), mRun(true), mTrafficClosed(false)
{
    bool unused = establishConnection();
}

void Game::runTheGame()
{
    broadcastMessage(std::string("theGameStarts:"));
    log(std::string("Game started"));
    mTrafficClosed = true;

    for (const auto question : mQuestions)
    {
        mAnswersNum = 0;
        mGotAllAnswers = false;
        mCurrentCorrectAnswer = "";

        broadcastMessage(std::string("question:") + question.getQuestionBody());

        std::string answers = "answer:";
        for (auto& answer : question.getAnswers())
            answers += answer + ":";

        broadcastMessage(answers);

        mCurrentCorrectAnswer = question.getCorrectAnswer();
        mBroadcastTimepoint = std::chrono::high_resolution_clock::now();
        
        while (!mGotAllAnswers)
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

        broadcastPunctation();  
    }

    broadcastMessage(std::string("theGameEnds:"));
    log(std::string("Game ended"));
    broadcastPunctation();
    mRun = false;
}

void Game::broadcastPunctation()
{
    std::string msg = "punctation:";
    for (auto& punct : mPunctation)
    {
        msg = msg + punct.first + ":" + std::to_string(punct.second) + ":";
    }

    broadcastMessage(msg);
}

void Game::extractAnswer(const std::vector<std::string>& msg)  //format odpowiedzi OdpowiedzTekstem:NickGracza:CzasOddaniaOdpowiedzi:
{
    using namespace std::chrono;

    const int twoThirds = static_cast<int>(static_cast<double>(2 * mPunctation.size()) / 3);
    int elapsed = high_resolution_clock::now().time_since_epoch().count() - mBroadcastTimepoint.time_since_epoch().count();

    if (elapsed < mTimePerQuestion && mAnswersNum < twoThirds)
    {
        mPunctation[msg[1]] += calculatePoints(msg);
        mAnswersNum++;
    }
    else
        mGotAllAnswers = true;
}

double Game::calculatePoints(const std::vector<std::string> &msg) const
{
    using namespace std::chrono;

    if (mCurrentCorrectAnswer == msg[0])
    {
        long dur = std::stol(msg[2]) - mBroadcastTimepoint.time_since_epoch().count();

        return (dur / mTimePerQuestion) * 1000;
    }

    return 0.0;
}

void Game::handleResponse(const int& fd)
{   
    auto message = receiveMessage(fd, 1);

    if ((message.size() > 0) && ((message[0] == "joinGame") || (message[0] == "createGame")))
    {
        ;//ignore
    }

    if ((message.size() > 2) && (message[0] == "answer"))
    {
        extractAnswer(message);
    }

    if ((message.size() > 0) && (message[0] == "startTheGame") && (fd == mPolls[1].fd))
    {
        std::thread th(&Game::runTheGame, this);
    }

    if ((message.size() > 1) && (message[0] == "leaveTheGame"))
    {
        removePlayer(fd, message[1]);
    }
}

void Game::broadcastMessage(const std::string& msgBody)
{
    for (auto &client : mPolls)
        sendMessage(client.fd, msgBody);
}

bool Game::acceptClient()
{
    if (mTrafficClosed)
        return false;

    int clientFd = accept(mSock, nullptr, nullptr);
    if (clientFd == -1)
    {
        log(std::string("Accept failed"));
        return false;
    }

    pollfd poll;
    poll.fd = clientFd;
    poll.events = POLLIN;

    if (!addPlayer(clientFd))
        return false;

    mPolls.push_back(poll);

    if (mPolls.size() == 2)
        setupGame();

    return true;
}

void Game::waitForAnswer(const int fd)
{
    pollfd client;
    client.events = POLLIN;
    client.fd = fd;

    log("Waiting for message...");
    while (!(client.revents & POLLIN))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        if (poll(&client, 1, -1) == -1)
        {
            log("Error trying to poll");
            continue;
        }
    }
}

std::vector<std::string> Game::receiveMessage_correctSizeOnly(const int hostFd, const int size)
{
    std::vector<std::string> msg;
    do
    {
        waitForAnswer(hostFd);
        msg = receiveMessage(hostFd, size);
    }
    while(msg.size() < size);

    return msg;
}

bool Game::addPlayer(const int clientFd)
{
    auto nickMsg = receiveMessage_correctSizeOnly(clientFd, 1);
    if (mPunctation.find(nickMsg[0]) != mPunctation.end())
    {
        sendMessage(clientFd, std::string("rejected:"));
        log(std::string("Player ") + nickMsg[0] + std::string(" rejected"));
        shutdown(clientFd, SHUT_RDWR);
        close(clientFd);
        return false;
    }
    else
        sendMessage(clientFd, std::string("accepted:"));

    mPunctation[nickMsg[0]] = 0;

    std::string allNicks = "allNicks:";
    for (auto& nick : mPunctation)
    {
        allNicks += nick.first + ":";
    }

    sendMessage(clientFd, allNicks);
    log(std::string("Player ") + nickMsg[0] + std::string(" added"));
    
    return true;
}

void Game::removePlayer(const int fd, const std::string &nick)
{
    removeClient(fd);

    mPunctation.erase(mPunctation.find(nick));
    log(std::string("Player ") + nick + std::string(" removed"));
}

void Game::setupGame()
{
    log("Entering game setup mode");

    const int hostFd = mPolls[1].fd;
    auto setupMsg = receiveMessage_correctSizeOnly(hostFd, 2);

    const int questionsNum = std::stoi(setupMsg[0]);
    mTimePerQuestion = std::stoi(setupMsg[1]);

    for (int i = 0; i < questionsNum; ++i)
        mQuestions.push_back(createQuestion(i));

    log("Leaving game setup mode");
}

Questions Game::createQuestion(const int questionNum)
{
    const int hostFd = mPolls.at(1).fd;
    sendMessage(hostFd, std::string("sendQuestion:"));
    auto questionMsg = receiveMessage_correctSizeOnly(hostFd, 1);
    log(std::string("Received question: ") + questionMsg[0]);

    std::array<std::string, 4> answers;
    for (int j = 0; j < 4; ++j)
    {
        sendMessage(hostFd, std::string("sendAnswer:"));
        auto answerMsg = receiveMessage_correctSizeOnly(hostFd, 1);
        log(std::string("Received answer: ") + answerMsg[0]);

        answers[j] = answerMsg[0];
    }

    sendMessage(hostFd, std::string("provideCorrectMsgIndex"));
    auto indexMsg = receiveMessage_correctSizeOnly(hostFd, 1);
    log(std::string("Received correct answer index: ") + indexMsg[0]);

    int index = std::stoi(indexMsg[0]);

    Questions q(questionMsg[0], answers);
    q.setCorrectAnswerIndex(index);

    return q;
}