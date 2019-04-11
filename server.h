#pragma once

#include "core.h"
#include "contest.h"

class Server{

    public:
        Server();
        ~Server();

        bool listening();
        bool parse_input();
        void close_connection();
    private:
        int _socket;
        int _connected_socket;
        std::vector<Question*> _questions;
        std::vector<Contest*> _contests;

        struct sockaddr_in address;

        void socket_init();

        //read in questions from file
        void read_in_questions();

        //handling commands
        void create_question(char* msg);
        void delete_question(char* msg);
        void get_question(char* msg);
        void review();
        // void get_rand_question();
        bool index_valid(int index);
        void check_answer(char* msg);
        void set_contest(char* msg);
        void add_q_contest(char* msg);
        void begin_contest(char* msg);
        void list_contests(char* msg);
        //        int assign_val();
        int index_of(int num);
        int index_of_contest(int num);
        //protocol commands
        int yeet(std::string s);
        std::string yoink();
};


