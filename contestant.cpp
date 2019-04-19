#include "contestant.h"

int Contestant::socket_init(const char* hostname, const char* port){

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

int Contestant::connecting(){
    //open connection with server
    if(connect(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Connection Failed!");
        return -1;
    }
}

int Contestant::resolve_hostname(const char* hostname, const char* port, std::string& ip){

    hostent* host = gethostbyname(hostname);
    if(host == NULL){
        return -1;
    }

    in_addr* address = (in_addr*)host->h_addr;
    ip = inet_ntoa(* address);
    return 0;

}

void Contestant::run_game(){

    //nickname stuff
    std::string answer;
    int num_questions = std::stoi(yoink());
    std::cout << yoink();
    std::cin >> answer;
    yeet(answer);
    std::cout << yoink();

    for(int i = 0; i != num_questions; ++i){
        try{
            std::cout << yoink();

            std::cin >> answer;
            yeet(answer);
            std::cout << yoink();
            if(i + 1 == num_questions){
                std::cout << yoink();
                return;
            }
        }
        catch (const std::exception& e){
            std::cout << e.what() << std::endl;
            return;
        }
    }
    std::cout << yoink();
}


std::string Contestant::yoink(){
    DEBUG("Contestant::yoink called");
    uint32_t length;
    int readBytes = read(_socket, &length, sizeof(uint32_t));
    if(readBytes <= 0){
        DEBUG("Contestant::yoink: size read was 0");
        throw new std::runtime_error("Connection closed");
    }
    length = ntohl(length);
    //extract the size, create a buffer the appropriate size
    int size = length;
    char* msg;
    msg = new(std::nothrow) char[size + 1]();
    msg[size] = '\0';
    readBytes = read(_socket, msg, size);
    if(readBytes <= 0){
        DEBUG("Contestant::yoink: size read was 0");
        throw new std::runtime_error("Connection closed");
    }
    if(readBytes != size){
        DEBUG("Contestant::Yoink wtf bro");
    }
    return msg;
    delete [] msg;
}

int Contestant::yeet(std::string s){
    DEBUG("Contestant::yeet called");
    DEBUG("YEETING " + s);
    uint32_t length = htonl(s.length());
    send(_socket, &length, sizeof(uint32_t), 0);
    return send(_socket, s.c_str(), s.length(), 0);
}