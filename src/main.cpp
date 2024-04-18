#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include <generation.hpp>
using namespace std;

int main(int argc, char* argv[]){
    if(argc!=2){
        cerr<<"Incorrect usage.Input file is required"<<endl;
        cerr<<"neo <intput.neo>"<<endl;
        return EXIT_FAILURE;
    }
    
    stringstream contents_stream;
    {
        fstream input(argv[1],ios::in);
        contents_stream<<input.rdbuf();
    }
    string contents=contents_stream.str();

    Tokenizer tokenizer(move(contents));
    vector<Token> tokens=tokenizer.tokenize();

    Parser parser(tokens);
    optional<NodeExit> parseTree=parser.parse();

    if(!parseTree.has_value()){
        cerr<<"Invalid Syntax"<<endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(parseTree.value());
    
    {
        fstream output("out.asm",ios::out);
        output<<generator.generate();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}