#pragma once
#include <cstdlib>
#include <string>
#include <unordered_map>
#include "parser.hpp"
#include "logging.hpp"

class Generator
{
public:
    // we made this method to generate the interface for the assembly
    // and to automaticlly parse all the statements without the need to do a lot of things in the main file
    string gen_prog(parser::NodeProg Prog){
        string prog;
        prog += "extern ExitProcess\n";
        prog += "section .text\n";
        prog += "    global main\n";
        prog += "main:\n";

        for (auto stmt: Prog.stmts){
            string st = gen_stmt(stmt);
            prog += st;
        }
        // the default exit if no exit was provided
        prog += "    mov     ecx, 0\n";
        prog += "    call ExitProcess\n";
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

    string gen_stmt(parser::NodeStmt st)
    {
        // we made this visitor because the NodeStmt can be a variant of multiple types
        struct visitor
        {
            Generator* self; // we passed this class pointer to acces the private fields of the class
            string operator()(const parser::NodeExit &node) const
            {
                string code;
                if (node.expr.is_ident){
                    auto it = self->m_vars.find(node.expr.value);
                    if (it == self->m_vars.end()){
                        cerr << "Undecleared identfire '" << node.expr.value << "'" << endl;
                        exit(EXIT_FAILURE);
                    }
                    code += "    mov ecx, [rsp + " + to_string((self->m_size - it->second.location) * 8) + "]\n";
                    code += "    call ExitProcess\n";
                    return code;
                }
                code += "   mov ecx, " + node.expr.value;
                code += "    call ExitProcess\n";
                return code;
            }
            string operator()(const parser::NodeVar &node) const
            {
                string code;
                if (node.expr.is_ident){
                    auto it = self->m_vars.find(node.expr.value);
                    if (it == self->m_vars.end()){
                        logging::error("Undecleared identfire '" + node.expr.value + "'", node.token);
                    }
                    m_var var = {self->m_size++, it->second.value};
                    self->m_vars.insert({node.token.value, var});
                    code += "    push " + it->second.value + "\n";
                    return code;
                }
                m_var var = {self->m_size++, node.expr.value};
                self->m_vars.insert({node.token.value, var});
                code += "    push " + node.expr.value + "\n";
                return code;
            }
            string operator()(const parser::NodeAssign node) const{
                string code;
                auto var = self->m_vars.find(node.token.value);
                if (var == self->m_vars.end()){
                    logging::error("Undecleared identfire '" + node.token.value + "'", node.token);
                }
                if (node.expr.is_ident){
                    auto it = self->m_vars.find(node.expr.value);
                    if (it == self->m_vars.end()){
                        logging::error("Undecleared identfire '" + node.expr.value + "'", node.token);
                    }
                    var->second.value = it->second.value;
                    code += "    mov QWORD [rsp + " + to_string((self->m_size - var->second.location) * 8) + "], " + var->second.value + "\n";
                    return code;
                }
                var->second.value = node.expr.value;
                code += "    mov QWORD [rsp + " + to_string((self->m_size - var->second.location) * 8) + "], " + var->second.value + "\n";
                return code;
            }
        };

        return std::visit(visitor{this}, st.stmt);
    }
};
