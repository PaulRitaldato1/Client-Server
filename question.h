#include<iostream>
#include<fstream>
#include<string>
#include<vector>

class Question{
    public:
        Question(int question_num, std::string question_tag, std::string question_text, std::vector<std::string> question_choices, char correct_answer) 
        : _question_num(question_num), _question_tag(question_tag), _question_text(question_text), _question_choices(question_choices), _correct_answer(correct_answer)
        {}

        inline ~Question(){ delete this; }
        //write out question to file
        int write_out(bool first);
        
        std::string to_string(); //returns string that can be presented to client
        inline bool check_answer(char ans){ return ans == _correct_answer; }

        inline int get_question_num() { return _question_num; }
        inline std::string get_question_tag() { return _question_tag; }
        inline std::string get_question_text() { return _question_text; }
        inline std::vector<std::string> get_question_choices() { return _question_choices; }

    private:

        int _question_num;
        std::string _question_tag;
        std::string _question_text;
        std::vector<std::string> _question_choices;
        char _correct_answer;
        std::string to_string_IO(); //returns string that can be printed to file. Contains all info about object
};