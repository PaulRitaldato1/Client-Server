/* C networking headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdio.h>
#include<netdb.h>	
/* end C networking headers */

/* C++ headers */
#include<iostream>
#include<vector>
#include "question.h"
#include <stdexcept>
#include <fstream>
/* end C++ headers */

class Client{

    public:
        int connecting();
        int parse_input();
        void close_connection();
        int socket_init(const char* hostname, const char* port);
    private:
        int _socket;
        struct sockaddr_in address, serv_addr;

        int resolve_hostname(const char* hostname, const char* port,std::string& ip);
        void help();
        int kill();
        int check_answer();
        int random();
        int get();
        int delete_q();
        int put_q();

        void read_response();
        int send_response(std::string s);
};
