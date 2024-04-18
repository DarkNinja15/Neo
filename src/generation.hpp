#pragma once
#include "parser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


class Generator{
    public:
        inline explicit Generator(NodeExit root):m_root(move(root)){}

        [[nodiscard]] inline string generate() const{
            stringstream output;
            output<<"global _start\n_start:\n";
            output<<"    mov rax, 60\n";
            output<<"    mov rdi, "<<m_root.expr.int_lit.value.value()<<endl;
            output<<"    syscall"<<endl;
            return output.str();
        }
    private:
        const NodeExit m_root;
};