#pragma once
#include "parser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

using namespace std;


class Generator{
    public:
        inline explicit Generator(NodeProgram root):m_prog(move(root)){}


        void gen_expr(const NodeExpr& expr){
            struct ExprVistor{
                Generator* gen;
                void operator()(const NodeExprIntLit expr_int_lit){
                    gen->m_output<<"    mov rax, "<<expr_int_lit.int_lit.value.value()<<endl;
                    gen->push("rax");

                }
                void operator()(const NodeExprIdent expr_indent){
                    if(gen->m_vars.find(expr_indent.ident.value.value())==gen->m_vars.end()){
                        cerr<<"Undeclared identifier: "<<expr_indent.ident.value.value()<<endl;
                        exit(EXIT_FAILURE);
                    }
                    const auto &var=gen->m_vars.at(expr_indent.ident.value.value());
                    stringstream offset;
                    offset<<"QWORD [rsp + "<<(gen->m_stack_size-var.stack_location-1)*8<<"]\n";
                    gen->push(offset.str());
                }
            };

            ExprVistor visitor{.gen=this};
            visit(visitor,expr.var);
        }

        void gen_stmt(const NodeStmt& statement){
            struct StmtVisitor{
                Generator* gen;
                void operator()(const NodeStmtExit stmt_exit) const{
                    gen->gen_expr(stmt_exit.expr);
                    gen->m_output<<"    mov rax, 60"<<endl;
                    gen->pop("rdi");
                    gen->m_output<<"    syscall"<<endl;
                }
                void operator()(const NodeStmtLet stmt_let){
                    if(gen->m_vars.find(stmt_let.ident.value.value())!=gen->m_vars.end()){
                        cerr<<"Identifier already in use "<<stmt_let.ident.value.value()<<endl;
                        exit(EXIT_FAILURE);
                    }
                    gen->m_vars.insert({stmt_let.ident.value.value(),Var{.stack_location=gen->m_stack_size}});
                    gen->gen_expr(stmt_let.expr);
                }
            };
            StmtVisitor vistor{.gen=this};
            visit(vistor,statement.var);
        }

        [[nodiscard]] inline string gen_prog(){
            m_output<<"global _start\n_start:\n";

            for(const NodeStmt &stmt:m_prog.statements){
                gen_stmt(stmt);
            }

            m_output<<"    mov rax, 60"<<endl;
            m_output<<"    mov rdi, 0"<<endl;;
            m_output<<"    syscall"<<endl;
            return m_output.str();
        }
    private:

        void push(const string& reg){
            m_output<<"    push "<<reg<<endl;
            m_stack_size++;
        }

        void pop(const string& reg){
            m_output<<"    pop "<<reg<<endl;
            m_stack_size--;
        }

        struct Var{
            size_t stack_location;
        };

        const NodeProgram m_prog;
        stringstream m_output;
        size_t m_stack_size=0;
        unordered_map<string,Var> m_vars {};
};