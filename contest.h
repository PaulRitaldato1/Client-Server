#pragma once

#include "core.h"

class Contest{
 public:

 Contest(uint8_t contest_num) : contest_num(contest_num) {};
    Contest(uint8_t contest_num, double average_correct, int max_correct, std::vector<int> q_nums, std::vector<Question*>& all_questions);
    inline uint8_t get_contest_num(){ return contest_num; }
    void run_contest();
    bool add_question(Question* question);
    std::string list_contest();
    std::string evaluate_contest();
    void write_out();
    std::string review();
    inline int get_max() {return max_correct;}
    inline double get_average() { return average_correct; }
    inline int get_total() { return total_questions; }
    inline std::vector<Question*>& get_questions() {return _contest_questions;}
    inline Contest* get_instance(){return this;}

    inline void set_average(double avg) {average_correct = avg; }
    inline void set_max(int max) {max_correct = max; }

    int yeet(std::string message, int socket);
    std::string yoink(int socket);
 private:
    uint8_t contest_num;
    std::vector<Question*> _contest_questions;
    std::vector<std::pair<int, double>> per_q_stats;
    bool run = false;
    double average_correct = 0.0;
    int max_correct = 0;
    int total_questions = 0;

    void begin_contest();
    inline void get_address(struct sockaddr_in& addr) {addr.sin_port = 0;}

    //File descriptors for sockets, IP addrs, etc...
    //add to run_contest
    //fd_set contestants;
    //int _master_socket, _new_connect;
    //struct sockaddr_in address;





};

struct Contestant{
    std::string nickname;
    int sock;
    char answer;
    bool correct;
    int num_correct;
};