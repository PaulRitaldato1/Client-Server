#include "connection.h"

Server::Server(){
    socket_init();
}
Server::~Server(){
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

bool Server::listen(){
	
	//Open the socket for listening, only allow 1 connection at a time.	
	if(listen(_socket, 1) < 0){
		throw std::runtime_error("Server::listen(): Failed to listen on socket.");
	}
	
	//accept a connection so that I can read the input
	_connected_socket = accept(_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if(_connected_socket < 0){
		throw std::runtime_error("Server::listen(): Failed to accept connection.");
	}
    else{
        std::cout << "Server::listen(): Connection accepted on port " << ntohs(address.sin_port) << " with hostname " << address.sin_addr.s_addr <<  << std::endl;
        return true;
    }

}

bool Server::parse_input(){

    char incoming_size = 0;
    int read_bytes = 0;

    //The first byte sent will be a number representing how many bytes the incoming message will be
    if(read(_connected_socket, &incoming_size, 1))
        throw std::runtime_error("Server::parse_input(): Failed to read size of incoming message");
    
    //extract the size, create a buffer the appropriate size
    int size = atoi(incoming_size);
    char* msg;
    msg = new(nothrow) char[size];

    
    //read the message
    read_bytes = read(_connected_socket, &msg, size);
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

    }
    
    
}

void Server::close_connection(){
    close(_connected_socket);
}