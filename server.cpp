#include "server.h"

Server::Server(){
    read_in_questions();
    socket_init();
}
Server::~Server(){
    if(_questions.size() == 0)
        remove("questions.txt");
    else{
        for(int i = 0; i != _questions.size(); ++i){
            if(i == 0)
                _questions[i]->write_out(true);
            else
                {
                    _questions[i]->write_out(false);
                }
        }
    }
    close_connection();
    close(_socket);
}

void Server::socket_init(){

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if( _socket < 0){
        throw std::runtime_error("Server::socket_init(): Failed to create TCP socket file descriptor.");
    }

    int opt = 1;
    int addrlen = sizeof(address);

    //set socket options
    if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        throw std::runtime_error("Server::socket_init(): Failed to set socket options.");
    }

    //set address options
    address.sin_family = AF_INET;
    address.sin_port = htons(0);
    address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket
    if(bind(_socket, (struct sockaddr *)&address, sizeof(address) ) < 0){
        throw std::runtime_error("Server::socket_init(): Failed to bind socket.");
    }
}

bool Server::listening(){

    //Open the socket for listening, only allow 1 connection at a time.
    if(listen(_socket, 1) < 0){
        throw std::runtime_error("Server::listen(): Failed to listen on socket.");
    }

    int addrlen = sizeof(address);
    //accept a connection so that I can read the input
    struct sockaddr_in new_address;
    int size = sizeof(new_address);
    getsockname(_socket, (struct sockaddr*)&new_address, (socklen_t*) &size);
   	std::cout << "Server::listening(): On Port: " << ntohs(new_address.sin_port) << std::endl;
    std::cout << "Server::listening(): Hostname: storm.cise.ufl.edu" << std::endl;

    _connected_socket = accept(_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if(_connected_socket < 0){
        throw std::runtime_error("Server::listen(): Failed to accept connection.");
    }
    else{
        std::string s = "Connected!";
        yeet(s);
        return true;
    }

}

bool Server::parse_input(){

    std::string msg = yoink();
    DEBUG("Message server side: " + msg);
    if(msg.size() <= 0){
        DEBUG("Empty message");
        return true;
    }
    //the first byte in the message will be the command
    char command = msg.at(0);
    if (!command){
        return true;
    }
    //switch statement to handle commands
    switch(command){
    case 'k' :
        throw std::runtime_error("Connection close by kill command");
    case 'p' :
        create_question(msg);
        return true;
    case 'd' :
        delete_question(msg);
        return true;
    case 'g' :
        get_question(msg);
        return true;
    case 'r' :
        review(msg);
        return true;
    case 'c' :
        check_answer(msg);
        return true;
    case 's':
        set_contest(msg);
        return true;
    case 'a':
        add_q_contest(msg);
        return true;
    case 'b':
        begin_contest(msg);
        return true;
    case 'l':
        list_contests(msg);
        return true;
    default:
        yeet("Invalid Command!");
        return true;

    }
    DEBUG("Some how made it here");
}

void Server::set_contest(std::string message){
    message.erase(0,2);

    uint8_t c_num = (uint8_t)std::stoi(message);

    DEBUG("Got here");
    _contests.push_back(new Contest(c_num));
    DEBUG("Made the new contest");
    if(index_of_contest(c_num) != -1){
        yeet("Error: Contest " + std::to_string(c_num) + " already exists");
        return;
    }
    yeet("Contest " + std::to_string(c_num) + " is set");

}

void Server::add_q_contest(std::string message){
    
    message.erase(0,2);
    unsigned int pos = message.find(" ");
    unsigned int c_num = std::stoi(message.substr(0, pos));
    int c_index = index_of_contest(c_num);
    if(c_index < 0){
        yeet("Error: Contest " + std::to_string(c_num) + " does not exist!");
        return;
    }
    message.erase(0, pos + 1);
    pos = message.find(" ");
    unsigned int q_num = std::stoi(message.substr(0,pos));
    int q_index = index_of(q_num);
    if(q_index < 0){
        yeet("Error: Question number " + std::to_string(q_num) + " does not exist!");
        return;
    }
    _contests[c_index]->add_question(_questions[q_index]);
    yeet("Question " + std::to_string(q_num) + " added to contest " + std::to_string(c_num));
}

void Server::begin_contest(std::string message){
    
    message.erase(0,2);
    int c_num = std::stoi(message);
    if(index_of_contest(c_num) < 0){
        yeet("Error: Contest " + std::to_string(c_num) + " does not exist");
        return;
    }
    _contests[c_num]->run_contest();
    yeet("Contest " + std::to_string(c_num) + " started");
}

void Server::list_contests(std::string message){
    if(_contests.size() <= 0){
        yeet("No Contests!");
        return;
    }
    for (int i = 0; i != _contests.size(); ++i){
        yeet(_contests[i]->list_contest());
    }
}
void Server::close_connection(){
    close(_connected_socket);
}

//int Server::assign_val(){
//    int* qnums;
//    qnums = new int[_questions.size()];
//    for(int i = 0; i != _questions.size(); ++i){
//
//        qnums[i] = _questions[i]->get_question_num();
//
//    }
//
//    int* result = std::max_element<int*>(qnums, qnums + _questions.size());
//    return *result + 1;
//}

void Server::create_question(std::string message){
    std::string delim = "%";
    
    message.erase(0, 2);
    int pos = 0;
    pos = message.find(delim);
    int question_num = std::stoi(message.substr(0, pos));
    message.erase(0, pos + delim.length());
    //this function returns -1 if it couldnt find the value. This is used to test if the question already exists
    if(index_of(question_num) != -1){
        yeet("Error: question number " + std::to_string(question_num) + " already used");
    }

    pos = message.find(delim);
    std::string question_tag = message.substr(0, pos);
    message.erase(0, pos + delim.length());

    delim = "%";
    pos = message.find(delim);
    std::string question_text = message.substr(0, pos);
    message.erase(0, pos + delim.length());

    pos = message.find(delim);
    std::string choices = message.substr(0, pos);
    message.erase(0, pos + delim.length());

    pos = message.find(delim);
    std::string correct_answer = message.substr(0, pos);


    std::vector<std::string> question_choices;

    int dot_count = 0;
    for (int i = 0; i != choices.size(); ++i){
        if(choices.at(i) == '.')
            ++dot_count;
    }

    std::string tmp;
    delim = ".";
    for(int i = 0; i != dot_count; ++i){
        pos = choices.find(delim);
        tmp = choices.substr(0, pos);
        //tmp.erase(tmp.length()- 1, 1);
        question_choices.push_back(tmp);
        choices.erase(0, pos + delim.length());

    }
    _questions.push_back( new Question(question_num, question_tag, question_text, question_choices, correct_answer.at(0)));
    yeet("Question " + std::to_string(question_num) + " added");
}

int Server::index_of(int num){

    for(int i = 0; i != _questions.size(); ++i){
        if(num == _questions[i]->get_question_num())
            return i;
    }
    return -1;
}

int Server::index_of_contest(int num){
    if(_contests.size() <= 0)
        return -1;
    for(int i = 0; i != _contests.size(); ++i){
        if(num == _contests[i]->get_contest_num())
            return i;
    }
    return -1;
}

void Server::delete_question(std::string message){
    message.erase(0, 2); //delete the command and following space since we already know it

    //read the question number to delete
    int q_num = std::stoi(message);

    int index = index_of(q_num);
    if(index == -1){
        std::string error = "Question " + std::to_string(q_num) + " does not exist!";
        yeet(error);
        return;
    }
    _questions.erase(_questions.begin() + index);
    std::string error = "Deleted Question " + std::to_string(q_num);
    yeet(error);
}

void Server::get_question(std::string message){
    
    message.erase(0, 2); //delete the command and following space since we already know it

    int q_num = std::stoi(message);

    int index = index_of(q_num);
    if(index == -1){
        std::string error = "Error: Question " + std::to_string(q_num) + " not found";
        yeet(error);
        return;
    }

    std::string question = _questions[index]->to_string_get();

    yeet(question);
}

void Server::review(std::string message){
    message.erase(0,2);

    int c_num = std::stoi(message);
    int index = index_of_contest(c_num);
    if(index < 0){
        std::string error = "Error: Contest " + std::to_string(c_num) + " not found";
        yeet(error);
        return;
    }

    std::string analytics = _contests[index]->evaluate_contest();
    yeet(analytics);

}
//void Server::get_rand_question(){
//
//    int rand_index = rand() % _questions.size();
//    int index = index_of(rand_index);
//    std::string question = _questions[rand_index]->to_string_rand();
//
//    yeet(question);
//
//    //wait for answer
//    std::string ans = yoink();
//
//    if(_questions[rand_index]->check_answer(ans.at(0))){
//        std::string correct_message = "Correct!";
//        yeet(correct_message);
//    }
//    else{
//        std::string incorrect_message = "Incorrect!";
//        yeet(incorrect_message);
//    }
//
//}

void Server::check_answer(std::string message){

    
    message.erase(0, 2); //delete the command and following space since we already know it
    std::string delim = " ";
    size_t pos = 0;

    pos = message.find(delim);
    int q_num = std::stoi(message.substr(0, pos));
    message.erase(0, 2);
    int index = index_of(q_num);
    if (index == -1){
        std::string error = "Error: Question " + std::to_string(q_num) + " not found!";
        yeet(error);
        return;
    }

    //wait for answer and send response
    char ans = message.at(0);

    if(_questions[index]->check_answer(ans)){
        std::string correct_message = "Correct!";
        yeet(correct_message);
    }
    else{
        std::string incorrect_message = "Incorrect!";
        yeet(incorrect_message);
    }
}

bool Server::index_valid(int index){
    if (index < 0 || index > _questions.size()){
        return false;
    }
    else{
        return true;
    }
}

void Server::read_in_questions(){
    //in file the contents are stored in the order q_num\n, ans\n, q_tag\n, q_text\n., q_choice\n.\n.
    std::ifstream file ("questions.txt");

    if(!file.is_open())
        return;


    //std::stringstream stream;

    //Question(question_num, question_tag, question_text, question_choices, correct_answer)
    std::string tmp;
    while(getline(file, tmp)){

        int question_num;


        std::string question_tag;

        std::string question_text;

        std::vector<std::string> question_choices;

        char correct_answer;

        char delim = '.';

        //get question number

        //getline(file, tmp);
        //std::cout << "Num: " << tmp << std::endl;
        question_num = std::stoi(tmp);

        getline(file, tmp);
        correct_answer = tmp.at(0);
        //std::cout << "ans: " << correct_answer << std::endl;

        //get question tagThere are two n
        getline(file, question_tag);
        //std::cout << "tag: " <<question_tag << std::endl;
        //get question text
        getline(file, question_text, delim);
        question_text.erase(question_text.length()- 1, 1);
        //std::cout << "text: " <<question_text << std::endl;

        int i = 0;
        while(true){

            getline(file, tmp, delim);
            //std::cout << tmp.length() << std::endl;
            if(tmp.length() == 1 || tmp.length() == 2)
                break;

            tmp.erase(0, 1);
            tmp.erase(tmp.length()- 1, 1);
            question_choices.push_back(tmp);
        }
        getline(file, tmp);

        _questions.push_back( new Question(question_num, question_tag, question_text, question_choices, correct_answer));

    }
}
std::string Server::yoink(){
    uint32_t length;
    int readBytes = read(_connected_socket, &length, sizeof(uint32_t));
    length = ntohl(length);
    //extract the size, create a buffer the appropriate size
    int size = length;
    char* msg;
    msg = new(std::nothrow) char[size + 1]();
    read(_connected_socket, msg, size);
    msg[size] = '\0';
    std::string rtn(msg);
    delete [] msg;
    return rtn;
}

int Server::yeet(std::string s){
    uint32_t length = htonl(s.length());
    send(_connected_socket, &length, sizeof(uint32_t), 0);
    return send(_connected_socket, s.c_str(), s.length(), 0);
}
