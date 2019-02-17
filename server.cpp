#include "connection.h"

Server::Server(){
    read_in_questions();
    socket_init();
}
Server::~Server(){

    for(int i = 0; i != _questions.size() - 1; ++i){
            _questions[i].write_out();
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
	std::cout << "Server::listening(): With Hostname: storm.cise.ufl.edu" << std::endl;

	_connected_socket = accept(_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if(_connected_socket < 0){
		throw std::runtime_error("Server::listen(): Failed to accept connection.");
	}
    else{
        return true;
    }

}

bool Server::parse_input(){

    char* incoming_size = 0;
    int read_bytes = 0;

    //The first byte sent will be a number representing how many bytes the incoming message will be
    if(read(_connected_socket, incoming_size, 1))
        throw std::runtime_error("Server::parse_input(): Failed to read size of incoming message");
    
    //extract the size, create a buffer the appropriate size
    int size = atoi(incoming_size);
    char* msg;
    msg = new(std::nothrow) char[size];

    
    //read the message
    read_bytes = read(_connected_socket, msg, size);
    if(read_bytes != size)
        throw std::runtime_error("Server::parse_input(): Failed to read the entire message");
    
    //the first byte in the message will be the command
    char command = msg[0];

    //switch statement to handle commands
    switch(command){
        case 'k' : 
            delete [] msg; 
            return false;
        case 'p' : 
            create_question(msg);
            delete [] msg;
            return true;
        case 'd' :
            delete_question(msg);
            delete [] msg;
            return true;
        case 'g' :
            get_question(msg);
            delete [] msg;
        case 'r' : 
            get_rand_question();
            delete [] msg;
            return true;
        case 'c' :
            check_answer(msg);
            delete [] msg;
            return true;
        default:
            return false;

    }
    
    
}

void Server::close_connection(){
    close(_connected_socket);
}

void Server::create_question(char* msg){
    std::string message(msg);

    message.erase(0, 1); //delete the command since we already know it

    size_t pos = 0;
    int question_num;
    std::string question_tag;
    std::string question_text;
    std::vector<std::string> question_choices;
    char correct_answer;

    std::string delim = "\n";
    question_num = _questions[_questions.size()-1].get_question_num() + 1;

    pos = message.find(delim);
    question_tag = message.substr(0, pos);
    message.erase(0, pos + delim.length());

    delim = ".";
    pos = message.find(delim);
    question_text = message.substr(0, pos);
    message.erase(0, pos + delim.length());

    std::string test_str = "";
    pos = message.find(delim);
    test_str = message.substr(0, pos);
    while(test_str.compare("\n.") != 0){

        question_choices.push_back(test_str);
        message.erase(0, pos + delim.length());
        pos = message.find(delim);
        test_str = message.substr(0, pos);

    }

    message.erase(0, pos + delim.length() + 1);

    delim = "\n";
    pos = message.find(delim);
    correct_answer = message.substr(0, pos).at(0);

    _questions.push_back( Question(question_num, question_tag, question_text, question_choices, correct_answer));
}

void Server::delete_question(char* msg){
    std::string message(msg);
    message.erase(0, 2); //delete the command and following space since we already know it

    //read the question number to delete
    int q_num = std::stoi(message);

    _questions.erase(_questions.begin() + (q_num - 1));
}

void Server::get_question(char* msg){
    std::string message(msg);
    message.erase(0, 2); //delete the command and following space since we already know it
    
    int q_num = std::stoi(message);
    if (!index_valid(q_num)){
        std::string error = "Error: Question " + std::to_string(q_num) + " not found!";
        send(_connected_socket, error.c_str(), strlen(error.c_str()), 0);
        return;
    }

    std::string question = _questions[q_num - 1].to_string();

    int size_to_send = strlen(question.c_str());
    send(_connected_socket, std::to_string(size_to_send).c_str(), 1, 0);
    //send the message
    send(_connected_socket, question.c_str(), strlen(question.c_str()), 0);

    //wait for answer
    check_ans_help(q_num);
}

void Server::get_rand_question(){

    int rand_index = rand() % _questions.size();

    std::string question = _questions[rand_index].to_string();

    //send the size of the incoming message first
    int size_to_send = strlen(question.c_str());
    send(_connected_socket, std::to_string(size_to_send).c_str(), 1, 0);
    //send the message
    send(_connected_socket, question.c_str(), strlen(question.c_str()), 0);

    //wait for answer
    check_ans_help(rand_index);

}

void Server::check_answer(char* msg){

    std::string message(msg);
    message.erase(0, 2); //delete the command and following space since we already know it
    std::string delim = " ";
    size_t pos = 0;

    pos = message.find(delim);
    int q_num = std::stoi(message.substr(0, pos));
    
    if (!index_valid(q_num)){
        std::string error = "Error: Question " + std::to_string(q_num) + " not found!";
        send(_connected_socket, error.c_str(), strlen(error.c_str()), 0);
        return;
    }

    //wait for answer and send response
    check_ans_help(q_num);
}

bool Server::index_valid(int index){
    if (index < 0 || index > _questions.size()){
        return false;
    }
    else{
        return true;
    }
}

void Server::check_ans_help(int q_num){

    //wait for answer
    char ans;
    read(_connected_socket, &ans, 1);

    if(_questions[q_num - 1].check_answer(ans)){
        const char* correct_message = "Correct!"; 
        send(_connected_socket, correct_message, strlen(correct_message), 0);
    }
    else{
        const char* incorrect_message = "Incorrect!"; 
        send(_connected_socket, incorrect_message, strlen(incorrect_message), 0);
    }

}

void Server::read_in_questions(){




}
