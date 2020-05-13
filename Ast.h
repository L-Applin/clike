#include <string> 

unsigned int AST_TYPE_PRIMITIVE = 0x1;
unsigned int AST_TYPE_STRUCT    = 0x2;
unsigned int AST_TYPE_FUNCTION  = 0x3;
unsigned int AST_TYPE_TYPE      = 0x4;

struct Type {

};


struct Ast {
  
};


struct Ast_Expr : Ast {

};


struct Ast_Type : Ast {
  unsigned int flag;
  unsigned int primitive_id = 0;    
};


template<int N, int M>
struct Ast_Function_Type : Ast_Type {
  bool is_var_arg; 
  int arg_amount = N;
  int return_valu_amount = M;
  Ast_Type arg_type_list[N];
  Ast_Type ret_type_list[M];
};

struct Ast_Variable : Ast {
  std::string name;
  
  Ast_Type* type;
  bool type_is_known;
};


struct Ast_Statement : Ast {

};


struct Ast_Assignement : Ast {
  Ast_Variable* var;
  Ast_Expr*     value;
};


struct Ast_Decl : Ast {

};
