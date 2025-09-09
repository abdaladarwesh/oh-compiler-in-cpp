#pragma once
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "logging.hpp"
#include "parser.hpp"
#include <algorithm>
#include <vector>
#include <string>


class Generator
{
public:
    // we made this method to generate the interface for the assembly
    // and to automaticlly parse all the statements without the need to do a lot of things in the main file
    string gen_prog(parser::NodeProg Prog){
        code += "section .text\n";
        code += "   global _start\n";
        code += "_start:\n";

        for (auto stmt: Prog.stmts){
            gen_stmt(stmt);
        }
        // the default exit if no exit was provided
        code += "   mov     rax, 60\n";
        code += "   syscall\n";
        return code;
    }

    
    Generator() {}
    ~Generator() {}
    
private:

    void gen_term(const parser::NodeTerm* te)
    {
        struct visitor
        {
            Generator *self;
            void operator()(const parser::NodeIdent* ident) const{
                const auto it = std::ranges::find_if(std::as_const(self->m_vars), [&](const m_var& var) {
                    return var.value == ident->ident.value;
                });
                if (it == self->m_vars.cend()) {
                    logging::error("Undeclared identifier: " + ident->ident.value , ident->ident);
                }
                self->push("QWORD [rsp + " + to_string(((self->m_size - it->location) * 8)) + "]\n");
            }

            void operator()(const parser::NodeInt* int_lit) const
            {
                self->code += "   mov rax, " + int_lit->int_lit.value + "\n";
                self->push("rax");
            }
        };
        std::visit(visitor{this},  te->var);
    }

    void gen_bin_expr(const parser::BinExpr* BE)
    {
        struct visitor
        {
            Generator *self;

            void operator()(const parser::NodeBinExprPlus* Binplus)
            {
                self->genExpr(Binplus->lhs);
                self->genExpr(Binplus->rhs);
                self->pop("rax");
                self->pop("rbx");
                self->code += "   add rax, rbx\n" ;
                self->push("rax");
            }
            void operator()(const parser::NodeBinExprMulti* BinMulti)
            {
                self->genExpr(BinMulti->lhs);
                self->genExpr(BinMulti->rhs);
                self->pop("rax");
                self->pop("rbx");
                self->code += "   mul rbx\n" ;
                self->push("rax"); 
            }
            void operator()(const parser::NodeBinExprSub* BinSub)
            {
                self->genExpr(BinSub->lhs);
                self->genExpr(BinSub->rhs);
                self->pop("rbx");
                self->pop("rax");
                self->code += "   sub rax, rbx\n" ;
                self->push("rax"); 
            }
            void operator()(const parser::NodeBinExprDev* BinDiv)
            {
                self->genExpr(BinDiv->lhs);
                self->genExpr(BinDiv->rhs);
                self->pop("rax");
                self->code += "   xor rdx, rdx\n";
                self->pop("rbx");
                self->code += "   div rbx\n" ;
                self->push("rdx"); 
            }
        };
        std::visit(visitor{this}, BE->var);
    }

    void genExpr(parser::NodeExpr* Expr){
        struct visitor
        {
            Generator* self;
            void operator()(const parser::NodeTerm* te) const
            {
                self->gen_term(te);
            }
            void operator()(const parser::BinExpr* BE) const
            {
                self->gen_bin_expr(BE);
            }
        };

        std::visit(visitor{this}, Expr->var);

    }

    void gen_stmt(parser::NodeStmt* st)
    {
        // we made this visitor because the NodeStmt can be a variant of multiple types
        struct visitor
        {
            Generator* self; // we passed this class pointer to acces the private fields of the class
            void operator()(const parser::NodeExit* node) const
            {
                self->code += "   ; exit\n";
                self->genExpr(node->expr);
                self->code += "   mov     rax, 60\n";
                self->pop("rdi");
                self->code += "   syscall\n";
                self->code += "   ; /exit\n";
            }
            void operator()(const parser::NodeVar* node) const
            {
                self->code += "   ; let\n";
                if (std::ranges::find_if(
                    std::as_const(self->m_vars),
                        [&](const m_var& var) { return var.value == node->token.value; })
                    != self->m_vars.cend()) {
                        logging::error("Already Decleared Identfier", node->token);
                }
				size_t gg = self->m_size;
                self->m_vars.push_back(m_var{++gg, node->token.value});
                self->genExpr(node->expr);
                self->code += "   ; end let\n";
            }
            void operator()(const parser::NodeAssign* node) const{

                const auto it = std::ranges::find_if(self->m_vars, [&](const m_var &var) {
                    return var.value == node->token.value;
                });
                if (it == self->m_vars.cend()) {
                    logging::error("Undecleared Identfire", node->token);
                }
                self->genExpr(node->expr);
                self->pop("rax");
                self->code += "   mov [rsp + " + to_string((self->m_size - it->location ) * 8 ) += "], rax\n";
            }
        };

        std::visit(visitor{this}, st->var);
    }



    void push(string reg){
        code += "   push " + reg + "\n";
        m_size++;
    }

    void pop(string reg){
        code += "   pop " + reg + "\n";
        m_size--;  
    }
    
    string code;
    struct m_var
    {
        size_t location;
        string value;
    };
    size_t m_size = 0; // to know the size of the stack
    std::vector<m_var> m_vars;
};
