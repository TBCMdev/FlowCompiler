#include "iostream"
#include "entry.h"
#include <iostream>
#include <list>
#include <regex>
#include <map>
#include "../dependencies/lexertk-master/lexertk.hpp"

/*
--READ--
using a handy library found at: https://github.com/ArashPartow/lexertk
please check it out! ill be using this for the lexical indexing.

*/

using namespace std;
using namespace flow;


class Tokenizer
{

private:
    std::map<string, string> list;

public:
    Tokenizer(std::map<string, string> m)
    {
        list = m;
    }
    std::list<string> compile(string fc) // fc = file content
    {
        std::list<string> result;

        //MAIN LOGIC 
        return result;
    }
};