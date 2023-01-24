#include "game.hpp"

#include <vector>
#include <map>
#include <array>
#include <poll.h>

#include <chrono>
#include <string>
#include <thread>

Game::Game(const long port, const std::string& addr) : Server(port, addr), mRun(true)
{
    
}

void Game::runTheGame()
{
    broadcastMessage(std::string("theGameStarts"));
    mTrafficClosed = true;

    for (const auto question : mQuestions)
    {
        mAnswersNum = 0;
        mGotAllAnswers = false;
        mCurrentCorrectAnswer = "";

        broadcastMessage(std::string("question:") + question.getQuestionBody());

        std::string answers = "";
        for (auto& answer : question.getAnswers())
            answers += std::string("answer:") + answer + ";";

        broadcastMessage(answers);

        mCurrentCorrectAnswer = question.getCorrectAnswer();
        mBroadcastTimepoint = std::chrono::high_resolution_clock::now();
        
        while (!mGotAllAnswers)
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

        broadcastPunctation();  
    }

    broadcastMessage(std::string("theGameEnds"));
    mRun = false;
}

void Game::broadcastPunctation()
{
    std::string msg;
    for (auto& punct : mPunctation)
    {
        msg = msg + punct.first + ":" + std::to_string(punct.second) + ";";
    }

    broadcastMessage(msg);
}

void Game::extractAnswer(const std::vector<std::string>& msg)  //format odpowiedzi answer:OdpowiedzTekstem;player:NickGracza;timestamp:CzasOddaniaOdpowiedzi;
{
    using namespace std::chrono;

    const int twoThirds = static_cast<int>(static_cast<double>(2 * mPunctation.size()) / 3);
    int elapsed = high_resolution_clock::now().time_since_epoch().count() - mBroadcastTimepoint.time_since_epoch().count();

    if (elapsed < mTimePerQuestion && mAnswersNum < twoThirds)
    {
        mPunctation[msg[3]] += calculatePoints(msg);
        mAnswersNum++;
    }
    else
        mGotAllAnswers = true;
}

double Game::calculatePoints(const std::vector<std::string> &msg) const
{
    using namespace std::chrono;

    if (mCurrentCorrectAnswer == msg[1])
    {
        long dur = std::stol(msg[5]) - mBroadcastTimepoint.time_since_epoch().count();

        return (dur / mTimePerQuestion) * 1000;
    }

    return 0.0;
}

bool Game::addPlayer(const int clientFd)
{
    auto nickMsg = receiveMessage(clientFd);
    if (mPunctation.find(nickMsg[1]) != mPunctation.end())
    {
        sendMessage(std::string("rejected"), clientFd);
        return false;
    }
    else
        sendMessage(std::string("accepted"), clientFd);

    mPunctation[nickMsg[1]] = 0;
    return true;
}

Questions Game::createQuestion(const int questionNum)
{
    const int hostFd = mPolls.at(1).fd;
    sendMessage(std::string("sendQuestion:") + std::to_string(questionNum), hostFd);
    auto questionMsg = receiveMessage(hostFd);

    std::array<std::string, 4> answers;
    for (int j = 0; j < 4; ++j)
    {
        sendMessage(std::string("sendAnswer:") + std::to_string(j), hostFd);
        auto answerMsg = receiveMessage(hostFd);
        answers[j] = answerMsg[1];
    }

    sendMessage(std::string("provideCorrectMsgIndex"), hostFd);
    auto indexMsg = receiveMessage(hostFd);
    int index = std::stoi(indexMsg[1]);

    Questions q(questionMsg[1], answers);
    q.setCorrectAnswerIndex(index);

    return q;
}

void Game::handleResponse(const int& fd)
{   
    auto message = receiveMessage(fd);

    if ((message[0] == "joinGame") || (message[0] == "createGame"))
    {
        ;//ignore
    }

    if (message[0] == "answer")
    {
        extractAnswer(message);
    }

    if (message[0] == "startTheGame" && fd == mPolls[1].fd)
    {
        std::thread th(&Game::runTheGame, this);
    }
}

bool Game::acceptClient()
{
    if (mTrafficClosed)
        return false;

    int clientFd = accept(mSock, nullptr, nullptr);
    if (clientFd == -1)
    {
        mDebugFile << "accept failed";
        return false;
    }

    if (!addPlayer(clientFd))
        return false;

    pollfd poll;
    poll.fd = clientFd;
    poll.events = POLLIN;

    mPolls.push_back(poll);

    if (mPolls.size() == 2)
        setupGame();

    return true;
}

void Game::setupGame()
{
    const int hostFd = mPolls[1].fd;
    auto questionsMsg = receiveMessage(hostFd);
    const int questionsNum = std::stoi(questionsMsg[1]);

    auto timesMsg = receiveMessage(hostFd);
    mTimePerQuestion = std::stoi(timesMsg[1]);

    for (int i = 0; i < questionsNum; ++i)
        mQuestions.push_back(createQuestion(i));
}

void Game::run()
{
    while (mRun)
    {
        int res = poll(mPolls.data(), mPolls.size(), -1);

        for (auto& client : mPolls)
        {
            if (client.revents & POLLIN)
            {
                if (client.fd == mSock)
                    bool x = acceptClient();
                else
                    handleResponse(client.fd);
            }
        }
    }
}