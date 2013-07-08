// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef SERVER_H
#define SERVER_H

#include <WebSocket/WebSocket.h>

#include <unordered_map>
#include <cstdint>
#include <memory>
#include <string>

namespace chat
{

        // Forward declaration of classes
        class LinuxSocket;
        class Dispatcher;

        /// Represents server for chat.
        class Server
        {
        public:
                /// Represents configuration of the server.
                class Configuration
                {
                public:
                        /// \brief Constructs configuration object with given port.
                        /// \param[in] port server port
                        Configuration(uint16_t port);
                        ~Configuration();

                        /// \brief Returns server port.
                        /// \return server port
                        uint16_t getServerPort() const;

                private:
                        uint16_t port_;

                };

                /// \brief Constructs server with given configuration and dispatcher.
                /// \param[in] configuration configuration of the server
                /// \param[in] dispatcher dispatcher
                Server(const Configuration& configuration, ws::WebSocket::Dispatcher& dispatcher);
                Server(const Server&) = delete;
                ~Server();
                Server& operator=(const Server&) = delete;

                /// \brief Returns configuration of the server.
                /// \return server configuration
                const Configuration& getConfiguration() const;

                /// \brief Initializes and runs server.
                /// Runs endless loop until halt() method is invoked.
                /// \return false if some error occurred during initialization or running
                bool initializeAndRun();

                /// \brief Halts server.
                void halt();

        private:
                typedef std::unordered_map<int, std::unique_ptr<LinuxSocket>> SocketsMap;

                /// Helper constants
                enum
                {
                        BUFFER_SIZE = 4096,
                        EPOLL_SIZE = 10000
                };

                Configuration configuration_;
                ws::WebSocket::FrameBuffer frameBuffer_;
                ws::WebSocket::Dispatcher* dispatcher_;

                SocketsMap sockets_;

                int epollDescriptor_, listener_;
                volatile bool isActive_;

                /// \brief Adds socket.
                void addSocket();

                /// \brief Removes socket.
                /// \param[in] handle socket handle
                void removeSocket(int handle);

                /// \brief Runs server.
                /// Runs endless loop until halt() method is invoked.
                /// \return true if server has been successfully stopped
                bool run();

        };

}

#endif
