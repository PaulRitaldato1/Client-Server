#include "server.h"
Server server;
void my_handler(int s){
    server.~Server();
    std::cout << "\nCaught ctrl+c kill signal. Handled!" << std::endl;
    exit(1);
}
int main(int argc, const char* argv[]){

    bool server_online = true;
    bool connection_alive = false;
    while (server_online){
        try{
            signal(SIGINT, my_handler);
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
                bool parse_out = server.parse_input();
                DEBUG("Parse out: " + std::to_string(parse_out));
                if(!parse_out){
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

    // struct sigaction sigIntHandler;

    // sigIntHandler.sa_handler = my_handler;
    // sigemptyset(&sigIntHandler.sa_mask);
    // sigIntHandler.sa_flags = 0;

    // sigaction(SIGINT, &sigIntHandler, NULL);
    return 1;
}
