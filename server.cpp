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
        std::string s= "Connected!";
        send_response(s);
        return true;
    }

}

bool Server::parse_input(){

    char* incoming_size = 0;
    int read_bytes = 0;

    //The first byte sent will be a number representing how many bytes the incoming message will be
    uint32_t length;
    int res = read(_connected_socket, &length, sizeof(uint32_t));
    if(res == 0){
        return false;
    }
    length = ntohl(length);
    //extract the size, create a buffer the appropriate size
    int size = length;
    char* msg;
    msg = new(std::nothrow) char[size + 1];
    msg[size] = '\0';
    
    //read the message
    read_bytes = read(_connected_socket, msg, size);
    
    //the first byte in the message will be the command
    char command = msg[0];
    //switch statement to handle commands
    switch(command){
        case 'k' : 
            delete [] msg; 
            throw std::runtime_error("Connection close by kill command");
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
            return true;
        case 'r' : 
            get_rand_question();
            delete [] msg;
            return true;
        case 'c' :
            check_answer(msg);
            delete [] msg;
            return true;
        default:
            std::string error = "Invalid command";
            uint32_t length = htonl(error.length());
            send(_connected_socket, &length, sizeof(uint32_t), 0);
            
            send(_connected_socket, error.c_str(), strlen(error.c_str()), 0);
            delete [] msg;
            return true;

    }
    
    
}

void Server::close_connection(){
    close(_connected_socket);
}

int Server::assign_val(){
    int* qnums;
    qnums = new int[_questions.size()];
    for(int i = 0; i != _questions.size(); ++i){
        
        qnums[i] = _questions[i]->get_question_num();
    
    }

    int* result = std::max_element<int*>(qnums, qnums + _questions.size());
    return *result + 1;
}

void Server::create_question(char* msg){
    std::string message(msg);
    message.erase(0, 2);
    int pos = 0;
    int question_num = assign_val();
    
    std::string delim = "\n";
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
    send_response(std::to_string(question_num));


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
    
}

int Server::index_of(int num){

    for(int i = 0; i != _questions.size(); ++i){
        if(num == _questions[i]->get_question_num())
            return i;
    }
    return -1;
}

void Server::delete_question(char* msg){
    std::string message(msg);
    message.erase(0, 2); //delete the command and following space since we already know it

    //read the question number to delete
    int q_num = std::stoi(message);

    int index = index_of(q_num);
    if(index == -1){
        std::string error = "Question " + std::to_string(q_num) + " does not exist!";
        send_response(error);
        return;
    }
    _questions.erase(_questions.begin() + index);
    std::string error = "Deleted Question " + std::to_string(q_num);
    send_response(error);
}

void Server::get_question(char* msg){
    std::string message(msg);
    message.erase(0, 2); //delete the command and following space since we already know it
    
    int q_num = std::stoi(message);

    int index = index_of(q_num);
    if(index == -1){
         std::string error = "Error: Question " + std::to_string(q_num) + " not found!";
         send_response(error);
         return;
    }

    std::string question = _questions[index]->to_string_get();

   
    
    send_response(question);
}

void Server::get_rand_question(){

    int rand_index = rand() % _questions.size();
    int index = index_of(rand_index);
    std::string question = _questions[rand_index]->to_string_rand();

    send_response(question);

    //wait for answer
    std::string ans = read_response();

    if(_questions[rand_index]->check_answer(ans.at(0))){
        std::string correct_message = "Correct!"; 
        send_response(correct_message);
    }
    else{
        std::string incorrect_message = "Incorrect!"; 
        send_response(incorrect_message);
    }

}

void Server::check_answer(char* msg){

    std::string message(msg);
    message.erase(0, 2); //delete the command and following space since we already know it
    std::string delim = " ";
    size_t pos = 0;

    pos = message.find(delim);
    int q_num = std::stoi(message.substr(0, pos));
    message.erase(0, 2);
    int index = index_of(q_num);
    if (index == -1){
        std::string error = "Error: Question " + std::to_string(q_num) + " not found!";
        send_response(error);
        return;
    }

    
    //wait for answer and send response
    char ans = message.at(0);
    
    if(_questions[index]->check_answer(ans)){
        std::string correct_message = "Correct!"; 
        send_response(correct_message);
    }
    else{
        std::string incorrect_message = "Incorrect!"; 
        send_response(incorrect_message);
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


    std::stringstream stream;

    

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
std::string Server::read_response(){

    uint32_t length;
    read(_connected_socket, &length, sizeof(uint32_t));
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

int Server::send_response(std::string s){
    uint32_t length = htonl(s.length());
    send(_connected_socket, &length, sizeof(uint32_t), 0);
    return send(_connected_socket, s.c_str(), s.length(), 0);
}