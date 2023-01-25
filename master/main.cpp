#include "server.hpp"

int main(int argc, char* argv[])
{
    Server server(8080);
    
    if (server.isUp())
		server.run();

    return 0;
}
