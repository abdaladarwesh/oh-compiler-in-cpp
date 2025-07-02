#pragma once
#include <cstdlib>
#include <iostream>
#include "Tokenizer.hpp"
using namespace std;

class logging
{
public:
    static void error(const string &err, tokinaizer::Token tk){
        cerr << "\033[31mError detected at line " << tk.line << "\033[31m: ";
        cerr << err << "\033[0m" << endl;
        exit(EXIT_FAILURE);
    }

    logging(){}
    ~logging(){}
};

