#include<string>
#include<iostream>
int main(){

    std::string s;
    std::cin >> s;

    std::string s1;

    getline(std::cin, s1, '.');
    std::cout << s1;



}