#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, const char* argv[]){
	
	struct sockaddr_in address;
	int sockfd, valread;
	struct sockaddr_in serv_addr;
	const char* msg = "Paul Ritaldato:1470-1443";
	
	char buffer[1024] = {0};
	//check if a port was provided	
	if(argc < 2){
		printf("No port provided! Please specify a port.");
		return -1;
	}
	
	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Failed to create socket!");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	
	//set port to command line arguments
	int PORT = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	//convert ipv4 and ipv6 to binary data
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) < 0){
		printf("Could not use ip address provided!");
		return -1;
	}
	
	//open connection with server
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("Connection Failed!");
	}
	
	//send message to server
	send(sockfd, msg, strlen(msg), 0);

	//read and print message that was returned from the server
	valread = read(sockfd, buffer, 1024);
	printf("%s", buffer);
	return 0;
}
