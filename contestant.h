/* C networking headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdio.h>
#include<netdb.h>	
/* end C networking headers */

/* C++ headers */
#include<iostream>
#include<vector>
#include "question.h"
#include <stdexcept>
#include <fstream>
/* end C++ headers */

class Contestant{
 public:
    int connecting();
    int socket_init(const char* hostname, const char* port);
 private:
    int _socket;
    struct sockaddr_in address, serv_addr;
    int resolve_hostname(const char* hostname, const char* port,std::string& ip);
    void read_response();
    int send_response(std::string s);
}
