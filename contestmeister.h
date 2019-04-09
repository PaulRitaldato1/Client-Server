#pragma once
/* C networking headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
/* end C networking headers */

/* C++ headers */
#include <iostream>
#include <vector>
#include "question.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
/* end C++ headers */

class Contestmeister{

    public:
        int connecting();
        int parse_input(std::string filename);
        void close_connection();
        int socket_init(const char* hostname, const char* port);
    private:
        int _socket;
        struct sockaddr_in address, serv_addr;

        int resolve_hostname(const char* hostname, const char* port,std::string& ip);
        void help();
        int kill();
        int check_answer(std::string message);
        int random(std::istream& stream);
        int get(std::string message);
        int delete_q(std::string message);
        int put_q(std::istream& stream, std::string message);
        int set_contest(std::string message);
        int add_question(std::string message);
        int begin_contest(std::string message);
        int list_contests();
        int command_control(std::istream& stream);

        void yoink();
        int yeet(std::string s);
};
