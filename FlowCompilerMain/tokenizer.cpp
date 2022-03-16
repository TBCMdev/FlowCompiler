#pragma once

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

public:
    static lexertk::generator compile(string fc,bool debug = false) // fc = file content
    {
        lexertk::generator generator;

        if (!generator.process(fc))
        {
            if (debug) cout << "Failed to lex: " << fc << endl;
        }

        if (debug) lexertk::helper::dump(generator);
        // MAIN LOGIC
        return generator;
    }
    
};