#include  "core.h"
#include "contest.h"

int main(){

    std::vector<int*> test;
    int num;
    std::cin >> num;
    int* tmp;
    for(int i = 0; i != num; ++i ){
        tmp = new int;
        *tmp = i + 1;
        test.push_back(tmp);
    }

    for(int* i : test){
        std::cout << *i << std::endl;
    }

}