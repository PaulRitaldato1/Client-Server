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
/* end C++ headers */

class Contestmeister{

    public:
        int connecting();
        int parse_input(filename);
        void close_connection();
        int socket_init(const char* hostname, const char* port);
    private:
        int _socket;
        struct sockaddr_in address, serv_addr;

        int resolve_hostname(const char* hostname, const char* port,std::string& ip);
        void help(std::istream& stream);
        int kill(std::istream& stream);
        int check_answer(std::istream& stream);
        int random(std::istream& stream);
        int get(std::istream& stream);
        int delete_q(std::istream& stream);
        int put_q(std::istream& stream);
        int set_contest(std::istream& stream);
        int add_question(std::istream& stream);
        int begin_contest(std::istream& stream);
        int list_contests(std::istream& stream);

        void read_response();
        int send_response(std::string s);
};
