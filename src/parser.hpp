#pragma once
#include <vector>
#include "tokenization.hpp"
#include <optional>
#include <iostream>

#include <variant>

namespace node {
    struct NodeExprIntLit {
        Token int_lit;
    };
    struct NodeExprIdent {
        Token ident;
    };
    struct NodeExpr {
        variant<NodeExprIntLit,NodeExprIdent> var;
    };
    struct NodeStmtExit {
        NodeExpr expr;
    };
    struct NodeStmtLet {
        Token ident;
        NodeExpr expr;
    };
    struct NodeStmt {
        variant<NodeStmtExit,NodeStmtLet> var;
    };
    struct NodeProgram {
        vector<NodeStmt> statements;
    };
}


using namespace std;
using namespace node;

class Parser{
    public:
        inline explicit Parser(vector<Token> tokens):m_tokens(move(tokens)){}

        optional<NodeProgram> parse_prog(){
            NodeProgram prog;
            
            while(peek().has_value()){
                if(auto statement=parse_stmt()){
                    prog.statements.push_back(statement.value());
                }
                else{
                    cerr<<"Invalid statement"<<endl;
                    exit(EXIT_FAILURE);
                }
            }
            return prog;
        }

        optional<NodeExpr> parse_expr(){
            if(peek().has_value() && peek().value().type==TokenType::int_lit){
                return NodeExpr{.var=NodeExprIntLit{.int_lit=consume()}};
            }
            else if(peek().has_value() && peek().value().type==TokenType::identifier){
                return NodeExpr{.var=NodeExprIdent{.ident=consume()}};
            }
            return {};
        }

        optional<NodeStmt> parse_stmt(){
            if(peek().has_value() && peek().value().type==TokenType::_exit){
                consume();
                if(peek().has_value() && peek().value().type==TokenType::open_paren){
                    consume();
                    if(auto expr = parse_expr()){
                        if(peek().has_value() && peek().value().type==TokenType::closed_paren){
                            consume();
                            if(peek().has_value() && peek().value().type==TokenType::semi){
                                consume();
                                return NodeStmt{.var=NodeStmtExit{.expr=expr.value()}};
                            }
                            else{
                                cerr<<"Expected ; after expression"<<endl;
                                exit(EXIT_FAILURE);
                            }
                        }
                        else{
                            cerr<<"Expected ) after expression"<<endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{
                        cerr<<"Expected expression after ("<<endl;
                        exit(EXIT_FAILURE);
                    }
                }
                else{
                    cerr<<"Expected ( after exit"<<endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if(peek().has_value() && peek().value().type==TokenType::_let){
                consume();
                if(peek().has_value() && peek().value().type==TokenType::identifier){
                    auto ident = consume();
                    if(peek().has_value() && peek().value().type==TokenType::eq){
                        consume();
                        if(auto expr = parse_expr()){
                            if(peek().has_value() && peek().value().type==TokenType::semi){
                                consume();
                                return NodeStmt{.var=NodeStmtLet{.ident=ident,.expr=expr.value()}};
                            }
                            else{
                                cerr<<"Expected ; after expression"<<endl;
                                exit(EXIT_FAILURE);
                            }
                        }
                        else{
                            cerr<<"Expected expression after ="<<endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{
                        cerr<<"Expected = after identifier"<<endl;
                        exit(EXIT_FAILURE);
                    }
                }
                else{
                    cerr<<"Expected identifier after let"<<endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if(peek().has_value() && peek().value().type==TokenType::identifier){
                auto ident = consume();
                if(peek().has_value() && peek().value().type==TokenType::semi){
                    consume();
                    return NodeStmt{.var=NodeStmtLet{.ident=ident,.expr=NodeExpr{.var=NodeExprIdent{.ident=ident}}}};
                }
                else{
                    cerr<<"Expected ; after identifier"<<endl;
                    exit(EXIT_FAILURE);
                }
            }
            else{
                cerr<<"Unexpected token "<<peek().value().value.value()<<endl;
                exit(EXIT_FAILURE);
            }
            return {};
        }

    private:
        const vector<Token> m_tokens;
        size_t m_pos=0;

        [[nodiscard]] inline optional<Token> peek(int offset=0) const{
            if(m_pos+offset>=m_tokens.size())
                return {};
            return m_tokens.at(m_pos+offset);
        }

        inline Token consume(){
            return m_tokens.at(m_pos++);
        }
};