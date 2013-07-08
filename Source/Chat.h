// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef CHAT_H
#define CHAT_H

#include <WebSocket/WebSocket.h>
#include <unordered_set>

namespace chat
{

        /// Represents chat.
        class Chat: public ws::WebSocket::Dispatcher
        {
        public:
                Chat();
                Chat(const Chat&) = delete;
                virtual ~Chat();
                Chat& operator =(const Chat&) = delete;

                // WebSocket::Dispatcher interface implementation
                virtual void onOpen(ws::WebSocket& webSocket) final;
                virtual void onReceive(ws::WebSocket& webSocket, const uint8_t* data, size_t size) final;
                virtual void onPing(ws::WebSocket& webSocket) final;
                virtual void onPong(ws::WebSocket& webSocket) final;
                virtual void onClose(ws::WebSocket& webSocket, ws::WebSocket::STATUS status) final;

        private:
                std::unordered_set<ws::WebSocket*> users_;

        };

}

#endif
