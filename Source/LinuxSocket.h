// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef LINUX_SOCKET_H
#define LINUX_SOCKET_H

#include <WebSocket/WebSocket.h>

namespace chat
{

        /// Represents Linux socket.
        class LinuxSocket: public ws::Socket
        {
        public:
                /// \brief Constructs socket with given handle.
                /// \param[in] handle handle (file descriptor)
                /// \param[in] dispatcher dispatcher (dispatches WebSocket messages)
                /// \param[in] frameBuffer WebSocket frame buffer
                LinuxSocket(int handle,
                            ws::WebSocket::Dispatcher& dispatcher,
                            ws::WebSocket::FrameBuffer& frameBuffer);
                LinuxSocket(const LinuxSocket&) = delete;
                virtual ~LinuxSocket();
                LinuxSocket& operator =(const LinuxSocket&) = delete;

                // ws::Socket interface implementation
                virtual bool send(const uint8_t* data, size_t size) const final;
                virtual bool isOpen() final;

                /// \brief Returns socket handle.
                /// \return handle
                int getHandle() const;

        private:
                ws::WebSocket webSocket_;
                int handle_;

                // ws::Socket interface implementation
                virtual size_t read(uint8_t* data, size_t size) const final;
                virtual void release() final;

        };

}

#endif
