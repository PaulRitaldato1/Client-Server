
/* C networking headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* end C networking headers */

/* C++ headers */
#include<iostream>
#include <stdexcept>
/* end C++ headers */

// Connection interface
class Connection{

    public:
        virtual connection(){};
        virtual ~connection(){};
        virtual void close_connection() = 0;

    private:
        int _socket;
        struct sockaddr_in address;

        virtual void init_socket() = 0;
}; //end connection interface


class Server : public Connection {

    public:
        connection();
        ~connection();

        bool listen();
        bool parse_input();
        void close_connection();
    private:
        int _socket;
        int _connected_socket;
        struct sockaddr_in address;

        void init_socket();
};

class Client : public Connection {

    public:
        connection();
        ~connection();

        int connect();
        int make_packet();
        void close_connection();
    private:
        int _socket;
        struct sockaddr_in address;

        void init_socket();
}

