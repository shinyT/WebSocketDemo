// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Server.h"

#include "LinuxSocket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <utility>
#include <new>

namespace chat
{

        Server::Configuration::Configuration(uint16_t port): port_(port) {}
        Server::Configuration::~Configuration() {}

        //-----------------------------------------------------------------------------------------
        uint16_t Server::Configuration::getServerPort() const
        {
                return port_;
        }

        Server::Server(const Configuration& configuration, ws::WebSocket::Dispatcher& dispatcher):
                configuration_(configuration), frameBuffer_(8196), dispatcher_(&dispatcher),
                sockets_(), epollDescriptor_(-1), listener_(-1), isActive_(false) {}
        Server::~Server()
        {
                if(listener_ >= 0)
                        ::close(listener_);
                if(epollDescriptor_ >= 0)
                        ::close(epollDescriptor_);
                std::cout << std::endl << "destroying server" << std::endl;
        }

        //-----------------------------------------------------------------------------------------
        const Server::Configuration& Server::getConfiguration() const
        {
                return configuration_;
        }

        //-----------------------------------------------------------------------------------------
        bool Server::initializeAndRun()
        {
                if(isActive_)
                        return false;

                // create main listening socket and make it non-blocking
                std::cout << "creating listening socket...";

                listener_ = ::socket(PF_INET, SOCK_STREAM, 0);
                if(listener_ < 0)
                {
                        std::cout << "failed" << std::endl;
                        return false;
                }

                if(::fcntl(listener_, F_SETFL, ::fcntl(listener_, F_GETFD, 0) | O_NONBLOCK) < 0)
                {
                        std::cout << "failed" << std::endl;
                        return false;
                }
                std::cout << "succeeded" << std::endl;

                // bind listener to the port
                std::cout << "binding socket to port " << configuration_.getServerPort() << "...";

                struct sockaddr_in localAddress;
                localAddress.sin_family = PF_INET;
                localAddress.sin_port   = htons(configuration_.getServerPort());
                localAddress.sin_addr.s_addr = INADDR_ANY;

                if(::bind(listener_, reinterpret_cast<struct sockaddr*>(&localAddress),
                          sizeof(localAddress)) < 0)
                {
                        std::cout << "failed" << std::endl;
                        return false;
                }
                std::cout << "succeeded" << std::endl;

                // start to listen
                std::cout << "starting to listen...";

                if(::listen(listener_, 1) < 0)
                {
                        std::cout << "failed" << std::endl;
                        return false;
                }
                std::cout << "succeeded" << std::endl;

                // create epoll
                std::cout << "creating epoll...";

                epollDescriptor_ = ::epoll_create(EPOLL_SIZE);
                if(epollDescriptor_ < 0)
                {
                        std::cout << "failed" << std::endl;
                        return false;
                }
                std::cout << "succeeded" << std::endl;

                // add listener to epoll
                std::cout << "adding listening socket to epoll...";

                struct epoll_event epollEvent;
                epollEvent.events  = EPOLLIN | EPOLLET;
                epollEvent.data.fd = listener_;

                if(::epoll_ctl(epollDescriptor_,
                               EPOLL_CTL_ADD,
                               listener_,
                               &epollEvent) < 0)
                {
                        std::cout << "failed" << std::endl;
                        return false;
                }
                std::cout << "succeeded" << std::endl;

                isActive_ = true;
                return run();
        }

        //-----------------------------------------------------------------------------------------
        void Server::halt()
        {
                isActive_ = false;
        }

        //-----------------------------------------------------------------------------------------
        void Server::addSocket()
        {
                struct sockaddr_in remoteAddress;
                struct epoll_event epollEvent;

                socklen_t socketSize = sizeof(struct sockaddr_in);
                epollEvent.events = EPOLLIN | EPOLLET;

                int client = ::accept(listener_,
                                      reinterpret_cast<struct sockaddr*>(&remoteAddress),
                                      &socketSize);

                if(client < 0)
                        return;

                // setup non-blocking socket
                if(::fcntl(client, F_SETFL, ::fcntl(client, F_GETFD, 0) | O_NONBLOCK) < 0)
                {
                        ::close(client);
                        return;
                }

                // set new client to event template
                epollEvent.data.fd = client;

                // add new client to epoll
                if(::epoll_ctl(epollDescriptor_, EPOLL_CTL_ADD, client, &epollEvent) < 0)
                {
                        ::close(client);
                        return;
                }

                std::unique_ptr<LinuxSocket> socket(new(std::nothrow) LinuxSocket(client,
                                                                                  *dispatcher_,
                                                                                  frameBuffer_));
                if(!socket)
                {
                        ::close(client);
                        return;
                }

                try
                {
                        sockets_.insert(std::make_pair(client,
                                                       std::move(socket)));
                }
                catch(...)
                {
                        ::close(client);
                        return;
                }

                std::cout << "socket has been opened, address: " << std::hex <<
                             remoteAddress.sin_addr.s_addr << std::dec << ", handle: " <<
                             client << ", number of sockets: " <<
                             sockets_.size() << std::endl;
        }

        //-----------------------------------------------------------------------------------------
        void Server::removeSocket(int handle)
        {
                sockets_.erase(handle);
                std::cout << "socket has been closed, fd: " << handle << ", number of sockets: " <<
                             sockets_.size() << std::endl;
        }

        //-----------------------------------------------------------------------------------------
        bool Server::run()
        {
                struct epoll_event epollEvents[EPOLL_SIZE];
                uint8_t buffer[BUFFER_SIZE];
                int numEpollEvents;

                while(isActive_)
                {
                        numEpollEvents = ::epoll_wait(epollDescriptor_, epollEvents,
                                                      EPOLL_SIZE, -1);

                        for(register int i = 0; i < numEpollEvents; ++i)
                        {
                                // check for error
                                if(epollEvents[i].events & EPOLLERR)
                                {
                                        removeSocket(epollEvents[i].data.fd);
                                        continue;
                                }

                                // EPOLLIN event for listener (new client connection)
                                if(epollEvents[i].data.fd == listener_)
                                {
                                        addSocket();
                                }
                                else
                                {
                                        auto it = sockets_.find(epollEvents[i].data.fd);
                                        if(it == sockets_.end())
                                        {
                                                ::close(epollEvents[i].data.fd);
                                                continue;
                                        }

                                        auto socket = it->second.get();
                                        size_t size = socket->receive(buffer, BUFFER_SIZE);
                                        if(size == 0 || !socket->isOpen())
                                                removeSocket(epollEvents[i].data.fd);
                                }
                        }
                }

                return true;
        }

}
