#pragma once
#include <string>
#include <vector>
using namespace std;

enum class TokenType{
    _exit,
    int_lit,
    semi,
    open_paren,
    closed_paren,
    identifier,
    _let,
    eq
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
                    else if(buff=="let"){
                        tokens.push_back(Token{TokenType::_let});
                        buff.clear();
                    }
                    else{
                        tokens.push_back(Token{TokenType::identifier,buff});
                        buff.clear();
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
                else if(peek().value()=='('){
                    tokens.push_back(Token{TokenType::open_paren});
                    consume();
                }
                else if(peek().value()==')'){
                    tokens.push_back(Token{TokenType::closed_paren});
                    consume();
                }
                else if(peek().value()==';'){
                    tokens.push_back(Token{TokenType::semi});
                    consume();
                }
                else if(peek().value()=='='){
                    tokens.push_back(Token{TokenType::eq});
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
        size_t m_pos=0;

        [[nodiscard]] inline optional<char> peek(int offset=0) const{
            if(m_pos+offset>=m_src.length())
                return {};
            return m_src.at(m_pos+offset);
        }

        inline char consume(){
            return m_src.at(m_pos++);
        }
};