#include "contest.h"

Contest::Contest(uint8_t contest_num, double average_correct, int max_correct, std::vector<Question*>& all_questions){
    this->contest_num = contest_num;
    this->average_correct = average_correct;
    this->max_correct = max_correct;

    // for(int i = 0; i != q_nums.size(); ++i){
    //     int index = 0;
    //     for (int k = 0; k != all_questions.size(); ++k){
    //         if(q_nums[i] = all_questions[k]->get_question_num()){
    //             index = k;
    //             break;
    //         }
    //     }
    //     _contest_questions.push_back(all_questions[index]);
    // }
}
bool Contest::add_question(Question* question){
    _contest_questions.push_back(question);
    total_questions = _contest_questions.size();
    per_q_stats.push_back(std::make_pair(question->get_question_num(), 0.0));
}

std::string Contest::review(){
    std::string eval = std::to_string(contest_num) + "\t"+ std::to_string(total_questions) + " questions, ";
    if(!run){
        eval += "not run";
        return eval;
    }
    else{
        eval += "run, average correct: " + std::to_string(average_correct) + ";" + " maximum correct: " + std::to_string(max_correct) + "\n";
    }
    for(int i = 0; i != per_q_stats.size(); ++i){
        eval += "\t" + std::to_string(per_q_stats[i].first);
        if(run){
         eval += "\t" + std::to_string(per_q_stats[i].second) + "% corrrect\n";
        }
        else{
            eval += "\n";
        }
    }

    return eval;
}

std::string Contest::list_contest(){
    std::string run_or_not = "";
    std::string totalq = std::to_string(total_questions);
    std::string c_num = std::to_string(contest_num);
    std::string result = "";
    if(run)
        run_or_not = "run";
    else
        run_or_not = "not run";

    result += c_num + "\t";
    result += totalq + " questions, ";
    result += run_or_not + ", ";
    result += "average correct: " + std::to_string(average_correct) + ";";
    result += "maximum correct: " + std::to_string(max_correct);
    return result;
}

void Contest::run_contest(){
    std::thread t1 = std::thread(&Contest::begin_contest, this);
    t1.detach();
    run = true;
}

//THIS FUNCTION IS DESIGNED TO RUN ON ITS OWN THREAD, IT REPRESENTS AN "ACTIVE" CONTEST
//its also a mess, i know, but i ran out of time
void Contest::begin_contest(){
    run = true;
    int new_socket, activity, sd, max_sd;
    fd_set fds, copy;
    std::vector<Contestant> contestants;
    /* ======================================= Set up master Socket ===============================*/
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if( master_socket < 0){
        throw std::runtime_error("Contest::begin_contest: Failed to create TCP master socket file descriptor.");
    }

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    //set socket options
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        throw std::runtime_error("Contest::begin_contest: Failed to set master socket options.");
    }

    //set address options
    address.sin_family = AF_INET;
    address.sin_port = 0;
    address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket
    if(bind(master_socket, (struct sockaddr *)&address, sizeof(address) ) < 0){
        throw std::runtime_error("Server::socket_init(): Failed to bind socket.");
    }
    /* =================================== Set up master Socket end ===============================*/

    if(listen(master_socket, 3) < 0)
        throw std::runtime_error("Contest::begin_contest: Failed to listen on master socket.");

    // create scope for these temp variables
    {
    struct sockaddr_in new_address;
    int size = sizeof(new_address);
    getsockname(master_socket, (struct sockaddr*)&new_address, (socklen_t*) &size);
   	std::cout << "Contest " << std::to_string(contest_num) << " started on port: " << ntohs(new_address.sin_port) << std::endl;
    //std::cout << "Contest is on Hostname: storm.cise.ufl.edu" << std::endl;
    }

    //listen for new connections for 60 seconds
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        
 
    while(true){
        //clear socket set
        FD_ZERO(&fds);

        //add master socket to the socket set
        FD_SET(master_socket, &fds);
    
        max_sd = master_socket;
        for (int i = 0; i != contestants.size(); ++i){
            sd = contestants[i].sock;

            FD_SET(sd, &fds);
            copy = fds;
            if(sd > max_sd)
                max_sd = sd;
        }
        struct timeval timeout = {1, 0};
        activity = select(max_sd + 1, &fds, NULL, NULL, &timeout);
        if(FD_ISSET(master_socket, &fds)){
            if((new_socket = accept(master_socket, (struct sockaddr*)& address, (socklen_t*)&addrlen)) < 0){
                std::cerr << "Contest::begin_contest: Failed to accept connection." << std::endl;
            }

            //add new socket to contestants
            Contestant tmp;
            tmp.sock = new_socket;
            contestants.push_back(tmp);
        }
        //stop allowing connections after 60 seconds
        if(std::chrono::steady_clock::now() - start > std::chrono::seconds(10)){
            std::cout << "Done listening for connections" << std::endl;
            break;
        }
    }

    if(contestants.size() == 0){
        std::cout << "No contestants connected after 60 seconds, terminating.." << std::endl;
        run = false;
        return;
    }
    //No longer listening for contestants
    //close(master_socket);

    //runing contest with the curernt contestants
    //get contestant names
    std::vector<bool> bitmap;


    for (int i = 0; i != contestants.size(); ++i){
        bitmap.push_back(false);
    }

    //lambdas used to play the game
    auto checkAnswers = [&contestants, this](int q_num)mutable{ 
        for(int i = 0; i != contestants.size(); ++i){
        DEBUG(contestants[i].answer);
        DEBUG(_contest_questions[q_num]->get_answer());
        if(contestants[i].answer == _contest_questions[q_num]->get_answer()){
            contestants[i].correct = true;
            DEBUG("CheckAnswers::contestant.correct: " + std::to_string(contestants[i].correct));
        }
        else
            contestants[i].correct = false;
        }
    };
    auto reset = [&bitmap, &contestants]()mutable{ for (int i = 0; i != bitmap.size(); ++i) {
            bitmap[i] = false;
            contestants[i].correct = false;
            contestants[i].answer = 0;
            }
        };
    auto allResponded = [&bitmap](){
        for (auto is_set : bitmap)
            if(is_set == false)
                return false;
        return true;
    };
    auto sendAll = [this](std::vector<Contestant>& peeps, std::string msg){ for( auto peep : peeps) yeet(msg, peep.sock); };
    auto checkNickname = [&contestants](std::string& nickname){
        for(int i = 0; i != contestants.size(); ++i){
            if(contestants[i].nickname == nickname)
                return false;
        }
           return true;
    };

    sendAll(contestants, std::to_string(_contest_questions.size()));
    sendAll(contestants, "Please enter a nickname: ");
    fd_set copy1;

    /* TODO accept nicknames */
    while(!allResponded()){
        copy1 = copy;
        //struct timeval timeout = {1, 0};
        activity = select(max_sd + 1, &copy, NULL, NULL, NULL);
        copy = copy1;
        DEBUG("Made it to after select");
        for(int i = 0; i != contestants.size(); ++i){
            #ifdef DBG
            if(FD_ISSET(master_socket, &copy))
                DEBUG("Master socket activity");
            #endif
            if(FD_ISSET(contestants[i].sock, &copy) && bitmap[i] == false){
                std::string tmp = yoink(contestants[i].sock);
                DEBUG("Recieved tmp: " + tmp);
                if(!checkNickname(tmp)){
                    yeet("Error: Nickname " + tmp + " is already in use.", contestants[i].sock);
                }
                else{
                    yeet("\nHello " + tmp + ", get ready for the contest!\n", contestants[i].sock);
                    contestants[i].nickname = tmp;
                    bitmap[i] = true;
                }
            }
        }
    }
    reset();

    //main contest loop
    int tmp_max_correct = 0;
    copy = copy1;
    for(int question = 0; question != _contest_questions.size(); ++question){
        sendAll(contestants, _contest_questions[question]->to_string_rand());
        activity = select(max_sd + 1, &copy, NULL, NULL, NULL);
        copy = copy1;
        while(!allResponded()){
            for(int i = 0; i != contestants.size(); ++i){
                if(FD_ISSET(contestants[i].sock, &copy) && bitmap[i] == false){
                    contestants[i].answer = yoink(contestants[i].sock).at(0);
                    bitmap[i] = true;
                }
            }

        }
        /* TODO check answers */
        checkAnswers(question);
        /* TODO calc and send statistics back */
        //calc stats
        double per_q_percent = 0.0;
        int top_score = 0;
        int tmp = 0;
        for (int i = 0; i != contestants.size(); ++i){
            DEBUG("Contestants correct: " + std::to_string(contestants[i].correct));
            if(contestants[i].correct){
                contestants[i].num_correct = contestants[i].num_correct+1;
                ++tmp;
            }
            if(contestants[i].num_correct > top_score)
                top_score = contestants[i].num_correct;
        }

        //sending responses to each contestant
        per_q_percent = ((double)tmp/(double)contestants.size()) * 100;
        std::string response = "";
        for(int i = 0; i != contestants.size(); ++i){
            if(contestants[i].correct)
                response += "Correct. ";
            else
                response += "Incorrect. ";
            response += std::to_string(per_q_percent) + " of contestants answered this question correctly.\n";
            response += "Your score is " + std::to_string(contestants[i].num_correct) + "/" + std::to_string(question + 1) + "." + " The top score is currently " + std::to_string(top_score) + "/" + std::to_string(question+1) + ".\n";

            yeet(response, contestants[i].sock);
        }
        per_q_stats[question].first = _contest_questions[question]->get_question_num();
        per_q_stats[question].second = per_q_percent;
        /* TODO reset bitmap, responses, and "correct" in contestants */
        reset();
    }
    /* TODO update total stats for contest? (maybe after contest is over) */
    double avg_correct = 0.0;
    int tmpmax = 0;
    for(int i =0; i != contestants.size(); ++i){
        if(contestants[i].num_correct > tmpmax)
            tmpmax = contestants[i].num_correct;
        avg_correct += (double)contestants[i].num_correct/(double)_contest_questions.size();
    }
    max_correct = tmpmax;
    avg_correct = (avg_correct/contestants.size())*100;
    average_correct = avg_correct;

    for(int i = 0; i != contestants.size(); ++i){
        std::string message = "The contest is over -- thanks for playing " + contestants[i].nickname + "!";
        yeet(message, contestants[i].sock);
    }
    //close all connections
    for(int i = 0; i != contestants.size(); ++i){
        close(contestants[i].sock);
    }
}


void Contest::write_out(){

    std::string out_str = "";
    //cotnest, questions, average_correct, max_correct
    out_str += std::to_string(contest_num) + "%";

    for(int i = 0; i != _contest_questions.size(); ++i){
        out_str += std::to_string(per_q_stats[i].first) + "," + std::to_string(per_q_stats[i].second) + ",";
    }
    out_str += "#";
    out_str += std::to_string(average_correct) + "%";
    out_str += std::to_string(max_correct) + "%";
    int tmp = run;
    out_str += std::to_string(tmp) + "%\n";
    std::ofstream file;
    file.open("contests.txt");
    if(!file.is_open()){
        return;
    }

    file << out_str;
    file.close();
}

std::string Contest::evaluate_contest(){
    std::string eval = std::to_string(contest_num) + "\t"+ std::to_string(total_questions) + " questions, ";
    if(!run){
        eval += "not run";
        return eval;
    }
    else{
        eval += "run, average correct: " + std::to_string(average_correct) + ";" + " maximum correct: " + std::to_string(max_correct) + "\n";
    }

    return eval;

}


std::string Contest::yoink(int socket){

    uint32_t length;
    read(socket, &length, sizeof(uint32_t));
    length = ntohl(length);
    //extract the size, create a buffer the appropriate size
    int size = length;
    char* msg;
    msg = new(std::nothrow) char[size + 1]();
    read(socket, msg, size);
    msg[size] = '\0';
    std::string rtn(msg);
    DEBUG("Yoinked " + rtn);
    delete [] msg;
    return rtn;
}

int Contest::yeet(std::string s, int socket){
    uint32_t length = htonl(s.length());
    send(socket, &length, sizeof(uint32_t), 0);
    return send(socket, s.c_str(), s.length(), 0);
}
