#include "Lexer.h"
#include "Ast.h"

class Parser {
  public: 
    std::vector<Lexer_Token>* tokens;

    Ast_Expr* parse_expr();
};

