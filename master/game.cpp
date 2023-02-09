#include "game.hpp"

#include <poll.h>
#include <cmath>
#include <cstring>

#include <chrono>
#include <thread>
#include <algorithm>

#include <string>
#include <vector>
#include <map>
#include <array>

Game::Game(const long port, const long motherPort) : Server(port), mTrafficClosed(false), mMotherPort(motherPort)
{
    if (!establishConnection())
        mRun = false;
    else
        mRun = true;
}

Game::~Game()
{
    notifyOwnerAboutClosing();
}

void Game::notifyOwnerAboutClosing()
{
    sockaddr_in addrStruct;
    addrStruct.sin_family = AF_INET;
    addrStruct.sin_addr.s_addr = htonl(inet_addr("127.0.0.1"));
    addrStruct.sin_port = htons((uint16_t)mMotherPort);

    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1)
    {
        log("Error: socket failed");
        return;
    }

    if(connect(sock, (sockaddr*) &addrStruct, sizeof(addrStruct)))
    {
        log("Error: connect to mother failed");
        return;
    }

    std::string msgBody("deleteGame:");
    msgBody = msgBody + std::to_string(mPort);

    char* char_array = new char[msgBody.length() + 1];
    strcpy(char_array, msgBody.data());
    if (write(sock, char_array, strlen(char_array)) == -1)
        log("Error: sending data not successful");

    delete char_array;

    shutdown(sock, SHUT_RDWR);
    close(sock);
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

        broadcastMessage(std::string("question:") + question.getQuestionBody() + std::string(":"));
        log("Question sent");

        std::string answers = "answer:";
        for (auto& answer : question.getAnswers())
            answers += answer + ":";

        broadcastMessage(answers);
        log("Possible options sent. Waiting for players answers");

        mCurrentCorrectAnswer = question.getCorrectAnswer();
        mBroadcastTimepoint = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        
        bool timesUp = false;
        while ((!mGotAllAnswers) && (!timesUp))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            int elapsed = static_cast<int>((std::chrono::high_resolution_clock::now().time_since_epoch().count() - mBroadcastTimepoint) / 1000000000); // from nanoseconds to seconds

            if (elapsed >= mTimePerQuestion)
                timesUp = true;
        }

        broadcastPunctation();  

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    broadcastMessage(std::string("theGameEnds:"));
    log(std::string("Game ended"));
    mRun = false;
}

void Game::broadcastPunctation()
{
    std::string msg = "punctation:";
    for (auto& player : mPlayers)
        msg = msg + player.mNick + ":" + std::to_string(player.mScore) + ":";

    log(msg); // DO USUNIĘCIA PO DEBUGU
    broadcastMessage(msg);
}

void Game::extractAnswer(const std::vector<std::string>& msg)  // answer format Token:AnswerText:PlayersNick:Timestamp:
{
    using namespace std::chrono;

    const int twoThirds = std::round(static_cast<double>(2 * (mPlayers.size() - 1)) / 3); // calculate without the game host
    int elapsed = static_cast<int>((std::stol(msg[3]) - mBroadcastTimepoint) / 1000000000); // from nanoseconds to seconds

    if ((elapsed < mTimePerQuestion) && (mAnswersNum <= twoThirds) && (mCurrentCorrectAnswer == msg[1]))
    {
        auto it = std::find_if(mPlayers.begin(), mPlayers.end(), [&](const Player &player){return player.mNick == msg[2];});
        if (it != mPlayers.end())
            it->mScore += (1000 * (mTimePerQuestion - elapsed) / mTimePerQuestion);   
    }
    
    if (mAnswersNum >= twoThirds)
        mGotAllAnswers = true;
}

void Game::handleResponse(const int& fd)
{   
    std::vector<std::string> message;
    if (!receiveMessage(fd, 1, message))
        return;

    if ((message[0] == "joinGame") || (message[0] == "createGame"))
    {
        log("Received invalid request");
    }

    if ((message.size() > 3) && (message[0] == "answer"))
    {
        mAnswersNum++;
        extractAnswer(message);
    }

    if ((message[0] == "startTheGame") && (fd == mPolls[1].fd))
    {
        std::thread th(&Game::runTheGame, this);
        th.detach();
    }

    if (message[0] == "leaveTheGame")
    {
        removeClient(fd);
    }
}

void Game::broadcastMessage(const std::string& msgBody)
{
    for (auto &player : mPlayers)
        sendMessage(player.mFd, msgBody);
}

bool Game::acceptClient()
{
    if (mTrafficClosed)
    {
        log(std::string("Warning: connection request rejected - traffic is closed"));
        return false;
    }

    int clientFd = accept(mSock, nullptr, nullptr);
    if (clientFd == -1)
    {
        log(std::string("Error: accept failed"));
        return false;
    }

    pollfd poll;
    poll.fd = clientFd;
    poll.events = POLLIN;
    mPolls.push_back(poll);

    if (!addPlayer(clientFd))
    {  
        removeClient(clientFd);
        return false;
    }
   
    if (mPolls.size() == 2)
        setupGame();

    return true;
}

std::vector<std::string> Game::receiveMessage_correctSizeOnly(const int fd, const int size)
{
    log("Waiting for message...");

    pollfd client;
    client.events = POLLIN;
    client.fd = fd;

    std::vector<std::string> message;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        if (poll(&client, 1, -1) == -1)
            log("Error: poll failed");
    }
    while((!(client.revents & POLLIN)) || (!receiveMessage(fd, size, message)));

    return message;
}

bool Game::addPlayer(const int clientFd)
{
    auto nickMsg = receiveMessage_correctSizeOnly(clientFd, 1);

    auto it = std::find_if(mPlayers.begin(), mPlayers.end(), [&](const Player &player){return player.mNick == nickMsg[0];});
    if (it != mPlayers.end())
    {
        sendMessage(clientFd, std::string("rejected:"));
        log(std::string("Player ") + nickMsg[0] + std::string(" rejected"));

        return false;
    }
        
    mPlayers.push_back(Player(clientFd, 0.0, nickMsg[0]));

    sendMessage(clientFd, std::string("accepted:"));
    log(std::string("Player ") + nickMsg[0] + std::string(" accepted"));
    
    sendAllNicks();
    
    return true;
}

void Game::sendAllNicks()
{
    log(std::string("Broadcasting all nicks"));

    std::string allNicks = "allNicks:";
    for (auto& player : mPlayers)
    {
        allNicks += player.mNick + ":";
    }

    broadcastMessage(allNicks);
}

void Game::setupGame()
{
    log("Entering game setup mode");

    const int hostFd = mPolls.at(1).fd;
    auto setupMsg = receiveMessage_correctSizeOnly(hostFd, 2);
    
    const int questionsNum = std::stoi(setupMsg[0]);
    mTimePerQuestion = std::stoi(setupMsg[1]);

    for (int i = 0; i < questionsNum; ++i)
        mQuestions.push_back(createQuestion());

    log("Leaving game setup mode");
}

Questions Game::createQuestion()
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

    sendMessage(hostFd, std::string("provideCorrectMsgIndex:"));
    auto indexMsg = receiveMessage_correctSizeOnly(hostFd, 1);
    log(std::string("Received correct answer index: ") + indexMsg[0]);

    int index = std::stoi(indexMsg[0]);

    Questions q(questionMsg[0], answers);
    q.setCorrectAnswerIndex(index);

    return q;
}

void Game::removeClient(const int fd)
{
    if (fd == mPolls[1].fd)
    {
        mRun = false;
        log("Error: Host disconnected from the game");
        broadcastMessage(std::string("Error! Terminating game:"));
        closeConnection();
    }
    else
    {
        std::string nick("");
        auto it2 = std::find_if(mPlayers.begin(), mPlayers.end(), [&](const Player &player){return player.mFd == fd;});
        if (it2 != mPlayers.end())
        {
            nick = it2->mNick;
            mPlayers.erase(it2);
            log(std::string("Client ") + nick + std::string(" erased from the players lists"));
        }
        else
            log("Warning: clients fd not found in the players list");

        auto it = std::find_if(mPolls.begin(), mPolls.end(), [&](const pollfd &poll){return poll.fd == fd;});
        if (it != mPolls.end())
        {
            mPolls.erase(it);
             log(std::string("Client ") + nick + std::string(" erased from the polling lists"));
        }
        else
            log("Warning: clients fd not found in the polling list");
        
        shutdown(fd, SHUT_RD);
        close(fd);
         log(std::string("Client ") + nick + std::string(" disconnected"));

        sendAllNicks();
    }
}

void Game::run()
{
    while(mRun)
    {
        if (poll(mPolls.data(), mPolls.size(), -1) == -1)
        {
			log("Error: poll failed");
            continue;
        }

        for (auto &client : mPolls)
        {
            if (client.revents & POLLIN)
            {
                if (client.fd == mSock)
                    bool x = acceptClient();
                else
                    handleResponse(client.fd);
            }

            if (client.revents & POLLHUP)
                removeClient(client.fd);
        }
    }
}
