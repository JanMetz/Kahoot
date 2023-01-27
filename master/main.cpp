#include "server.hpp"

int main(int argc, char* argv[])
{
    Server server(8080);
    
    if (server.establishConnection() && server.isUp())
		  server.run();
  
    return 0;
}
