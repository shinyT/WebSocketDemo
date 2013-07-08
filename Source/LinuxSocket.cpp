// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "LinuxSocket.h"

#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <cassert>

namespace chat
{

        LinuxSocket::LinuxSocket(int handle,
                                 ws::WebSocket::Dispatcher& dispatcher,
                                 ws::WebSocket::FrameBuffer& frameBuffer):
                webSocket_(*this, dispatcher, frameBuffer), handle_(handle) {}
        LinuxSocket::~LinuxSocket()
        {
                std::cout << "destroying socket, fd: " << handle_ << std::endl;
                if(handle_ >= 0)
                        ::close(handle_);
        }

        //---------------------------------------------------------------------
        bool LinuxSocket::send(const uint8_t* data, size_t size) const
        {
                if(handle_ < 0)
                        return false;

                ::send(handle_, data, size, 0);
                return true;
        }

        //---------------------------------------------------------------------
        bool LinuxSocket::isOpen()
        {
                return (handle_ >= 0);
        }

        //---------------------------------------------------------------------
        int LinuxSocket::getHandle() const
        {
                return handle_;
        }

        //---------------------------------------------------------------------
        size_t LinuxSocket::read(uint8_t* data, size_t size) const
        {
                assert(handle_ >= 0);
                return ::recv(handle_, data, size, 0);
        }

        //---------------------------------------------------------------------
        void LinuxSocket::release()
        {
                if(!isOpen())
                        return;

                ::close(handle_);
                handle_ = -1;
        }

}
