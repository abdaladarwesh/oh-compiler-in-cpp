#pragma once
#include <vector>
#include <optional>
#include "Tokenizer.hpp"
#include <variant>
#include "logging.hpp"
using namespace std;


class parser
{
public:

    struct NodeExpr
    {
        string value;
        bool is_ident = false; // true if this is an identifier
    };
    struct NodeVar
    {
        tokinaizer::Token token; // the name
        NodeExpr expr; // value
    };
    struct NodeAssign
    {
        tokinaizer::Token token; // the name
        NodeExpr expr; // value
    };
    struct NodeExit
    {
        NodeExpr expr;
    };
    struct NodeStmt
    {
        variant<NodeExit, NodeVar, NodeAssign> stmt;
    };
    struct NodeProg
    {
        vector<NodeStmt> stmts;
    };


    NodeProg parse_stmt(vector<tokinaizer::Token> toks)
    {
        tokens = toks;
        size = 0;
        vector<NodeStmt> stmts;
        optional<NodeExpr> expr;
        while (size < tokens.size())
        {
            if (toks[size].type == tokinaizer::TokenType::Exit)
            {
                consume();
                try_consume(tokinaizer::TokenType::OpenPrenc, "Expected '('");
                auto p = peek().value();
                expr = parse_expr(peek().value());
                if (!expr.has_value())
                {
                    logging::error("Expected a valid expression", peek().value());
                }
                try_consume(tokinaizer::TokenType::ClosePrenc, "Expected ')'");
                try_consume(tokinaizer::TokenType::semi, "Expected ';'");
                stmts.push_back(NodeStmt{NodeExit{expr.value()}});
            }
            else if(toks[size].type == tokinaizer::TokenType::Var){
                consume();
                auto var_token = toks[size];
                try_consume(tokinaizer::TokenType::Ident, "Expected an identfire");
                try_consume(tokinaizer::TokenType::eq, "Expected '='");
                auto expr = parse_expr(peek().value());
                if (!expr.has_value()){
                    logging::error("invalid Exepression", peek().value());
                }
                try_consume(tokinaizer::TokenType::semi, "Expected ';'");
                stmts.push_back({NodeStmt{NodeVar{var_token, expr.value()}}});

            }
            else if (peek()->type == tokinaizer::TokenType::Ident){
                auto var_token = toks[size];
                consume();
                try_consume(tokinaizer::TokenType::eq, "Expected an equal");
                auto expr = parse_expr(peek().value());
                if (!expr.has_value()) {
                    logging::error("invalid Exepression", peek().value());
                }
                try_consume(tokinaizer::TokenType::semi, "Expected ';'");
                stmts.push_back(NodeStmt{NodeAssign{var_token, expr.value()}});
            }
        }
        return NodeProg{stmts};
    }
    parser() {}
    ~parser() {}

private:
    vector<tokinaizer::Token> tokens;
    size_t size = 0;

    optional<tokinaizer::Token> peek(int offset = 0)
    {
        if (size + offset < tokens.size())
        {
            return tokens[size + offset];
        }
        return {};
    }

    void consume() { size++; }

    void try_consume(tokinaizer::TokenType ty, const string &err)
    {
        if (auto token = peek(); token.has_value() && token->type == ty)
        {
            consume();
        }
        else
        {
            logging::error(err, peek().value());
        }
    }

    optional<NodeExpr> parse_expr(tokinaizer::Token token)
    {
        if (token.type == tokinaizer::TokenType::Number)
        {
            consume();
            return NodeExpr{token.value, false};
        }
        if (token.type == tokinaizer::TokenType::Ident)
        {
            consume();
            return NodeExpr{token.value, true};
        }
        return {};
    }
};
