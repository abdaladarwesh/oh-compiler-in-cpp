#include <iostream>
#include <fstream>
#include <sstream>
#include "Tokenizer.hpp"
#include "parser.hpp"
#include "Generator.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // make sure that there is a file next to the compiler
    if (argc < 2)
    {
        cerr << "erorr : usage is " << argv[0] << " <filename>" << endl;
        exit(EXIT_FAILURE);
    }
    
    fstream filepath(argv[1]); // takes the file path from program argument
    if (!filepath.is_open())
    {
        cerr << "erorr : Wrong file path\nfile not found";
        exit(EXIT_FAILURE);
    }

    stringstream buffer; // prepare a buffer to load the file in
    buffer << filepath.rdbuf(); // load the file into the buffer
    
    
    tokinaizer tok;
    vector<tokinaizer::Token> tokens = tok.tokenize(buffer.str()); // tokinaize the file in to tokens

    // then parse the tokens to make the sure that the program have a valid grammer AKA syntax
    parser Parser;
    auto stmts = Parser.parse_stmt(tokens);


    Generator gen; // the generator class to generate assembly after making sure that the program is valid

    ofstream myFile("main.asm", ios::out | ios::trunc); // open or create the file if not existed
    if (myFile.is_open()){
        myFile << gen.gen_prog(stmts); // add the generated assembly to the file
    }
    myFile.close(); // close the file because the assembler and the linker fails if the file is not closed

    system("nasm -f elf64 main.asm -o main.obj"); // assemble the assembly file to a object file
    system("ld main.obj -o main.out"); // link the assembly file to create a excutable AKA .exe file

    exit(EXIT_SUCCESS); // just the equivlent to return 0;
}