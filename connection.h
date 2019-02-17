
/* C networking headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<stdio.h>
/* end C networking headers */

/* C++ headers */
#include<iostream>
#include<vector>
#include "question.h"
#include <stdexcept>
#include <fstream>
/* end C++ headers */

// Connection interface
class Connection{

    public:
        virtual void close_connection() = 0;

    private:
        int _socket;
        struct sockaddr_in address;

        virtual void socket_init() = 0;
}; //end connection interface


class Server : public Connection {

    public:
        Server();
        ~Server();

        bool listening();
        bool parse_input();
        void close_connection();
    private:
        int _socket;
        int _connected_socket;
        std::vector<Question> _questions;

        struct sockaddr_in address;

        void socket_init();

        //read in questions from file
        void read_in_questions();

        //handling commands
        void create_question(char* msg);
        void delete_question(char* msg);
        void get_question(char* msg);
        void get_rand_question();
        bool index_valid(int index);
        void check_ans_help(int q_num);
        void check_answer(char* msg);
};

class Client : public Connection {

    public:
        Client();
        ~Client();

        int connect();
        int make_packet();
        void close_connection();
    private:
        int _socket;
        struct sockaddr_in address;

        void init_socket();
};

