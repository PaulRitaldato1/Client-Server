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

 int Client::parse_input(){

     bool run_client = true;
     while(run_client){
         char command;
         std::cout << "> " <<std::endl;
         std::cin >> command;

         switch(command){
            case 'k':
                kill();
                break;
            case 'p':
                put_q();
                break;
            case 'q' :
                close_connection();               
                run_client = false;
                break;
            case 'h' :
                help();
                break;
            case 'd' :
                delete_q();
                break;
            case 'g' :
                get();
                break;
            case 'r' :
                random();
                break;
            case 'c' :
                check_answer();
                break;
            default :
                continue;
         }


     }

     return 1;

 }

        void Client::help(){
            std::string help_message = "Here are the commands you can use:\np - put question on server\nd<n> - delete question <n> from bank\ng<n> - get question <n> from bank\nr - get random question from bank\nc <n> <x> - check answer <x> of question <n>\nk - kill the server\nq - quit the client\nh - print this help message";
            std::cout << help_message << std::endl;       
        }
        void Client::kill(){
            //send kill message
            std::string command = "k";
            send_response(command);
        }

        void Client::check_answer(){
            std::string command = "c";
            std::string number;
            std::cin >> number;
            std::string ans;
            std::cin >> ans;

            std::string s = command + " " + number + " " + ans;
            send_response(s);
            read_response();
        }

        void Client::random(){
            std::string command = "r";
            send_response(command);
            read_response();
            std::string choice;

            bool input = true;
            while(input){
                std::cin >> choice;
                if(choice.at(0) >= 'a' && choice.at(0) <= 'd'){
                    send_response(choice);
                    input = false;
                    continue;
                }
                else{
                    std::cout << "Invalid input, try any char a-d" << std::endl;
                }
            }

            read_response();
            
        }
        void Client::get(){
            std::string command = "g";
            std::string number;
            std::cin >> number;
            std::string s = command + " " + number;
            send_response(s);
            read_response();
        }

        void Client::delete_q(){

            std::string command = "d";
            std::string number;
            std::cin >> number;

            std::string s = command + " " + number;
            send_response(s);
            read_response();
        }
        void Client::put_q(){}

        void Client::read_response(){

            uint32_t length;
            read(_socket, &length, sizeof(uint32_t));
            length = ntohl(length);
            //extract the size, create a buffer the appropriate size
            int size = length;
            char* msg;
            msg = new(std::nothrow) char[size];

            read(_socket, msg, size);

            std::cout << msg << std::endl;
            delete [] msg;
        }

        void Client::send_response(std::string s){
            uint32_t length = htonl(s.length());
            send(_socket, &length, sizeof(uint32_t), 0);
            send(_socket, s.c_str(), strlen(s.c_str()), 0);
        }