#include "contest.h"


bool Contest::add_question(Question* question){
    _contest_questions.push_back(question);
    total_questions = _contest_questions.size();
}

void Contest::list_contest(){
    std::string run_or_not = "";
    std::string totalq(total_questions);
    std::string c_num(contest_num);
    std::string result = "";
    if(run)
        run_or_not = "run";
    else
        run_or_not = "not run";

    result += c_num + "\t";
    result += totalq + " questions, ";
    result += run_or_not + ", ";
    result += "average correct: " + std::string(average_correct) + ";";
    result += "maximum correct: " + std::string(max_correct);
    std::cout << result << std::endl;
}

void Contest::run_contest(){
    std::thread(begin_contest()).detach();
}

//THIS FUNCTION IS DESIGNED TO RUN ON ITS OWN THREAD, IT REPRESENTS AN "ACTIVE" CONTEST
//its also a mess, i know, but i ran out of time
void Contest::begin_contest(){
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

   	std::cout << "Server::listening(): On Port: " << ntohs(address.sin_port) << std::endl;
    std::cout << "Server::listening(): Hostname: storm.cise.ufl.edu" << std::endl;

    //listen for new connections for 60 seconds
    std::chrono::stead_clock::timepoint start = std::chrono::steady_clock::now();
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
            sd = contestants[i];

            FD_SET(sd, &fds);
            if(sd > max_sd)
                max_sd = s;
        }

        activity = select(max_sd + 1, &fds, NULL, NULL, NULL);
        if(FD_ISSET(master_socket, &fds)){
            if((new_socket = accept(master_socket, (struct sockaddr*)& address, (socklen_t*)&addrlen)) < 0){
                std::cerr << "Contest::begin_contest: Failed to accept connection." << std::endl;
            }

            //add new socket to contestants
            Contestant tmp = new Contestant();
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
    auto AllResponded = [&bitmap](){ for (auto is_set : bitmap)
            if(is_set == false)
                return false;
            return true;
    };
    auto sendAll = [](std::vector<Contestant*>& peeps, std::string& msg){ for( auto peep : peeps) yeet(msg, peep->sock); };
    sendAll(contestants, "Please enter a nickname: ");
    /* TODO accept nicknames */

    //main contest loop
    for(int question = 0; question != _contest_questions.size(); ++question){
        sendAll(contestants, _contest_questions[question]->to_string_get());
        activity = select(max_sd + 1, &fds, NULL, NULL, NULL);
        while(!allResponded()){
            for(int i = 0; i != contestants.size(); ++i){
                if(FD_ISSET(contestants[i]->sock, &fds) && bitmap[i] == false){
                    contestants[i]->answer = yoink(contestants[i]->sock);
                    bitmap[i] = true;
                }
            }
            /* TODO check answers */
            /* TODO calc and send statistics back */
            /* TODO reset bitmap, responses, and "correct" in contestants */
        }
    }
    /* TODO update total stats for contest? (maybe after contest is over) */
}


std::string Server::yoink(int socket){

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

int Server::yeet(std::string s, int socket){
    uint32_t length = htonl(s.length());
    send(socket, &length, sizeof(uint32_t), 0);
    return send(socket, s.c_str(), s.length(), 0);
}
