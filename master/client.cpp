#include "client.hpp"
#include "message.hpp"

#include <string>


Client::Client(const long port, const std::string &addr)
{
    /*mAddrStruct.sin_family = AF_INET;
    mAddrStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    mAddrStruct.sin_port = htons((uint16_t)port);

    mSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   
    if (mSock == -1)
    {
        mDebugFile << "socket failed";
        return false;
    }

    const int optval = 1;
    fcntl(mSock, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK)
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if (connect(mSock, (sockaddr*)&mAddrStruct, sizeof(mAddrStruct)))
    {
        mDebugFile << "bind failed";
        return false;
    }*/
}

Client::~Client()
{

}

Message Client::receiveMessage()
{
    /*size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
    }*/
}

void Client::sendMessage(const Message &msg)
{

}