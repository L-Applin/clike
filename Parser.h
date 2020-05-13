#include "Lexer.h"
#include "Ast.h"

class Parser {
  public: 
    std::string filename;
    int index;
    int token_size;
    Lexer_Token* current;
    Lexer_Token* tokens;

    Parser(Lexer_Token* tokens, int size, std::string filename) {
      this->tokens=tokens;
      this->token_size=size;
      this->filename=filename;
    }

    void ensure_maximum_token_size();
    Lexer_Token* peek_next();
    void eat_token();


    Ast_Type*          parse_type(int);
    Ast_Type*          parse_function_type();
    Ast_Expr*          parse_expr();
    Ast_Assignement*   parse_assignement();


};

