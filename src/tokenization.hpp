#pragma once
#include <string>
#include <vector>
using namespace std;

enum class TokenType{
    _exit,
    int_lit,
    semi
};

struct Token
{
    TokenType type;
    optional<string> value {};
};

class Tokenizer{
    public:
        inline explicit Tokenizer(const string &src):m_src(move(src)){}

        inline vector<Token> tokenize(){
            vector<Token> tokens {};
            string buff;

            while(peek().has_value()){
                if(isalpha(peek().value())){
                    buff.push_back(consume());
                    while(peek().has_value() && isalnum(peek().value())){
                        buff.push_back(consume());
                    }
                    if(buff=="exit"){
                        tokens.push_back(Token{TokenType::_exit});
                        buff.clear();
                    }
                    else{
                        cerr<<"Unknown token "<<buff<<endl;
                        exit(EXIT_FAILURE);
                    }
                }
                else if(isdigit(peek().value())){
                    buff.push_back(consume());
                    while(peek().has_value() && isdigit(peek().value())){
                        buff.push_back(consume());
                    }
                    tokens.push_back(Token{TokenType::int_lit,buff});
                    buff.clear();
                }
                else if(peek().value()==';'){
                    tokens.push_back(Token{TokenType::semi});
                    consume();
                }
                else if(isspace(peek().value())){
                    consume();
                }
                else{
                    
                    cerr<<"Unknown token "<<peek().value()<<endl;
                    exit(EXIT_FAILURE);
                }
            }
            return tokens;
        }

    private:
        const string m_src;
        int m_pos=0;

        [[nodiscard]] optional<char> peek(int ahead=1) const{
            if(m_pos+ahead>m_src.length())
                return {};
            return m_src.at(m_pos);
        }

        char consume(){
            return m_src.at(m_pos++);
        }
};