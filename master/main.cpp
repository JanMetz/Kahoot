#include "server.hpp"

int main(int argc, char* argv[])
{
    Server server(22, "localhost");
    server.run();

    return 0;
}

