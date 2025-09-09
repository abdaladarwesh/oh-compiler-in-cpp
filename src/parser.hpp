#pragma once
#include "Tokenizer.hpp"
#include "arena.hpp"
#include "logging.hpp"
#include <cassert>
#include <cstdlib>
#include <optional>
#include <variant>
#include <vector>
using namespace std;

class parser {
public:
  struct NodeInt {
    tokinaizer::Token int_lit;
  };
  struct NodeIdent {
    tokinaizer::Token ident;
  };

  struct NodeTerm {
    std::variant<NodeInt *, NodeIdent *> var;
  };

  struct BinExpr;
  struct NodeExpr {
    std::variant<NodeTerm *, BinExpr *> var;
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
    variant<NodeExit *, NodeVar *, NodeAssign *> var;
  };
  struct NodeProg {
    vector<NodeStmt *> stmts;
  };

  struct NodeBinExprPlus {
    NodeExpr *lhs;
    NodeExpr *rhs;
  };
  struct NodeBinExprMulti {
    NodeExpr *lhs;
    NodeExpr *rhs;
  };
  struct NodeBinExprSub {
    NodeExpr *lhs;
    NodeExpr *rhs;
  };
  struct NodeBinExprDev {
    NodeExpr *lhs;
    NodeExpr *rhs;
  };
  struct BinExpr {
    std::variant<NodeBinExprMulti *, NodeBinExprPlus *, NodeBinExprDev* , NodeBinExprSub*> var;
  };

  NodeProg parse_stmt(vector<tokinaizer::Token> toks) {
    tokens = toks;
    size = 0;
    vector<NodeStmt *> stmts;
    optional<NodeExpr *> expr;
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
        nodeStmt->var = nodeExit;
        stmts.push_back(nodeStmt);
      } else if (toks[size].type == tokinaizer::TokenType::VAR) {
        consume();
        auto var_token = toks[size];
        try_consume(tokinaizer::TokenType::IDENT, "Expected an identfire");
        try_consume(tokinaizer::TokenType::EQ, "Expected '='");
        optional<NodeExpr *> expr = parse_expr(peek().value());
        if (!expr.has_value()) {
          logging::error("invalid Exepression var", peek().value());
        }
        try_consume(tokinaizer::TokenType::SEMI, "Expected ';'");
        NodeVar *nodeVar = m_arena.alloc<NodeVar>();
        nodeVar->token = var_token;
        nodeVar->expr = expr.value();
        NodeStmt *nodeStmt = m_arena.alloc<NodeStmt>();
        nodeStmt->var = nodeVar;
        stmts.push_back(nodeStmt);

      } else if (peek()->type == tokinaizer::TokenType::IDENT) {
        auto var_token = toks[size];
        consume();
        try_consume(tokinaizer::TokenType::EQ, "Expected an equal");
        auto expr = parse_expr(peek().value());
        if (!expr.has_value()) {
          logging::error("invalid Exepression ident", peek().value());
        }
        try_consume(tokinaizer::TokenType::SEMI, "Expected ';'");
        NodeAssign *nodeAssign = m_arena.alloc<NodeAssign>();
        nodeAssign->token = var_token;
        nodeAssign->expr = expr.value();
        NodeStmt *nodeStmt = m_arena.alloc<NodeStmt>();
        nodeStmt->var = nodeAssign;
        stmts.push_back(nodeStmt);
      }
    }
    return NodeProg{stmts};
  }
  parser() : m_arena(1024 * 1024 * 4) {}
  ~parser() {}

private:
  optional<NodeTerm *> parse_term(tokinaizer::Token token) {
    if (token.type == tokinaizer::TokenType::NUMBER) {
      NodeInt *nodeInt = m_arena.alloc<NodeInt>();
      nodeInt->int_lit = token;
      NodeTerm *nodeTerm = m_arena.alloc<NodeTerm>();
      nodeTerm->var = nodeInt;
      consume();
      return nodeTerm;
    }
    if (token.type == tokinaizer::TokenType::IDENT) {
      consume();
      NodeIdent *nodeIdent = m_arena.alloc<NodeIdent>();
      nodeIdent->ident = token;
      NodeTerm *nodeTerm = m_arena.alloc<NodeTerm>();
      nodeTerm->var = nodeIdent;
      return nodeTerm;
    } else {
      return {};
    }
  }

  optional<NodeExpr *> parse_expr(tokinaizer::Token token, const int min_prec = 0) {

    std::optional<NodeTerm*> term_lhs = parse_term(token);
    optional<int> prec;
    if (!term_lhs.has_value())
      return {};
    NodeExpr* expr_lhs = m_arena.alloc<NodeExpr>();
    expr_lhs->var = term_lhs.value();
    while (true) {
      auto curr_token = peek();
      if (curr_token.has_value())
      {
         prec = tokinaizer::op_prec(curr_token.value().type);
         if (!prec.has_value() || prec < min_prec){
           break;
        }
      } 
      else {
        break;
      }
      const optional<tokinaizer::Token> cur_t = peek().value();
      consume();
      int next_min_prec =  prec.value() + 1;
      auto expr_rhs = parse_expr(peek().value(), next_min_prec);
      if (!expr_rhs.has_value()){
        logging::error("Unable to parse an exepression", token);
      }
      BinExpr* bin_expr = m_arena.alloc<BinExpr>();
      NodeExpr* lhs_expr = m_arena.alloc<NodeExpr>();
      if (cur_t->type == tokinaizer::TokenType::PLUS){
        lhs_expr->var = expr_lhs->var;
        NodeBinExprPlus* add = m_arena.alloc<NodeBinExprPlus>();
        add->rhs = expr_rhs.value();
        add->lhs = lhs_expr;
        bin_expr->var = add;
      }
      else if (cur_t->type == tokinaizer::TokenType::MULTI){
        lhs_expr->var = expr_lhs->var;
        NodeBinExprMulti* multi = m_arena.alloc<NodeBinExprMulti>();
        multi->rhs = expr_rhs.value();
        multi->lhs = lhs_expr;
        bin_expr->var = multi;
      }
      else if (cur_t->type == tokinaizer::TokenType::SLASH){
        lhs_expr->var = expr_lhs->var;
        NodeBinExprDev* dev = m_arena.alloc<NodeBinExprDev>();
        dev->rhs = expr_rhs.value();
        dev->lhs = lhs_expr;
        bin_expr->var = dev;
      }
      else if (cur_t->type == tokinaizer::TokenType::MINUS){
        lhs_expr->var = expr_lhs->var;
        NodeBinExprSub* sub = m_arena.alloc<NodeBinExprSub>();
        sub->rhs = expr_rhs.value();
        sub->lhs = lhs_expr;
        bin_expr->var = sub;
      }
      else{
        assert(false); // Unreachable;
      }
      expr_lhs->var = bin_expr;
    }
    return expr_lhs;
  }

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
      logging::error(err, peek(-1).value());
    }
  }

  vector<tokinaizer::Token> tokens;
  size_t size = 0;
  ArenaAllocator m_arena;
};
