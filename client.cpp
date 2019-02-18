#include "client.h"
void Client::socket_init(const char* hostname, const char* port){
   
    //create socket
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(_socket < 0){
		std::cerr << "Failed to create socket!" << std::endl;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	
    char ip[15]; //max size an ip addr can be 255.255.255.255 = 15 chars

	unsigned short PORT = atoi(port);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(resolve_hostname(hostname, hostname, ip) < 0){
        std::cerr << "Failed to resolve hostname " << hostname << std::endl; 
    }

	//convert ipv4 to binary data
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) < 0){
		printf("Could not use ip address provided!");
	}
}
int Client::connecting(){
	//open connection with server
	if(connect(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("Connection Failed!");
        return -1;
	}
    std::cout << "Connected!" << std::endl;
}

void Client::close_connection(){
    close(_socket);
}

int Client::resolve_hostname(const char* hostname, const char* port, char* ip){
    
    struct addrinfo hints;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* results = 0;

    if(getaddrinfo(hostname, port, &hints, &results)){
        return -1;
    }

    for (results; results != NULL; results->ai_next){
        struct sockaddr_in* temp = (struct sockaddr_in* ) results->ai_addr;
        strcpy(ip, inet_ntoa(temp->sin_addr));
    }

    freeaddrinfo(results);
    return 0;

}

 int parse_input(){

     bool run_client = true;
     while(run_client){
         char command;
         std::cout << "Type your command letter then press enter" <<std::endl;
         std::cin >> command;

         switch(command){
            case 'k':
            case 'p':
            case 'q' :
                close_connection();               
                run_client = false;
                break;
            case 'h' :
            case 'd' :
            case 'g' :
            case 'r' :
            case 'c' : 
            default :

         }


     }

 }