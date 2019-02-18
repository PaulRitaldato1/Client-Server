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
        void socket_init(const char* hostname, const char* port);
    private:
        int _socket;
        struct sockaddr_in address, serv_addr;

        int resolve_hostname(const char* hostname, const char* port,char* ip);
        void help();
        void kill();
        void quit();
        void check_answer();
        void random();
        void get();
        void delete_q();
        void put_q();
};
