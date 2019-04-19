#include "contestant.h"

int main (int argc, const char* argv[]){

    if(argc < 3 || argc > 3){
        std::cerr << "Invalid number of arguments!" << std::endl;
        return -1;
    }

    Contestant contestant;
    if(contestant.socket_init(argv[1], argv[2]) == -1)
        return -1;
    contestant.connecting();
    contestant.run_game();
    return 0;
}