#pragma once

#include "core.h"

class Contest{
 public:
 Contest(std:vector<Question*>& all_questions, uint8_t contest_num) : all_questions(all_questions), contest_num(contest_num);

    inline uint8_t get_contest_num(){ return contest_num; }
 private:
    uint8_t contest_num;
    std::vector<Question*> contest_questions;
    std::vector<Question*> all_questions;
    bool run = false;
    double average_correct = 0.0;
    int max_correct;
    int total_questions = contest_questions.size();

    //File descriptors for sockets, IP addrs, etc...
    //add to run_contest
    //fd_set contestants;
    //int _master_socket, _new_connect;
    //struct sockaddr_in address;

    int run_contest();
    bool add_question(const unsigned int question_num);
    void list_contest();
    std::string evaluate_contest();


    int yeet(std::string message, int socket);
    int yoink(int socket);
};
