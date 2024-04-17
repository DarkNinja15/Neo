#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include <tokenization.hpp>
using namespace std;

string tokensToASM(const vector<Token> &tokens){
    stringstream output;
    output<<"global _start\n_start:\n";
    for(int i=0;i<tokens.size();i++){
        const Token token=tokens.at(i);
        if(token.type==TokenType::_exit){
            if(i+1<tokens.size() && tokens.at(i+1).type==TokenType::int_lit){
                if(i+2<tokens.size() && tokens.at(i+2).type==TokenType::semi){
                    output<<"    mov rax, 60\n";
                    output<<"    mov rdi, "<<tokens.at(i+1).value.value()<<endl;
                    output<<"    syscall"<<endl;
                }
                else{
                    cerr<<"Expected ; after exit"<<endl;
                    exit(EXIT_FAILURE);
                }
            }
            else{
                cerr<<"Expected int literal after exit"<<endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return output.str();
}

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
    string asmC = tokensToASM(tokens);
    
    {
        fstream output("out.asm",ios::out);
        output<<asmC;
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}