#include "question.h"

std::string Question::to_string_rand(){
    std::string rtn_str = "";

    rtn_str += std::to_string(_question_num);
    rtn_str += "\n";
    rtn_str += _question_text;
    rtn_str += "\n";

    
    for(int i = 0; i != _question_choices.size(); ++i){
    
        rtn_str += _question_choices[i];
        rtn_str += "\n";


    }

    return rtn_str;

}

std::string Question::to_string_get(){
    std::string rtn_str = "";

    rtn_str += _question_tag;
    rtn_str += "\n";
    rtn_str += _question_text;
    rtn_str += "\n";
    
    for(int i = 0; i != _question_choices.size(); ++i){
    
        rtn_str += _question_choices[i];
        rtn_str += "\n";


    }

    return rtn_str;

}


int Question::write_out(bool first){
    std::string out_str = to_string_IO();
    if(first){
        out_str.erase(0, 1);
        std::ofstream file;
        file.open("questions.txt");
    
        if(!file.is_open())
            return -1;

        file << out_str;
        file.close();
    }
    else{
        std::ofstream file;
        file.open("questions.txt", std::ios_base::app);
    
        if(!file.is_open())
            return -1;

        file << out_str;
        file.close();
    }
    
    
    return 1;
}

std::string Question::to_string_IO(){

    std::string rtn_str = "";

    rtn_str += std::to_string(_question_num);
    rtn_str += "\n";
    //have to conver to char* for string constructor
    //char* cor_ans = _correct_answer&;
    std::string ans(&_correct_answer);
    rtn_str += ans; 
    rtn_str += "\n";
    rtn_str += _question_tag;
    rtn_str += "\n";
    rtn_str += _question_text;
    rtn_str += "\n.";
    
    for(int i = 0; i != _question_choices.size(); ++i){
        rtn_str += "\n";
        rtn_str += _question_choices[i];
        rtn_str += "\n.";

    }
    rtn_str += "\n.";
    std::string s = "\n";
    s += rtn_str;

    return s;

}