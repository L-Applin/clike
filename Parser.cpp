#include"Parser.h"

void report_error(std::string msg, Lexer_Token* tk) {
  // @todo better error reporting and stop ?
  printf("%s %llu:%llu - %s", tk->filename, tk->line_number, tk->position, msg.c_str());
}


Ast_Variable * make_ast_variable(std::string name, Ast_Type *ast_type) {
  Ast_Variable *ast = new Ast_Variable;
  ast->name = name;
  ast->type = ast_type;
  ast->type_is_known = ast_type != NULL;
  return ast;
}


Ast_Type* make_primitive_type(Lexer_Token *tk) {
  assert(tk != NULL);
  assert(tk->primitive_flags!=0);
  Ast_Type *ast = new Ast_Type;
  ast->flag = AST_TYPE_PRIMITIVE;
  ast->primitive_id = tk->primitive_flags;
  return ast;
}


Ast_Type* make_type(Lexer_Token* tk) {
  assert(tk!=NULL);
  assert(tk->token_type == Lexer_Token_Type::IDENTIFIER);
  Ast_Type *ast = new Ast_Type;
  ast->flag = AST_TYPE_STRUCT;
  return ast;
}


void Parser::ensure_maximum_token_size() {
  if (index == token_size-1){
    //@todo report error
    std::cerr << "End of token stream while parsing file " << this->filename << std::endl;
  }
}


void Parser::eat_token() {
  ensure_maximum_token_size();
  this->current=&tokens[++index];
}


Lexer_Token* Parser::peek_next() {
  ensure_maximum_token_size();
  return &tokens[index+1];
}


///
Ast_Type* Parser::parse_type(int number_of_token_to_parse) {
  Ast_Type* ast_type;
  if (number_of_token_to_parse == 1) {
    if (current->token_type == Lexer_Token_Type::PRIMITIVE) {
      ast_type = make_primitive_type(current);
    }
    if( current->token_type == Lexer_Token_Type::IDENTIFIER) {
      ast_type = make_type(current);
    }
  }
  return NULL;
}


Ast_Type* Parser::parse_function_type() {
    
}


Ast_Assignement* Parser::parse_assignement() {
  Lexer_Token* var_token = current;
  // current pos must be variable name
  if (current->token_type!=Lexer_Token_Type::IDENTIFIER){
    return NULL; // @todo report error?
  }
  eat_token();
  if (current->token_type != Lexer_Token_Type::COLON){
    return NULL; // @todo report error?
  }
  eat_token();
  auto ast_type = parse_type();
  if (!ast_type) {
    // @todo report_error?
    return NULL;
  }
  if (current->token_type != Lexer_Token_Type::EQUAL) {
    //@todo report error ?
    return NULL;
  }

  eat_token(); 
  
  auto ast_val = parse_expr();
  auto ast_variable = make_ast_variable(var_token->text, ast_type);
  auto ast = new Ast_Assignement;
  ast->var = ast_variable;
  ast->value = ast_val;
  return ast;
}
