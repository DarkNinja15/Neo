#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
using namespace std;

enum class TokenType{
    _return,
    int_lit,
    semi
};

struct Token
{
    TokenType type;
    optional<string> value {};
};

vector<Token> tokenize(string &str){
    vector<Token> tokens {};
    string buff; 
    for(int i=0;i<str.size();i++){
        char c=str.at(i);
        if(isalpha(c)){
            buff.push_back(c);
            i++;
            while(i<str.size() && isalnum(str.at(i))){
                buff.push_back(str.at(i));
                i++;
            }
            i--;

            if(buff=="return"){
                tokens.push_back(Token{TokenType::_return});
                buff.clear();
            }
            else{
                cerr<<"Unknown token "<<buff<<endl;
                exit(EXIT_FAILURE);
            }
        }
        else if(isdigit(c)){
            buff.push_back(c);
            i++;
            while(i<str.size() && isdigit(str.at(i))){
                buff.push_back(str.at(i));
                i++;
            }
            i--;

            tokens.push_back(Token{TokenType::int_lit,.value=buff});
            buff.clear();
        }
        else if(c==';'){
            tokens.push_back(Token{TokenType::semi});
        }
        else if(isspace(c)){
            continue;
        }
        else{
            cerr<<"Unknown token "<<c<<endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

string tokensToASM(const vector<Token> &tokens){
    stringstream output;
    output<<"global _start\n_start:\n";
    for(int i=0;i<tokens.size();i++){
        const Token token=tokens.at(i);
        if(token.type==TokenType::_return){
            if(i+1<tokens.size() && tokens.at(i+1).type==TokenType::int_lit){
                if(i+2<tokens.size() && tokens.at(i+2).type==TokenType::semi){
                    output<<"    mov rax, 60\n";
                    output<<"    mov rdi, "<<tokens.at(i+1).value.value()<<endl;
                    output<<"    syscall"<<endl;
                }
                else{
                    cerr<<"Expected ; after return"<<endl;
                    exit(EXIT_FAILURE);
                }
            }
            else{
                cerr<<"Expected int literal after return"<<endl;
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

    vector<Token> tokens=tokenize(contents);
    string asmC = tokensToASM(tokens);
    
    {
        fstream output("out.asm",ios::out);
        output<<asmC;
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}