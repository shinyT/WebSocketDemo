// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Chat.h"

#include <iostream>
#include <utility>

namespace chat
{

        Chat::Chat(): users_() {}
        Chat::~Chat()
        {
                std::cout << "destroying chat" << std::endl;
        }

        //----------------------------------------------------------------------------------------
        void Chat::onOpen(ws::WebSocket& webSocket)
        {
                users_.insert(&webSocket);
                std::cout << "web socket has been opened, number of users: " <<
                             users_.size() << std::endl;

                const uint8_t data[] = "Hello!";
                const size_t  size = sizeof(data) - 1;
                webSocket.send(data, size);
        }

        //----------------------------------------------------------------------------------------
        void Chat::onReceive(ws::WebSocket&, const uint8_t* data, size_t size)
        {
                if(size == 0)
                        return;

                for(auto it = users_.begin(); it != users_.end(); ++it)
                        (*it)->send(data, size);
        }

        //----------------------------------------------------------------------------------------
        void Chat::onPing(ws::WebSocket&)
        {
                std::cout << "ping" << std::endl;
        }

        //----------------------------------------------------------------------------------------
        void Chat::onPong(ws::WebSocket&)
        {
                std::cout << "pong" << std::endl;
        }

        //----------------------------------------------------------------------------------------
        void Chat::onClose(ws::WebSocket& webSocket, ws::WebSocket::STATUS status)
        {
                users_.erase(&webSocket);

                std::cout << "web socket has been closed [" << status << "], number of users: " <<
                             users_.size() << std::endl;
        }

}
