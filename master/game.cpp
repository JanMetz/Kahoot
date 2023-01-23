#include "game.hpp"

#include <vector>
#include <map>
#include <array>

#include <chrono>
#include <string>
#include <thread>

Game::Game(const long port, const std::string& addr) : Server(port, addr)
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
        mCurrentCorrectAnswer = question.getCorrectAnswer();
        mBroadcastTimepoint = std::chrono::high_resolution_clock::now();
        
        while (!mGotAllAnswers)
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

        broadcastPunctation();  
    }

    broadcastMessage(std::string("theGameEnds"));
}

void Game::broadcastPunctation()
{
    std::string msg;
    for (auto& punct : mPunctation)
    {
        msg = msg + punct.first() + ":" + std::to_string(punct.second()) + ";";
    }

    broadcastMessage(msg);
}

void Game::extractAnswer(const std::vector<std::string>& msg) const  //format odpowiedzi answer:OdpowiedzTekstem;player:NickGracza;timestamp:CzasOddaniaOdpowiedzi;
{
    using namespace std::chrono;

    const int twoThirds = static_cast<int>(static_cast<double>(2 * mPlayers.size()) / 3);
    int elapsed = 0;

    if (elapsed < mTimePerQuestion && mAnswersNum < twoThirds)
    {
        duration<int> dur = high_resolution_clock::now() - mBroadcastTimepoint;
        elapsed = dur.count();

        mPunctation[msg[3]] += calculatePoints(msg);
        answersNum++;
    }
    else
        mGotAllAnswers = true;
}

double Game::calculatePoints(const std::vector<std::string> &msg) const
{
    if (mCurrentCorrectAnswer == msg[1])
    {
        std::chrono::duration<int> len = std::chrono::high_resolution_clock::time_point::time_since_epoch() - mBroadcastTimepoint;
        long dur = std::stol(msg[5]) - len.count();

        if (dur < 0)
            return 0.0;

        return ((mTimePerQuestion - dur) / mTimePerQuestion) * 1000;
    }

    return 0.0;
}

void Game::addPlayer(const std::string &nick)
{
    mPunctation[nick] = 0;
}

Question Game::createQuestion(const int questionNum) const
{
    const int hostFd = mPolls[0].fd;
    sendMessage(std::string("sendQuestion:") + std::to_string(questionNum), hostFd);
    question = receiveMessage(hostFd);

    std::array<4, std::string> answers;
    for (int j = 0; j < 4; ++j)
    {
        sendMessage(std::string("sendAnswer:") + std::to_string(j), hostFd);
        answers[j] = receiveMessage(hostFd);
    }

    sendMessage(std::string("provideCorrectMsgIndex"), hostFd);
    int = receiveMessage(hostFd);

    Question q(question, answers);
    q.setCorrectAnswerIndex(num);

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

    if (message[0] == "startTheGame")
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

    pollfd poll;
    poll.fd = clientFd;
    poll.events = POLLIN;

    mPolls.push_back(poll);

    if (mPolls.size() == 1)
        setupGame();

    return true;
}

void Game::setupGame()
{
    const int questionsNum = std::stoi(readIni("config.ini", "questions_per_game"));
    mTimePerQuestion = std::stoi(readIni("config.ini", "seconds_per_question"));

    const int hostFd = mPolls[0].fd;
    sendMessage(std::string("numOfQuestions:") + std::to_string(questionsNum), hostFd);

    for (int i = 0; i < questionsNum; ++i)
        mQuestions.push_back(createQuestion(i));
}
