
#include"client.h"

int main(int argc, const char* argv[]){
  std::string filename = "";
	//check if a port was provided
	if(argc < 3){
		std::cerr << "Invalid argument number provided!" << std::endl;
		return -1;
	}
  else if(argc > 3){
    filename = arg[3]
  }

  Contestmeister cm;
	if(client.socket_init(argv[1], argv[2]) == -1)
		return 0;

	cm.connecting();
	cm.parse_input(filename);

	return 0;
}
