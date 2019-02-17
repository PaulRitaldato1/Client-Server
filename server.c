#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, const char* argv[]){


	int sockfd, newsock, valread;
	const char* msg = "Welcome to the server running on REPTILIAN";
	
	//this struct is for handling internet addresses. In this case it will only be for localhost
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	
	//check if a port was provided
	int n;
	if(argc < 2){
		printf("No port provided! Please specify a port.");
		return -1;	
	}
	
	//open the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Failed to open socket!");
		return -1;
	}
	
	//set socket options
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		printf("Failed to set socket options");
		return -1;
	}
	
	//set address values
	//Set port to command line args
	int PORT = atoi(argv[1]);
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;
	
	//bind the socket
	if(bind(sockfd, (struct sockaddr *)&address, sizeof(address) ) < 0){
		printf("Failed to bind socket");
		return -1;
	}

	//Open the socket for listening, only allow 1 connection at a time.	
	if(listen(sockfd, 1) < 0){
		printf("Failed to open listener");
		return -1;
	}
	
	//accept a connection so that I can read the input
	newsock = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if(newsock < 0){
		printf("Failed to accept connection");
		return -1;
	}
	
	//read input from a connection into the buffer.
	valread = read(newsock, buffer, 1024);
	//print the buffer
	printf("%s", buffer);
	//send a response
	send(newsock, msg, strlen(msg), 0);
	return 0;
}
