#include "game.hpp"

#include <vector>
#include <map>
#include <array>
#include <poll.h>

#include <chrono>
#include <string>
#include <thread>

Game::Game(const long port) : Server(port), mRun(true)
{
    bool unused = establishConnection();
}

void Game::runTheGame()
{
    sendMessage(std::string("theGameStarts"));
    log(std::string("Game started ") + std::to_string(mPort));
    mTrafficClosed = true;

    for (const auto question : mQuestions)
    {
        mAnswersNum = 0;
        mGotAllAnswers = false;
        mCurrentCorrectAnswer = "";

        sendMessage(std::string("question:") + question.getQuestionBody());

        std::string answers = "answer:";
        for (auto& answer : question.getAnswers())
            answers += answer + ":";

        sendMessage(answers);

        mCurrentCorrectAnswer = question.getCorrectAnswer();
        mBroadcastTimepoint = std::chrono::high_resolution_clock::now();
        
        while (!mGotAllAnswers)
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

        broadcastPunctation();  
    }

    sendMessage(std::string("theGameEnds"));
    log(std::string("Game ended ") + std::to_string(mPort));
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

    sendMessage(msg);
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
    auto nickMsg = receiveMessage(2);

    if (mPunctation.find(nickMsg[1]) != mPunctation.end())
    {
        sendMessage(std::string("rejected"));
        log(std::string("Player rejected ") + std::to_string(mPort));
        return false;
    }
    else
        sendMessage(std::string("accepted"));

    mPunctation[nickMsg[1]] = 0;

    std::string allNicks = "allNicks:";
    for (auto& nick : mPunctation)
    {
        allNicks += nick.first + ":";
    }

    sendMessage(allNicks);

    log(std::string("Player added ") + std::to_string(mPort));
    
    return true;
}

void Game::removePlayer(const int fd, const std::string &nick)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);

    auto it = mPolls.begin();
    for (it; it < mPolls.end();  ++it)
    {
        if (it->fd == fd)
            break;
    }

    mPolls.erase(it);
    mPunctation.erase(mPunctation.find(nick));

    log(std::string("Player removed ") + std::to_string(mPort));
}

Questions Game::createQuestion(const int questionNum)
{
    const int hostFd = mPolls.at(1).fd;
    sendMessage(std::string("sendQuestion:") + std::to_string(questionNum));
    auto questionMsg = receiveMessage(2);

    std::array<std::string, 4> answers;
    for (int j = 0; j < 4; ++j)
    {
        sendMessage(std::string("sendAnswer:") + std::to_string(j));
        auto answerMsg = receiveMessage(2);
        answers[j] = answerMsg[1];
    }

    sendMessage(std::string("provideCorrectMsgIndex"));
    auto indexMsg = receiveMessage(2);
    int index = std::stoi(indexMsg[1]);

    Questions q(questionMsg[1], answers);
    q.setCorrectAnswerIndex(index);

    return q;
}

void Game::handleResponse(const int& fd)
{   
    auto message = receiveMessage(1);

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

    if (message[0] == "leaveTheGame")
    {
        removePlayer(fd, message[1]);
    }
}

bool Game::acceptClient()
{
    if (mTrafficClosed)
        return false;

    int clientFd = accept(mSock, nullptr, nullptr);
    if (clientFd == -1)
    {
        log(std::string("Accept failed ") + std::to_string(mPort));
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
    auto questionsMsg = receiveMessage(2);
    const int questionsNum = std::stoi(questionsMsg[1]);

    auto timesMsg = receiveMessage(2);
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
