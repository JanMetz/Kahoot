#include "game.hpp"
#include "message.hpp"

#include <vector>
#include <map>
#include <array>

#include <chrono>
#include <string>
#include <thread>

Game::Game(const long port, const std::string& addr, const int hostFd) : Server(port, addr)
{
    const int questionsNum = std::stoi(readIni("config.ini", "questions_per_game"));
    mTimePerQuestion = std::stoi(readIni("config.ini", "seconds_per_question"));

    sendMessage(std::string("numOfQuestions:") + std::to_string(questionsNum), clientsFd);

    for (int i = 0; i < questionsNum; ++i)
    {
        mQuestions.push_back(createQuestion(i));
    }

    const int code = generateCode();
    mGames[code] = game;

    sendMessage(std::string("gameCode:") + std::to_string(code), clientsFd);
}

void Game::runTheGame()
{
    sendMessage(std::string("theGameStarts"));
    //block incoming traffic

    for (const auto question : mQuestions)
    {
        sendMessage(std::string("question:") + question.getQuestionBody());
        mBroadcastTimepoint = std::chrono::high_resolution_clock::now();
        waitForAnswers(question);
        broadcastPunctation();
    }

    sendMessage(std::string("theGameEnds"));
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

Question Game::createQuestion(const int questionNum) const
{
    sendMessage(std::string("sendQuestion:") + std::to_string(questionNum), clientsFd);
    question = receiveMessage(clientsFd);

    std::array<4, std::string> answers;
    for (int j = 0; j < 4; ++j)
    {
        sendMessage(std::string("sendAnswer:") + std::to_string(j), clientsFd);
        answers[j] = receiveMessage(clientsFd);
    }

    sendMessage(std::string("provideCorrectMsgIndex"), clientsFd);
    int = receiveMessage(clientsFd);

    Question q(question, answers);
    q.setCorrectAnswerIndex(num);

    return q;
}

void Game::handleResponse(const int& fd)
{
    auto message = receiveMessage(fd);

    /*size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
    }
    if (message)*/

    //createGame(fd);
    //handleAnswer(fd);
}
