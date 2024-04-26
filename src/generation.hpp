#pragma once
#include "parser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


class Generator{
    public:
        inline explicit Generator(NodeProgram root):m_prog(move(root)){}


        void gen_expr(const NodeExpr& expr){
            struct ExprVistor{
                Generator* gen;
                void operator()(const NodeExprIntLit expr_int_lit){
                    gen->m_output<<"    mov rax, "<<expr_int_lit.int_lit.value.value()<<endl;
                    gen->m_output<<"    push rax"<<endl;

                }
                void operator()(const NodeExprIdent expr_indent){}
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
                    gen->m_output<<"    pop rdi, "<<endl;;
                    gen->m_output<<"    syscall"<<endl;
                }
                void operator()(const NodeStmtLet stmt_let){}
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
        const NodeProgram m_prog;
        stringstream m_output;
};