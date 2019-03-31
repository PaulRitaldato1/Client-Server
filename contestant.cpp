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
    read_response();
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
    
}
