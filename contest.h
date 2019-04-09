#pragma once

#include "server.h"

class Contest{
 public:
    Contest();
 private:
    //network connection variabls
    //File descriptors for sockets, IP addrs, etc...
    fd_set contestants;
    int _master_socket, _new_connect;
    struct sockaddr_in address;

    int startContest();


    int yeet(std::string message, int socket);
    int yoink(int socket);
};
