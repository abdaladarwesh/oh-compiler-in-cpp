#pragma once
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <variant>
#include "parser.hpp"
#include "logging.hpp"

class Generator
{
public:
    // we made this method to generate the interface for the assembly
    // and to automaticlly parse all the statements without the need to do a lot of things in the main file
    string gen_prog(parser::NodeProg Prog){
        string prog;
        prog += "section .text\n";
        prog += "    global _start\n";
        prog += "_start:\n";

        for (auto stmt: Prog.stmts){
            string st = gen_stmt(stmt);
            prog += st;
        }
        // the default exit if no exit was provided
        prog += "    mov     rax, 60\n";
        prog += "    syscall\n";
        return prog;
    }

    
    Generator() {}
    ~Generator() {}
    
private:
    // we made this struct to be able to locate every variable on the stack
    struct m_var
    {
        size_t location;
        string value;
    };
    size_t m_size = 0; // to know the size of the stack
    std::unordered_map<std::string, m_var> m_vars; // to assign every variable to its name
    
    string gen_stmt(parser::NodeStmt* st)
    {
        // we made this visitor because the NodeStmt can be a variant of multiple types
        struct visitor
        {
            Generator* self; // we passed this class pointer to acces the private fields of the class
            string operator()(const parser::NodeExit* node) const
            {
                string code;
                if (auto identptr = std::get_if<parser::NodeIdent *>(&node->expr->expr)){
                    auto ident = *identptr;
                    auto it = self->m_vars.find(ident->ident.value);
                    if (it == self->m_vars.end()){
                        cerr << "Undecleared identfire '" << ident->ident.value << "'" << endl;
                        exit(EXIT_FAILURE);
                    }
                    code += "    mov rdi, [rsp + " + to_string((self->m_size - it->second.location) * 8) + "]\n";
                    code += "    mov     rax, 60\n";
                    code += "    syscall\n";
                    return code;
                }
                auto intptr = std::get_if<parser::NodeInt*>(&node->expr->expr);
                auto int_lit = *intptr;
                code += "   mov rdi, " + int_lit->int_lit.value + "\n";
                code += "   mov     rax, 60\n";
                code += "   syscall\n";
                return code;
            }
            string operator()(const parser::NodeVar* node) const
            {
                string code;
                auto exprPtr = std::get_if<parser::NodeIdent*>(&node->expr->expr);
                if (exprPtr != NULL){
                    auto expr = *exprPtr;
                    auto it = self->m_vars.find(expr->ident.value);
                    if (it == self->m_vars.end()){
                        logging::error("Undecleared identfire '" + expr->ident.value + "'", node->token);
                    }
                    m_var var = {++self->m_size, it->second.value};
                    self->m_vars.insert({node->token.value, var});
                    code += "    push " + it->second.value + "\n";
                    return code;
                }
                auto exprPt = std::get_if<parser::NodeInt*>(&node->expr->expr);
                auto ex = *exprPt;
                m_var var = {++self->m_size, ex->int_lit.value};
                self->m_vars.insert({node->token.value, var});
                code += "    push " + ex->int_lit.value + "\n";
                return code;
            }
            string operator()(const parser::NodeAssign* node) const{
                string code;
                auto var = self->m_vars.find(node->token.value);
                if (var == self->m_vars.end()){
                    logging::error("Undecleared identfire '" + node->token.value + "'", node->token);
                }
                auto exprPtr = std::get_if<parser::NodeIdent*>(&node->expr->expr);
                if (exprPtr != NULL){
                    auto exprValue = * exprPtr;
                    auto it = self->m_vars.find(exprValue->ident.value);
                    if (it == self->m_vars.end()){
                        logging::error("Undecleared identfire '" + exprValue->ident.value + "'", node->token);
                    }
                    var->second.value = it->second.value;
                    code += "    mov QWORD [rsp + " + to_string((self->m_size - var->second.location) * 8) + "], " + var->second.value + "\n";
                    return code;
                }
                auto expr2Ptr = std::get_if<parser::NodeInt*>(&node->expr->expr);
                auto ex = *expr2Ptr;
                var->second.value = ex->int_lit.value;
                code += "    mov QWORD [rsp + " + to_string((self->m_size - var->second.location) * 8) + "], " + var->second.value + "\n";
                return code;
            }
        };

        return std::visit(visitor{this}, st->stmt);
    }
};
