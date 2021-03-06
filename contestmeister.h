#pragma once
#include "core.h"
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
        int random(std::string message);
        int get(std::string message);
        int delete_q(std::string message);
        int put_q(std::istream& stream, std::string message);
        int set_contest(std::string message);
        int add_question(std::string message);
        int begin_contest(std::string message);
        int list_contests(std::string message);
        int command_control(std::istream& stream);

        void yoink();
        int yeet(std::string s);
};
