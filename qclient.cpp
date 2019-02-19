
#include"client.h"

int main(int argc, const char* argv[]){
	
	//check if a port was provided	
	if(argc < 3){
		std::cerr << "Invalid argument number provided!" << std::endl;
		return -1;
	}

    Client client;
	if(client.socket_init(argv[1], argv[2]) == -1)
		return 0;
	
	client.connecting();
	client.parse_input();
	
	return 0;
}
