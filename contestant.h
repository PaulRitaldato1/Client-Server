#include "core.h"

class Contestant{
 public:
    int connecting();
    int socket_init(const char* hostname, const char* port);
    void run_game();
 private:
    int _socket;
    struct sockaddr_in address, serv_addr;
    int resolve_hostname(const char* hostname, const char* port,std::string& ip);
    std::string yoink();
    int yeet(std::string s);
};