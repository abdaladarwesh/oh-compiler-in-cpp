#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

class tokinaizer
{
public:
    enum class TokenType
    {
        Exit,
        Number,
        OpenPrenc,
        ClosePrenc,
        semi,
        Var,
        Ident,
        eq
    };
    struct Token
    {
        TokenType type;
        string value;
        size_t line;
    };
    size_t line = 1;

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
                    tokens.push_back({TokenType::Exit, word, line});
                }
                else if (word == "var" || word == "let")
                { // allow let as alias
                    tokens.push_back({TokenType::Var, word, line});
                    i++;
                }
                else
                {
                    tokens.push_back({TokenType::Ident, word, line});
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
                tokens.push_back({TokenType::Number, word, line});
                continue;
            }
            if (src[i] == '(')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::OpenPrenc, word, line});
                i++;
                continue;
            }
            if (src[i] == ')')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::ClosePrenc, word, line});
                i++;
                continue;
            }
            if (src[i] == ';')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::semi, word, line});
                i++;
                continue;
            }
            if (src[i] == '=')
            {
                string word;
                word += src[i];
                tokens.push_back({TokenType::eq, word, line});
                i++;
                continue;
            }
            
        }
        return tokens;
    }

    tokinaizer() {}
    ~tokinaizer() {}
};
