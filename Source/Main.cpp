// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Server.h"
#include "Chat.h"

#include <signal.h>

#include <iostream>
#include <cstdlib>

using namespace chat;

std::unique_ptr<Server> server;
void signalHandler(int) {if(server) server->halt();}

int main()
{
        ::signal(SIGUSR1, signalHandler);
        Chat chat;
        Server::Configuration configuration(9988);

        server.reset(new Server(configuration, chat));

        if(server)
                server->initializeAndRun();

        server.reset();

        std::cout << "press ENTER to exit";
        ::getchar();

        return 0;
}
