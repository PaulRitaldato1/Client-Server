#include "contestmeister.h"
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
    yoink();
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

int Contestmeister::parse_input(std::string filename){

    if(!filename.empty()){
        //read from file
        std::ifstream f(filename);
        command_control(f);
    }
    command_control(std::cin);
}

int Contestmeister::command_control(std::istream& stream){
    bool file = true;
    bool run_client = true;

    if(&stream == &std::cin){
        std::cout << "not file" << std::endl;
        file = false;
    }

    while(run_client){
        std::string command = "";
        //if(file = false)
        //    std::cin.clear();
        std::cout << "\r> ";
        getline(stream,command);
        //std::cin >> command; 
        if(command.empty()){
            if(file)
                return 1;
            DEBUG("CM::input: empty command");
            continue;
        }
        switch(command.at(0)){
        case 'k':
            if (kill() == -1)
                run_client = false;
            run_client = false;
            break;
        case 'p':
            if(put_q(stream, command) == -1)
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
            if(delete_q(command) == -1)
                run_client = false;
            break;
        case 'g' :
            if(get(command) == -1)
                run_client = false;
            break;
        case 'r' :
            if(random(command) == -1)
                run_client = false;
            break;
        case 'c' :
            if(check_answer(command) == -1)
                run_client = false;
            break;
        case 's':
            if(set_contest(command) == -1)
                run_client = false;
            break;
        case 'a':
            if(add_question(command) == -1)
                run_client = false;
            break;
        case 'b':
            if(begin_contest(command) == -1)
                run_client = false;
            break;
        case 'l':
            if(list_contests(command) == -1)
                run_client = false;
            break;
        default :
            break;
        }
    }
    return 1;
}

int Contestmeister::list_contests(std::string message){
    //std::string command = "l";
    if (yeet(message) == -1)
        return -1;
    yoink();
    return 0;
}

int Contestmeister::begin_contest(std::string message){

    if (yeet(message) == -1)
        return -1;
    yoink();
    return 0;
}
int Contestmeister::add_question(std::string message){
    //    std::string command = "a";
    //    message.erase(0,2);
    //    std::string delim = " ";
    //    int pos = message.find(delim);
    //    std::string c_num = message.substr(0, pos);
    //    message.erase(0, pos + delim.length());
    //    delim = "\n";
    //    pos = message.find(delim);
    //    std::string q_num = message.substr(0, pos);

    if (yeet(message) == -1)
        return -1;

    yoink();
    return 0;
}

int Contestmeister::set_contest(std::string message){
    // std::string command = "s";
    // message.erase(0, 2);
    // std::string number = message.substr(0, message.find("\n"));
    int tmp = yeet(message);
    if(tmp == -1)
        return -1;
    yoink();
    return 0;
}

void Contestmeister::help(){
    std::string help_message = "Here are the commands you can use:\np - put question on server\nd<n> - delete question <n> from bank\ng<n> - get question <n> from bank\nr - get random question from bank\nc <n> <x> - check answer <x> of question <n>\nk - kill the server\nq - quit the client\nh - print this help message";
    std::cout << help_message << std::endl;
}

int Contestmeister::kill(){
    //send kill message
    std::string command = "k";
    return yeet(command);
}

int Contestmeister::check_answer(std::string message){
    //std::string command = "c";
    //message.erase(0, 2);
    //int pos = message.find(" ");
    //std::string number = message.substr(0, pos);
    //message.erase(0, pos + " ".length());
    //std::string ans = message.substr(0, message.find("\n"));

    //std::string s = command + " " + number + " " + ans;
    if(yeet(message) == -1)
        return -1;
    yoink();
}

int Contestmeister::random(std::string message){
    std::string command = "r";
    if(yeet(message) == -1)
        return -1;
    //yoink();
    //std::string choice;
    //std::cout << "What is your answer choice: ";
    //stream >> choice;
    //if(choice.at(0) >= 'a' && choice.at(0) <= 'd'){
    //    if(yeet(choice) == -1)
    //        return -1;
    //}
    //else{
    //    std::cout << "Invalid input, try any char a,b,c,d" << std::endl;
    //}
    yoink();
    return 0;
}

int Contestmeister::get(std::string message){
    //std::string command = "g";
    //message.erase(0,2);
    //std::string number = message.substr(0, message.find("\n"));
    //std::string s = command + " " + number;
    if(yeet(message) == -1)
        return -1;
    yoink();
    return 0;
}

int Contestmeister::delete_q(std::string message){
    //std::string command = "d";
    //message.erase(0,2);
    //std::string number = message.substr(0, message.find("\n"));
    //std::string s = command + " " + number;
    if(yeet(message) == -1)
        return -1;
    yoink();
    return 0;
}

int Contestmeister::put_q(std::istream& stream, std::string message){
    std::cout << "put a question in the bank" << std::endl;
    message.erase(0,2);
    std::string tag;
    std::string q_num;
    std::istringstream tempstream(message);
    getline(tempstream, q_num, ' ');
    std::string text = "";
    getline(stream, tag);
    getline(stream, text, '.');
    text.erase(text.length()-1, 1);
    std::string tmp, choices;
    std::cout << "Choice: ";
    int i;
    for(i = 0; i != 5; ++i){
        getline(stream, tmp, '.');
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
    stream >> answer;
    std::string result = "p%";
    result += q_num + "%";
    result += tag + "%";
    result += text + "%";
    result += choices + "%";
    result += answer + "%";
    //std::cout << result;
    yeet(result);
    yoink();
    return 0;
}

void Contestmeister::yoink(){
    DEBUG("CM::yoink called");
    uint32_t length;
    int read1 = read(_socket, &length, sizeof(uint32_t));
    if(read1 == 0){
        DEBUG("CM::yoink: size read was 0");
        throw new std::runtime_error("asdfadf");
    }
    length = ntohl(length);
    //extract the size, create a buffer the appropriate size
    int size = length;
    char* msg;
    msg = new(std::nothrow) char[size + 1]();
    msg[size] = '\0';
    int readBytes = read(_socket, msg, size);
    if(readBytes != size){
        DEBUG("CM::Yoink wtf bro");
    }
    std::cout << "CM::Yoink: " << msg << std::endl;
    delete [] msg;
}

int Contestmeister::yeet(std::string s){
    DEBUG("CM::yeet called");
    uint32_t length = htonl(s.length());
    send(_socket, &length, sizeof(uint32_t), 0);
    return send(_socket, s.c_str(), s.length(), 0);
}
