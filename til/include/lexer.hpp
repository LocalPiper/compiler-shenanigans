#pragma once
#include <cstdlib>
#include <string>
#include <vector>

namespace lexer {

enum class TokenType {
  Number,
  Identifier,

  Plus,
  Minus,
  Star,
  Slash,
  Assign,
  Ampersand,
  Greater,
  Dot,
  Comma,
  Colon,
  Semicolon,
  Equal,

  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,

  Var,
  Input,
  Output,
  If,
  Else,
  While,
  Return,
  Alloc,
  Null,

  End
};

struct Token {
  TokenType type;
  std::string lexeme;
};

class Lexer {
public:
  void setSource(std::string source);
  std::vector<Token> tokenize();

private:
  std::string source;
  std::size_t pos = 0;

  Token readNumber();
  Token readIdentifier();
  Token readOperator();
};

const std::string tokenName(lexer::TokenType type);

} // namespace lexer
