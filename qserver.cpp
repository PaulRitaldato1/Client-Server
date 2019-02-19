#include "server.h"

int main(int argc, const char* argv[]){
	
	Server server;
	bool server_online = true;
	bool connection_alive = false;
	
	while (server_online){
		try{

			//listen() will return true if a connection is accepted
			if(server.listening())
				connection_alive = true;

		}catch(std::exception& e){
			std::cerr << "Connection failed to establish!" << std::endl;
			continue;
		}

		while(connection_alive){
			
			try {

				//this function will return false if the kill command 'k' is given, which will then terminate the server completely
				if(!server.parse_input()){
					connection_alive = false;
					std::cout << "\nClient has closed and/or connection was lost!\nListening for new connections.\n" << std::endl;
					//server.close_connection();
				}

			}catch (std::exception& e){
				server_online = false;
				connection_alive = false;
				std::cerr  << e.what() << std::endl;
			}
		}
	}
	return 0;
}
