#pragma once
#include "Tokenizer.hpp"
#include "arena.hpp"
#include "logging.hpp"
#include <optional>
#include <variant>
#include <vector>
using namespace std;

class parser {
public:
  struct NodeInt
  {
    tokinaizer::Token int_lit;
  };
  struct NodeIdent
  {
    tokinaizer::Token ident;
  };
  struct BinExpr;
  struct NodeExpr {
    std::variant<NodeInt* , NodeIdent*, BinExpr*> expr;
  };
  struct NodeVar {
    tokinaizer::Token token; // the name
    NodeExpr *expr;          // value
  };
  struct NodeAssign {
    tokinaizer::Token token; // the name
    NodeExpr *expr;          // value
  };
  struct NodeExit {
    NodeExpr *expr;
  };

  struct NodeStmt {
    variant<NodeExit *, NodeVar *, NodeAssign *> stmt;
  };
  struct NodeProg {
    vector<NodeStmt *> stmts;
  };

  struct BinExprPlus {
    NodeExpr *lhs;
    NodeExpr *rhs;
  };
  struct BinExprMulti {
    NodeExpr *lhs;
    NodeExpr *rhs;
  };
  struct BinExpr {
    std::variant<BinExprMulti *, BinExprPlus *> Expr;
  };

  NodeProg parse_stmt(vector<tokinaizer::Token> toks) {
    tokens = toks;
    size = 0;
    vector<NodeStmt *> stmts;
    optional<NodeExpr*> expr;
    while (size < tokens.size()) {
      if (toks[size].type == tokinaizer::TokenType::EXIT) {
        consume();
        try_consume(tokinaizer::TokenType::OPENPREC, "Expected '('");
        auto p = peek().value();
        expr = parse_expr(peek().value());
        if (!expr.has_value()) {
          logging::error("Expected a valid expression", peek().value());
        }
        try_consume(tokinaizer::TokenType::CLOSEPRENC, "Expected ')'");
        try_consume(tokinaizer::TokenType::SEMI, "Expected ';'");
        NodeExit *nodeExit = m_arena.alloc<NodeExit>();
        nodeExit->expr = expr.value();
        NodeStmt *nodeStmt = m_arena.alloc<NodeStmt>();
        nodeStmt->stmt = nodeExit;
        stmts.push_back(nodeStmt);
      } else if (toks[size].type == tokinaizer::TokenType::VAR) {
        consume();
        auto var_token = toks[size];
        try_consume(tokinaizer::TokenType::IDENT, "Expected an identfire");
        try_consume(tokinaizer::TokenType::EQ, "Expected '='");
        optional<NodeExpr*> expr = parse_expr(peek().value());
        if (!expr.has_value()) {
          logging::error("invalid Exepression", peek().value());
        }
        try_consume(tokinaizer::TokenType::SEMI, "Expected ';'");
        NodeVar *nodeVar = m_arena.alloc<NodeVar>();
        nodeVar->token = var_token;
        nodeVar->expr = expr.value();
        NodeStmt *nodeStmt = m_arena.alloc<NodeStmt>();
        nodeStmt->stmt = nodeVar;
        stmts.push_back(nodeStmt);

      } else if (peek()->type == tokinaizer::TokenType::IDENT) {
        auto var_token = toks[size];
        consume();
        try_consume(tokinaizer::TokenType::EQ, "Expected an equal");
        auto expr = parse_expr(peek().value());
        if (!expr.has_value()) {
          logging::error("invalid Exepression", peek().value());
        }
        try_consume(tokinaizer::TokenType::SEMI, "Expected ';'");
        NodeAssign *nodeAssign = m_arena.alloc<NodeAssign>();
        nodeAssign->token = var_token;
        nodeAssign->expr = expr.value();
        NodeStmt *nodeStmt = m_arena.alloc<NodeStmt>();
        nodeStmt->stmt = nodeAssign;
        stmts.push_back(nodeStmt);
      }
    }
    return NodeProg{stmts};
  }
  parser() : m_arena(1024 * 1024 * 4) {}
  ~parser() {}

private:
  vector<tokinaizer::Token> tokens;
  size_t size = 0;

  optional<tokinaizer::Token> peek(int offset = 0) {
    if (size + offset < tokens.size()) {
      return tokens[size + offset];
    }
    return {};
  }

  void consume() { size++; }

  void try_consume(tokinaizer::TokenType ty, const string &err) {
    if (auto token = peek(); token.has_value() && token->type == ty) {
      consume();
    } else {
      logging::error(err, peek().value());
    }
  }

  optional<NodeExpr*> parse_expr(tokinaizer::Token token) {
    if (token.type == tokinaizer::TokenType::NUMBER) {
      consume();
      NodeInt* nodeInt = m_arena.alloc<NodeInt>();
      nodeInt->int_lit = token;
      NodeExpr* nodeExpr = m_arena.alloc<NodeExpr>();
      nodeExpr->expr = nodeInt;
      return nodeExpr;
    }
    if (token.type == tokinaizer::TokenType::IDENT) {
      consume();

      NodeIdent* nodeIdent = m_arena.alloc<NodeIdent>();
      nodeIdent->ident = token;
      NodeExpr* nodeExpr = m_arena.alloc<NodeExpr>();
      nodeExpr->expr = nodeIdent;
      return nodeExpr;
    }
    return {};
  }

  ArenaAllocator m_arena;
};
