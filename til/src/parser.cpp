#include "parser.hpp"
#include "lexer.hpp"
#include <iterator>
#include <vector>

namespace parser {

void Parser::addTokens(std::vector<lexer::Token> newTokens) {
  tokens.insert(tokens.end(), std::make_move_iterator(newTokens.begin()),
                std::make_move_iterator(newTokens.end()));
}
} // namespace parser
