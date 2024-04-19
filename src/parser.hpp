#pragma once
#include <vector>
#include "tokenization.hpp"
#include <optional>
#include <iostream>

namespace node {
    struct NodeExpr {
        Token int_lit;
    };
    struct NodeExit {
        NodeExpr expr;
    };
}


using namespace std;
using namespace node;

class Parser{
    public:
        inline explicit Parser(vector<Token> tokens):m_tokens(move(tokens)){}

        optional<NodeExpr> parse_expr(){
            if(peek().has_value() && peek().value().type==TokenType::int_lit){
                return NodeExpr{.int_lit=consume()};
            }
            return {};
        }

        optional<NodeExit> parse(){
            optional<NodeExit> exit_node; 
            while(peek().has_value()){
                if(peek().value().type==TokenType::_exit){
                    consume();
                    if(auto node_expr = parse_expr()){
                        exit_node = NodeExit{.expr=node_expr.value()};
                    }
                    else{
                        cerr<<"Invalid Expression"<<endl;
                        exit(EXIT_FAILURE);
                    }
                    if(peek().has_value() && peek().value().type==TokenType::semi){
                        consume();
                    }
                    else{
                        cerr<<"Expected ; after exit"<<endl;
                        exit(EXIT_FAILURE);
                    }
                }
            }
            return exit_node;
        };
    private:
        const vector<Token> m_tokens;
        size_t m_pos=0;

        [[nodiscard]] inline optional<Token> peek(int offset=0) const{
            if(m_pos+offset>m_tokens.size())
                return {};
            return m_tokens.at(m_pos+offset);
        }

        inline Token consume(){
            return m_tokens.at(m_pos++);
        }
};