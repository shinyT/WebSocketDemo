This is a simple C++ WebSocket server. To build this, you should first install WebSocket library: https://github.com/everard/WebSocket

REQUIREMENTS
============
The source code can be compiled with any C++ compiler, which supports C++11 standard.

COMPILATION
===========
Before proceeding to the next steps you should clone this repository:

        git clone git://github.com/everard/WebSocketDemo

Now you will be able to compile sources. This repository provides code::blocks project, which has been configured for compilation with GCC.
If you wish to compile it with GCC from command line, be sure to provide -lcrypto and -lWebSocket (or -lWebSocketd for DEBUG build) compiler flags.

HOW TO USE
==========
Run created executable and open client.html with your browser. Type in the IP address and press "connect". Now you can send messages to the server (type them in the textarea and press "send").
To terminate application, you should sent SIGUSR1 signal to it:

        killall -s SIGUSR1 WebSocketDemo

LICENSE
=======
Copyright (c) 2012 Nezametdinov E. Ildus

This software is licensed under the MIT License (see LICENSE.txt).
