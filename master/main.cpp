#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include <chrono>
#include <vector>
#include <array>
#include <string>
#include <exception>
#include <fstream>



std::string readIni(const std::string& path, const std::string& setting)
{
    std::ifstream config(path);

    std::string setting_name;
    std::string setting_val("");
    while (!config.eof())
    {
        config >> setting_name;
        if (setting_name == setting)
        {
            config >> setting_val;
            break;
        }
    }

    config.close();
    return setting_val;
}

int main(int argc, char* argv[])
{
    return 0;
}

