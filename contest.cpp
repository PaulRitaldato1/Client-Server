#include "contest.h"

Contest::Contest(uint8_t contest_num, double average_correct, int max_correct, std::vector<int> q_nums, std::vector<Question*>& all_questions){
    this->contest_num = contest_num;
    this->average_correct = average_correct;
    this->max_correct = max_correct;

    for(int i : q_nums){
        int index = 0;
        for (int k = 0; k != all_questions.size(); ++k){
            if(i = all_questions[k]->get_question_num()){
                index = k;
                break;
            }
        }
        _contest_questions.push_back(all_questions[index]);
    }
}
bool Contest::add_question(Question* question){
    _contest_questions.push_back(question);
    total_questions = _contest_questions.size();
}

void Contest::list_contest(){
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
    std::cout << result << std::endl;
}

void Contest::run_contest(){
    std::thread(begin_contest()).detach();
}

//THIS FUNCTION IS DESIGNED TO RUN ON ITS OWN THREAD, IT REPRESENTS AN "ACTIVE" CONTEST
//its also a mess, i know, but i ran out of time
void Contest::begin_contest(){
    run = true;
    int new_socket, activity, sd, max_sd;
    fd_set fds;
    std::vector<Contestant*> contestants;
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
    address.sin_port = htons(0);
    address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket
    if(bind(master_socket, (struct sockaddr *)&address, sizeof(address) ) < 0){
        throw std::runtime_error("Server::socket_init(): Failed to bind socket.");
    }
    /* =================================== Set up master Socket end ===============================*/

    if(listen(master_socket, 3) < 0)
        throw std::runtime_error("Contest::begin_contest: Failed to listen on master socket.");

   	std::cout << "Contest::begin_contest(): On Port: " << ntohs(address.sin_port) << std::endl;
    std::cout << "Contest::begin_contest(): Hostname: storm.cise.ufl.edu" << std::endl;

    //listen for new connections for 60 seconds
    std::chrono::steady_clock::timepoint start = std::chrono::steady_clock::now();
    while(true){

        //stop allowing connections after 60 seconds
        if(std::chrono::steady_clock::now() - start > std::chrono::seconds(60)){
            DEBUG("Done listening for connections");
            break;
        }
        //clear socket set
        FD_ZERO(&fds);

        //add master socket to the socket set
        FD_SET(master_socket, &fds);

        max_sd = master_socket;

        for (int i = 0; i != contestants.size(); ++i){
            sd = contestants[i]->sock;

            FD_SET(sd, &fds);
            if(sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &fds, NULL, NULL, NULL);
        if(FD_ISSET(master_socket, &fds)){
            if((new_socket = accept(master_socket, (struct sockaddr*)& address, (socklen_t*)&addrlen)) < 0){
                std::cerr << "Contest::begin_contest: Failed to accept connection." << std::endl;
            }

            //add new socket to contestants
            Contestant* tmp = new Contestant();
            tmp->sock = new_socket;
            contestants.push_back(tmp);
        }

    }

    if(contestants.size() == 0){
        DEBUG("No contestants connected after 60 seconds, terminating..");
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
    auto checkAnswers = [&contestants, &this](int q_num){ for(auto contestant : contestants)
        if(contestant->answer == _contest_questions[q_num]->get_answer())
            contestant->correct = true;
        else
            contestant->correct = false;
        };
    auto reset = [&bitmap, &contestants]()mutable{ for (int i = 0; i != bitmap.size(); ++i) {
            bitmap[i] = false;
            contestants[i]->correct = false;
            contestants[i]->answer = 0;
            }
        };
    auto allResponded = [&bitmap](){
        for (auto is_set : bitmap)
            if(is_set == false)
                return false;
        return true;
    };
    auto sendAll = [](std::vector<Contestant*>& peeps, std::string& msg){ for( auto peep : peeps) yeet(msg, peep->sock); };
    auto checkNickname = [&contestants](std::string& nickname){
        for(int i = 0; i != contestants.size(); ++i){
            if(contestants[i]->nickname == nickname)
                return false;
        }
           return true;
    };

    sendAll(contestants, "Please enter a nickname: ");
    /* TODO accept nicknames */
    while(!allResponded()){
        activity = select(max_sd + 1, &fds, NULL, NULL, NULL);
        for(int i = 0; i != contestants.size(); ++i){
            if(FD_ISSET(contestants[i]->sock, &fds) && bitmap[i] == false){
                std::string tmp = yoink(contestants[i]->sock);
                if(!checkNickname(tmp)){
                    yeet("Error: Nickname " + tmp + " is already in use.", contestants[i]->sock);
                }
                else{
                    contestants[i]->nickname = tmp;
                    bitmap[i] = true;
                }
            }
        }
    }

    //main contest loop
    int tmp_max_correct = 0;
    for(int question = 0; question != _contest_questions.size(); ++question){
        sendAll(contestants, _contest_questions[question]->to_string_rand());
        activity = select(max_sd + 1, &fds, NULL, NULL, NULL);
        while(!allResponded()){
            for(int i = 0; i != contestants.size(); ++i){
                if(FD_ISSET(contestants[i]->sock, &fds) && bitmap[i] == false){
                    contestants[i]->answer = yoink(contestants[i]->sock);
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
        for (int i = 0; i != contestants.size(); ++i){
            if(contestants[i]->correct){
                contestants[i]->num_correct = contestants[i]->num_correct+1;
                if(contestants[i]->num_correct > top_score)
                    top_score = contestants[i]->num_correct;
            }
        }

        //sending responses to each contestant
        per_q_percent = ((double)tmp/(double)contestants.size()) * 100;
        std::string response = "";
        for(int i = 0; i != contestants.size(); ++i){
            if(contestants[i]->correct)
                response += "Correct. ";
            else
                response += "Incorrect. ";
            response += std::to_string(per_q_percent) + " of contestants answered this question correctly.\n";
            response += "Your score is " + std::to_string(num_correct) + "/" + std::to_string(question) + "." + " The top score is currently " + std::to_string(top_score) + "/" + std::to_string(question) + ".";

            yeet(response, contestants[i]->sock);
        }
        /* TODO reset bitmap, responses, and "correct" in contestants */
        reset();
    }
    /* TODO update total stats for contest? (maybe after contest is over) */
    double avg_correct = 0.0;
    for(int i =0; i != contestants.size(); ++i){
        if(contestants[i]->num_correct > max_correct)
            max_correct = contestants[i]->num_correct;
        avg_correct += (double)contestants[i]->num_correct/(double)_contest_questions().size();
    }
    avg_correct = (avg_correct/contestants.size())*100;
    average_correct = avg_correct;

    for(int i = 0; i != contestants.size(); ++i){
        std::string message = "The contest is over -- thanks for playing " + contestants[i]->nickname + "!";
    }
}


void Contest::write_out(){

    std::string out_str = "";
    //cotnest, questions, average_correct, max_correct
    out_str += std::to_string(contest_num) + "%";

    for(int i = 0; i != _contest_questions.size(); ++i){
        out_str += std::to_string(_contest_questions[i]->get_question_num()) + "%%";
    }
    out_str += std::to_string(average_correct) + "%";
    out_str += std::to_string(max_correct) + "%";
    out_str += std::to_string(run) + "%";
    std::ofstream file;
    file.open("contests.txt");
    if(!file.open()){
        return;
    }

    file << out_str;
    file.close();
}

std::string Contest::evaluate_contest(){
    std::string eval = std::to_string(total_questions) + " questions, ";
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
    delete [] msg;
    return rtn;
}

int Contest::yeet(std::string s, int socket){
    uint32_t length = htonl(s.length());
    send(socket, &length, sizeof(uint32_t), 0);
    return send(socket, s.c_str(), s.length(), 0);
}
