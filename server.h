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
        void create_question(std::string message);
        void delete_question(std::string message);
        void get_question(std::string message);
        void review(std::string message);
        // void get_rand_question();
        bool index_valid(int index);
        void check_answer(std::string message);
        void set_contest(std::string message);
        void add_q_contest(std::string message);
        void begin_contest(std::string message);
        void list_contests(std::string message);
        //        int assign_val();
        int index_of(int num);
        int index_of_contest(int num);
        //protocol commands
        int yeet(std::string s);
        std::string yoink();
};


