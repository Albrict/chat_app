# Chat app
This is a client-server application that I wrote to learn network programming. Client only uses FLTK as GUI library and server uses SQLite database to store user info and [SHA1 library](https://github.com/vog/sha1/tree/master) for password encryption. 
Also I wrote a small network utils library for both the client and server.

To summarize what I learned and achieved:
- How to use Unix sockets for network
- How to use SQLite database and intergrate it into a program
- SHA1 encyption
- Learned how to encapsulate data to transmit it over a network

# How to compile
This program only compiles on Linux, because of use Unix sockets API. It can be ported tho using WINAPI sockets tho.

First, clone this repo

`git clone https://github.com/Albrict/chat_app.git`

Then

`cd chat_app && mkdir build`

Setup cmake

`cd build && cmake .. -DCMAKE_BUILD_TYPE=RELEASE`

Compile

`make`

# How to run it 
After compilation you will have two programs: the client and server. Run the server first

`./server`

After that you can launch client and use 3490 port to connect
# Program demonstration
https://github.com/Albrict/chat_app/assets/65279613/6050621c-33fa-41c4-bb75-49bbe47404b5
