#include "lexer.hpp"
#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
using namespace lexer;
const std::unordered_map<std::string, TokenType> operators = {
    {"+", TokenType::Plus},      {"-", TokenType::Minus},
    {"*", TokenType::Star},      {"/", TokenType::Slash},
    {">", TokenType::Greater},   {"&", TokenType::Ampersand},
    {":", TokenType::Colon},     {";", TokenType::Semicolon},
    {".", TokenType::Dot},       {",", TokenType::Comma},
    {"(", TokenType::LeftParen}, {")", TokenType::RightParen},
    {"{", TokenType::LeftBrace}, {"}", TokenType::RightBrace},
    {"=", TokenType::Assign},    {"==", TokenType::Equal}};

const std::unordered_map<std::string, TokenType> keywords = {
    {"var", TokenType::Var},       {"input", TokenType::Input},
    {"output", TokenType::Output}, {"if", TokenType::If},
    {"else", TokenType::Else},     {"while", TokenType::While},
    {"return", TokenType::Return}};

} // namespace
namespace lexer {

std::unordered_map<TokenType, std::string> getTokenMap() {
  std::unordered_map<TokenType, std::string> mp;
  for (auto [s, t] : operators)
    mp[t] = s;
  for (auto [s, t] : keywords)
    mp[t] = s;
  return mp;
}

const std::string tokenName(TokenType token) {
  static std::unordered_map<TokenType, std::string> mp = getTokenMap();
  auto it = mp.find(token);
  if (it == mp.end())
    throw std::runtime_error("No token for given type");
  return it->second;
}

void Lexer::setSource(std::string source) {
  this->source = std::move(source);
  pos = 0;
}

Token Lexer::readNumber() {
  std::size_t start = pos;
  while (pos < source.size() &&
         std::isdigit(static_cast<unsigned char>(source[pos]))) {
    ++pos;
  }
  return {TokenType::Number, source.substr(start, pos - start)};
}

Token Lexer::readIdentifier() {
  std::size_t start = pos;
  while (pos < source.size() &&
         std::isalnum(static_cast<unsigned char>(source[pos]))) {
    ++pos;
  }
  std::string id = source.substr(start, pos - start);
  if (auto it = keywords.find(id); it != keywords.end()) {
    return {it->second, id};
  }
  return {TokenType::Identifier, id};
}

Token Lexer::readOperator() {
  if (pos + 1 < source.size()) {
    std::string two = source.substr(pos, 2);

    if (auto it = operators.find(two); it != operators.end()) {
      pos += 2;
      return {it->second, two};
    }
  }

  std::string one(1, source[pos]);
  if (auto it = operators.find(one); it != operators.end()) {
    ++pos;
    return {it->second, one};
  }

  throw std::runtime_error("Expected operator, got " + one);
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  while (pos < source.size()) {
    char c = source[pos];

    if (std::isspace(static_cast<unsigned char>(c))) {
      ++pos;
      continue;
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
      tokens.push_back(readNumber());
      continue;
    }

    if (std::isalnum(static_cast<unsigned char>(c))) {
      tokens.push_back(readIdentifier());
      continue;
    }

    tokens.push_back(readOperator());
  }

  return tokens;
}

} // namespace lexer
