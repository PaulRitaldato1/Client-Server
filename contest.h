#pragma once

#include "core.h"

class Contest{
 public:
 Contest(uint8_t contest_num) : contest_num(contest_num);

    inline uint8_t get_contest_num(){ return contest_num; }
 private:
    uint8_t contest_num;
    std::vector<Question*> _contest_questions;
    bool run = false;
    double average_correct = 0.0;
    int max_correct = 0;
    int total_questions = 0;

    //File descriptors for sockets, IP addrs, etc...
    //add to run_contest
    //fd_set contestants;
    //int _master_socket, _new_connect;
    //struct sockaddr_in address;

    void run_contest();
    void begin_contest();
    bool add_question(Question* question);
    void list_contest();
    std::string evaluate_contest();

    struct Contestant{
        std::string nickname;
        int sock;
        std::string answer;
        bool correct;
    };

    int yeet(std::string message, int socket);
    int yoink(int socket);
};
