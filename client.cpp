#include "client.h"
int Contestmeister::socket_init(const char* hostname, const char* port){
    //create socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket < 0){
        std::cerr << "Failed to create socket!" << std::endl;
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    std::string ip = "";
    unsigned short PORT = atoi(port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(resolve_hostname(hostname, port, ip) < 0){
        std::cerr << "Failed to resolve hostname " << hostname << std::endl;
        return -1;
    }
    //convert ipv4 to binary data
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) < 0){
        printf("Could not use ip address provided!");
        return -1;
    }
}

int Contestmeister::connecting(){
    //open connection with server
    if(connect(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Connection Failed!");
        return -1;
    }
    read_response();
}

void Contestmeister::close_connection(){
    close(_socket);
}

int Contestmeister::resolve_hostname(const char* hostname, const char* port, std::string& ip){

    hostent* host = gethostbyname(hostname);
    if(host == NULL){
        return -1;
    }
    in_addr* address = (in_addr*)host->h_addr;
    ip = inet_ntoa(* address);
    return 0;
}

int Contestmeister::parse_input(filename){

    std::string commands;
    std::istream input;
    if(!filename.empty()){
        //read from file
        input = std::ifstream f(filename);
        //commands = std::string( (std::istreambuf_iterator _iterator<char>(f)), (std::istreambuf_iterator<char>()));
    }
    else{
        input = std::cin;
    }

    bool run_client = true;
    while(run_client){
        char command = 0;
        std::cout << "> ";
        std::cin >> command;
        switch(command){
        case 'k':
            if (kill() == -1)
                run_client = false;
            run_client = false;
            break;
        case 'p':
            if(put_q() == -1)
                run_client = false;
            break;
        case 'q' :
            close_connection();
            run_client = false;
            break;
        case 'h' :
            help();
            break;
        case 'd' :
            if(delete_q() == -1)
                run_client = false;
            break;
        case 'g' :
            if(get() == -1)
                run_client = false;
            break;
        case 'r' :
            if(random() == -1)
                run_client = false;
            break;
        case 'c' :
            if(check_answer() == -1)
                run_client = false;
            break;
        case 's':
            if(set_contest())
                run_client = false;
        case 'a':
            if(add_question())
                run_client = false;
        case 'b':
            if(begin_contest())
                run_client = false;
        case 'l':
            if(list_contests())
                run_client = false;
        default :
            continue;
        }
    }
    return 1;
}

int Contestmeister::list_contests(){
    std::string command = "l";
    if (send_response(command))
        return -1;
    read_response();
}
int Contestmeister::being_contest(){
    std::string command = "b";
    std::string c_num;
    std::cin >> c_num;


    if (send_response(command + " " + c_num))
        return -1;
    read_response();
}
int Contestmeister::add_question(){
    std::string command = "a";
    std::string c_num, q_num;
    std::cin >> c_num;
    std::cin >> q_num;

    if (send_response(command + " " + c_num + " " + q_num))
        return -1;

    read_response();
}

int Contestmeister::set_contest(){
    std::string command = "s";
    std::string number;
    std::cin >> number;
    if(send_response(command + " " + number))
        return -1;
    read_response();
}

void Contestmeister::help(){
    std::string help_message = "Here are the commands you can use:\np - put question on server\nd<n> - delete question <n> from bank\ng<n> - get question <n> from bank\nr - get random question from bank\nc <n> <x> - check answer <x> of question <n>\nk - kill the server\nq - quit the client\nh - print this help message";
    std::cout << help_message << std::endl;
}

int Contestmeister::kill(){
    //send kill message
    std::string command = "k";
    return send_response(command);
}

int Contestmeister::check_answer(){
    std::string command = "c";
    std::string number;
    std::cin >> number;
    std::string ans;
    std::cin >> ans;

    std::string s = command + " " + number + " " + ans;
    if(send_response(s) == -1)
        return -1;
    read_response();
}

int Contestmeister::random(){
    std::string command = "r";
    if(send_response(command) == -1)
        return -1;
    read_response();
    std::string choice;
    std::cout << "answer is: ";
    std::cin >> choice;
    if(choice.at(0) >= 'a' && choice.at(0) <= 'd'){
        if(send_response(choice) == -1)
            return -1;
    }
    else{
        std::cout << "Invalid input, try any char a,b,c,d" << std::endl;
    }
    read_response();
}

int Contestmeister::get(){
    std::string command = "g";
    std::string number = "";
    std::cin >> number;
    std::string s = command + " " + number;
    if(send_response(s) == -1)
        return -1;
    read_response();
}

int Contestmeister::delete_q(){
    std::string command = "d";
    std::string number = "";
    std::cin >> number;
    std::string s = command + " " + number;
    if(send_response(s) == -1)
        return -1;
    read_response();
}

int Contestmeister::put_q(){
    std::cout << "put a question in the bank" << std::endl;
    std::string tag;
    getline(std::cin, tag);
    std::string text = "";
    getline(std::cin, text, '.');
    text.erase(text.length()-1, 1);
    std::string tmp, choices;
    std::cout << "Choice: ";
    int i;
    for(i = 0; i != 5; ++i){
        getline(std::cin, tmp, '.');
        if(tmp.size() == 1 || tmp.size() == 2)
            break;
        tmp.back() = '.';
        tmp.erase(0, 1);
        choices += tmp;
    }
    if(i < 2){
        std::cout << "You didnt input enough choices! Try inputing the question again!" << std::endl;
        return 0;
    }
    std::string answer;
    std::cin >> answer;
    std::string result = "p%";
    result += tag;
    result += text + "%";
    result += choices + "%";
    result += answer + "%";
    //std::cout << result;
    send_response(result);
    read_response();
}

void Contestmeister::read_response(){

    uint32_t length;
    read(_socket, &length, sizeof(uint32_t));
    length = ntohl(length);
    //extract the size, create a buffer the appropriate size
    int size = length;
    char* msg;
    msg = new(std::nothrow) char[size + 1]();
    msg[size] = '\0';
    read(_socket, msg, size);
    std::cout << msg << std::endl;
    delete [] msg;
}

int Contestmeister::send_response(std::string s){
    uint32_t length = htonl(s.length());
    send(_socket, &length, sizeof(uint32_t), 0);
    return send(_socket, s.c_str(), s.length(), 0);
}
