#include <iostream>
#include <string>
#include <vector>

enum Lexer_Token_Type {
  IDENTIFIER,
  LITTERAL_VALUE,
  STRING_LITTERAL,
  KEYWORD,
  OPERATOR,
  DIRECTIVE,
  PRIMITIVE,
  PAREN_OPEN,
  PAREN_CLOSE,
  SQ_BRACKET_OPEN,
  SQ_BRACKET_CLOSE,
  CURLY_OPEN,
  CURLY_CLOSE,
  DOT,
  COMMA,
  COLON,
  SEMICOLON,
  EQUAL,
  SPACE,
  COMMENTS
};

std::string lexer_token_type_name(Lexer_Token_Type tk) {
  switch (tk) {
    case Lexer_Token_Type::IDENTIFIER:       return "IDENTIFIER"; break;
    case Lexer_Token_Type::LITTERAL_VALUE:   return "LITTERAL_VALUE"; break;
    case Lexer_Token_Type::STRING_LITTERAL:  return "STRING_LITTERAL"; break;
    case Lexer_Token_Type::KEYWORD:          return "KEYWORD"; break;
    case Lexer_Token_Type::OPERATOR:         return "OPERATOR"; break;
    case Lexer_Token_Type::DIRECTIVE:        return "DIRECTIVE"; break;
    case Lexer_Token_Type::PRIMITIVE:        return "PRIMITIVE"; break;
    case Lexer_Token_Type::PAREN_OPEN:       return "PAREN_OPEN"; break;
    case Lexer_Token_Type::PAREN_CLOSE:      return "PARENT_CLOSE"; break;
    case Lexer_Token_Type::SQ_BRACKET_OPEN:  return "SQ_BRACKET_OPEN"; break;
    case Lexer_Token_Type::SQ_BRACKET_CLOSE: return "SQ_BRACKET_CLOSE"; break;
    case Lexer_Token_Type::CURLY_OPEN:       return "CURLY_OPEN"; break;
    case Lexer_Token_Type::CURLY_CLOSE:      return "CURLY_CLOSE"; break;
    case Lexer_Token_Type::EQUAL:            return "EQUAL"; break;
    case Lexer_Token_Type::COMMA:            return "COMMA"; break;
    case Lexer_Token_Type::COLON:            return "COLON"; break;
    case Lexer_Token_Type::SEMICOLON:        return "SEMICOLON"; break;
    case Lexer_Token_Type::DOT:              return "DOT"; break;
    case Lexer_Token_Type::SPACE:            return "SPACE"; break;
    case Lexer_Token_Type::COMMENTS:         return "COMMENTS"; break;
  }
}


unsigned int OPERATOR_FLAG_IS_OPERATOR      = 0x0001;
unsigned int OPERATOR_FLAG_ADD              = 0x0002;
unsigned int OPERATOR_FLAG_SUB              = 0x0004;
unsigned int OPERATOR_FLAG_MULT             = 0x0008;
unsigned int OPERATOR_FLAG_DIV              = 0x0010;
unsigned int OPERATOR_FLAG_POW              = 0x0020;
unsigned int OPERATOR_FLAG_MOD              = 0x0040;
unsigned int OPERATOR_FLAG_LOGICAL_AND      = 0x0080;
unsigned int OPERATOR_FLAG_LOGICAL_OR       = 0x0100;
unsigned int OPERATOR_FLAG_LOGICAL_XOR      = 0x0200;
unsigned int OPERATOR_FLAG_SHIFT_L          = 0x0400;
unsigned int OPERATOR_FLAG_SHIFT_R          = 0x0800;
unsigned int OPERATOR_FLAG_SHIFT_ARITHEMTIC = 0x1000;
unsigned int OPERATOR_FLAG_LT               = 0x2000;
unsigned int OPERATOR_FLAG_GT               = 0x4000;
unsigned int OPERATOR_FLAG_IS_EQUALS        = 0x8000;

unsigned int NUMBER_FLAG_IS_A_NUMBER        = 0x0001;
unsigned int NUMBER_FLAG_IS_SIGNED          = 0x0002;
unsigned int NUMBER_FLAG_IS_HEX             = 0x0004;
unsigned int NUMBER_FLAG_IS_BINARY          = 0x0010;
unsigned int NUMBER_FLAG_IS_NEGATIVE        = 0x0020;
unsigned int NUMBER_FLAG_HAS_A_DOT          = 0x0040;

unsigned int PRIMITIVE_U8                   = 0x0001;
unsigned int PRIMITIVE_U16                  = 0x0002;
unsigned int PRIMITIVE_U32                  = 0x0004;
unsigned int PRIMITIVE_U64                  = 0x0008;
unsigned int PRIMITIVE_S8                   = 0x0010;
unsigned int PRIMITIVE_S16                  = 0x0020;
unsigned int PRIMITIVE_S32                  = 0x0040;
unsigned int PRIMITIVE_S64                  = 0x0080;
unsigned int PRIMITIVE_STR                  = 0x0100;
unsigned int PRIMITIVE_TRUE                 = 0x0200;
unsigned int PRIMITIVE_FALSE                = 0x0400;
unsigned int PRIMITIVE_F32                  = 0x0800;
unsigned int PRIMITIVE_F64                  = 0x1000;
unsigned int PRIMITIVE_TYPE                 = 0x2000;

struct Primitive_Type {
  std::string type;
  unsigned int flag;
}; 

struct Lexer_Token {
  const char *       filename; // 0 terminated
  unsigned long long line_number;
  unsigned long long position;

  char *       text; // position of the token in the file
  unsigned int length;

  Lexer_Token_Type token_type;

  unsigned int operator_flags;
  unsigned int number_flags;
  unsigned int primitive_flags;

  std::string to_string() {
    std::string s = lexer_token_type_name(token_type);
    if (token_type == Lexer_Token_Type::LITTERAL_VALUE && (number_flags & NUMBER_FLAG_IS_NEGATIVE) != 0) {
      s+= " NEG";
    }
    s += " '";
    s.append(text, length);
    s += "'";
    if (token_type == Lexer_Token_Type::OPERATOR) {
      s += ", flags [";
      s += std::bitset<16>(operator_flags).to_string();
      s += "] ";
    }
    s += " at line:";
    s += std::to_string(line_number);
    s += ", col:";
    s += std::to_string(position);
    return s;
  }
};


class Lexer {
  public:
    unsigned long long col, line;
    const char* filename;
    char* buffer;
    int buffer_length;
    int pos;
    std::vector<Lexer_Token> *tokens;
    int flag;

    Lexer(std::string filename) {
      this->filename = filename.c_str();
      tokens = new std::vector<Lexer_Token>();
    }

    /// lex a single keyword to a token and adds it to the list
    /// will manage internal space correctly and return true if all good
    /// or false if an error occured.
    bool lex_keyword(const char* keyword, int length, Lexer_Token_Type token_type = Lexer_Token_Type::KEYWORD);

    ///
    bool lex_directive(const char* keyword, int length);

    ///
    bool lex_primitive(const char* keyword, int length, unsigned int primitive_flags);

    ///
    std::vector<Lexer_Token>* lex_file();

    ///
    void create_token(Lexer_Token_Type token_type, int size, int = 0, unsigned int = 0, unsigned int = 0, unsigned int = 0);

    ///
    void create_token_single_maybe_equals(unsigned int flag);

    ///
    void create_number_token(unsigned int flags = 1);
    
    ///
    bool is_allowed_identifier_char(char c);

    ///
    void try_lex_identifier(char c);
    
    ///
    char peek_next() {
      return buffer[pos+1];
    }
};


