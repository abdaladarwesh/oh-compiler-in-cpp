#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

class tokinaizer
{
public:
    enum class TokenType
    {
        EXIT,
        NUMBER,
        OPENPREC,
        CLOSEPRENC,
        SEMI,
        VAR,
        IDENT,
        EQ,
        PLUS,
        MULTI,
        SLASH,
        MINUS
    };
    struct Token
    {
        TokenType type;
        string value;
        size_t line;
    };
    size_t line = 1;

    static std::optional<int> op_prec (const TokenType t){
        switch (t) {
            case TokenType::PLUS:
            case TokenType::MINUS:
                return 2;
            case TokenType::MULTI:
            case TokenType::SLASH:
                return 3;
            default:
                return {};
        }
    }

    vector<Token> tokenize(const string &src)
    {
        vector<Token> tokens;
        for (size_t i = 0; i < src.length();)
        {
            if (src[i] == '\n')
            {
                line++;
                i++;
                continue;
            }
            if (isspace(src[i]))
            {
                i++;
                continue;
            }
            if (isalpha(src[i]))
            {
                string word;
                while (isalpha(src[i]))
                {
                    word += src[i];
                    i++;
                }
                if (word == "exit")
                {
                    tokens.push_back({TokenType::EXIT, word, line});
                }
                else if (word == "var" || word == "let")
                { // allow let as alias
                    tokens.push_back({TokenType::VAR, word, line});
                    i++;
                }
                else
                {
                    tokens.push_back({TokenType::IDENT, word, line});
                }
                continue;
            }
            if (isdigit(src[i]))
            {
                string word;
                while (isdigit(src[i]))
                {
                    word += src[i];
                    i++;
                }
                tokens.push_back({TokenType::NUMBER, word, line});
                continue;
            }
            if (src[i] == '(')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::OPENPREC, word, line});
                i++;
                continue;
            }
            if (src[i] == ')')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::CLOSEPRENC, word, line});
                i++;
                continue;
            }
            if (src[i] == ';')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::SEMI, word, line});
                i++;
                continue;
            }
            if (src[i] == '=')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::EQ, word, line});
                i++;
                continue;
            }
            if (src[i] == '+')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::PLUS, word, line});
                i++;
                continue;
            }
            if (src[i] == '*')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::MULTI, word, line});
                i++;
                continue;
            }
            if (src[i] == '/')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::SLASH, word, line});
                i++;
                continue;
            }
            if (src[i] == '-')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::MINUS, word, line});
                i++;
                continue;
            }
            
        }
        return tokens;
    }

    tokinaizer() {}
    ~tokinaizer() {}
};
