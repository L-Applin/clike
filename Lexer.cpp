/* 
 * CLIKE syntax
 * Tokens:
 * > identifier
 *     - types
 *     - variable names
 *     - function names
 * > Litteral:
 *   - String
 *   - numbers
 *   - array
 * > Keywords
 *   - for
 *   - if - else
 *   - while
 *   - fn
 *   - struct
 *   - data
 * > Litteral type
 *   - u[8, 16 ,32, 64]
 *   - s[8, 16, 32, 64]
 *   - f[32, 62]
 *   - int ( == s64)
 *   - string
 *   - Type
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <ctype.h>
#include "ola.h"
#include "log.h"
#include "Lexer.h"

#define _DEBUG_LEXER true

static std::string known_keywords[] = {
  "fn", "->", "for", "if", "else", "while", "struct", "data", 
};

static Primitive_Type primitive_types[] = {
  { "u8",      PRIMITIVE_U8   }, 
  { "u16",     PRIMITIVE_U16  },      
  { "u32",     PRIMITIVE_U32  },
  { "u64",     PRIMITIVE_U64  },
  { "s8",      PRIMITIVE_S8   }, 
  { "s16",     PRIMITIVE_S16  },
  { "s32",     PRIMITIVE_S32  },
  { "s64",     PRIMITIVE_S64  },
  { "int",     PRIMITIVE_S64  },
  { "float32", PRIMITIVE_F32  }, 
  { "float62", PRIMITIVE_F64  },
  { "string",  PRIMITIVE_STR  },
  { "Type",    PRIMITIVE_TYPE }
};

void Lexer::create_number_token(unsigned int flags) {
  bool found_a_dot = false;
  char c = this->buffer[pos];
  int len = 0;
  while (isdigit(c) || c == '.' || c == '_') {
    if (c=='.') {
      if (found_a_dot) {
        // @todo: report error
        std::stringstream ss;

        return;
      }
      found_a_dot = true;
    }
    // end switch case
    len++;
    c = buffer[pos + len];
  } // end while
  flags |= NUMBER_FLAG_IS_A_NUMBER;
  if (found_a_dot) flags |= NUMBER_FLAG_HAS_A_DOT; 
  create_token(Lexer_Token_Type::LITTERAL_VALUE, len, 0, 0, flags);  
  pos += (len - 1);
  col += (len - 1);
}


void Lexer::create_token_single_maybe_equals(unsigned int flags) {
  char n = peek_next();
  flags |= OPERATOR_FLAG_IS_OPERATOR;
  if (n == '=') {
    flags |= OPERATOR_FLAG_IS_EQUALS;
    create_token(Lexer_Token_Type::OPERATOR, 2, 0, flags);
    this->pos++;
    this->col++;
    return;
  }
  create_token(Lexer_Token_Type::OPERATOR, 1, 0, flags);
}


void Lexer::create_token(Lexer_Token_Type token_type, 
                         int size, 
                         int offset_from_pos, 
                         unsigned int operator_flags, 
                         unsigned int number_flags,
                         unsigned int primitive_flags) 
{
  Lexer_Token t;
  t.filename = this->filename;
  t.line_number = this->line;
  t.position = this->col;
  t.text = &this->buffer[pos + offset_from_pos];
  t.length = size;
  t.token_type = token_type;
  t.operator_flags=operator_flags;
  t.number_flags=number_flags;
  t.primitive_flags=primitive_flags;
  this->tokens->push_back(t);
}


bool Lexer::lex_keyword(const char* keyword, int length, Lexer_Token_Type token_type) {
  if (0 == strncmp(&buffer[pos], keyword, length)) {
    create_token(token_type, length);
    pos = pos + length - 1;
    col = col + length - 1;
    return true;
  }
  return false;
}


bool Lexer::lex_directive(const char* keyword, int length) {
  if (0 == strncmp(&buffer[pos+1], keyword, length)) {
    create_token(Lexer_Token_Type::DIRECTIVE, length + 1);
    pos = pos + length;
    col = col + length;
    return true;
  }
  return false;
}


bool Lexer::lex_primitive(const char* keyword, int length, unsigned int flags) {
  if (0 == strncmp(&buffer[pos], keyword, length)) {
    create_token(Lexer_Token_Type::PRIMITIVE, length, 0, 0, 0, flags);
    pos += length - 1;
    col += length - 1;
    return true;
  }   
  return false;
}


std::vector<Lexer_Token>* Lexer::lex_file() {
  std::cout << "[INFO]  Reading file '" << filename << "'" << std::endl;
  std::ifstream is (filename, std::ifstream::binary);

  if (!is) {
    // @todo : report error reding file
    std::cerr << std::endl << "[ERROR] cannot read file " << filename << ". Stopping lex." << std::endl;
    return tokens;
  }
  // get length of file:
  is.seekg (0, is.end);
  int length = is.tellg();
  is.seekg (0, is.beg);

  this->buffer = new char[length];

  std::cout << "[INFO]  Reading " << length << " characters... " << std::endl;
  is.read (buffer,length);

  if (is) std::cout << "[INFO]  All characters read successfully." << std::endl;
  else    { std::cerr << "[ERROR] error: only " << is.gcount() << " could be read"; return tokens; }
  std::cout << std::endl;
  is.close();

  line = 1;
  col = 0;
  char c;
  bool in_comment = false;
  for (pos = 0; pos < length; pos++, col++) {

    c = buffer[pos];

    if (in_comment) {
      if (c == '\n') {
        col = 0;
        line++;
        in_comment = false;
      }
      continue;
    }
    
    if (isdigit(c)) {
      if (c == '0') {
        char n = peek_next();
        if (n == 'x') {
          // lex hex number
          create_number_token(NUMBER_FLAG_IS_HEX);
        }
      }
      create_number_token(NUMBER_FLAG_IS_A_NUMBER);
      continue;
    }

    // try lex known keywords
    for (std::string s : known_keywords) {
      if(lex_keyword(s.c_str(), s.length()))
        goto cnt;
    }


    // try lex known primitive types
    for (Primitive_Type t : primitive_types) {
      if(lex_primitive(t.type.c_str(), t.type.length(), t.flag))
        goto cnt;
    }
  
    switch (c) {

      case '\n' : {
        col=0;
        line++;
        continue;
      }
                  

      case ' ' : continue;

      case '/' : {
        if (peek_next() != '/') {
          // @todo: report error
          Log::log_error("Unsupported character /", filename, line, col);
        }
        in_comment = true;        
        continue;
      }


      case '#' : {
        if (lex_directive("import", 6))
          continue;
         
        if (lex_directive("assert", 6))
          continue;

      }
                 

      case '.' : {
        create_token(Lexer_Token_Type::DOT, 1);
        continue;
      }

 
      case ',' : {
        create_token(Lexer_Token_Type::COMMA, 1);
        continue;
      }


      case '"' : {
        char closing_quote = '\0';
        int j = 1;
        // special case empty string
        if (pos + j + 1 < length && buffer[pos+j] == '"' && buffer[pos+j+1] != '"') {
          create_token(Lexer_Token_Type::STRING_LITTERAL, 0);
          pos++;
          col++;
          continue;
        }
        bool found_other_quote = false;
        while (pos + j < length) {
          closing_quote = buffer[pos+j];
          if (closing_quote != '"') {
            j++;
            continue;
          }
          // is it an escape quote ?
          if (pos + j + 1 < length && buffer[pos + j + 1] == '"') {
            j+=2;
            continue;
          }
          found_other_quote = true;
          break;
        }
        //@todo: report error
        if (!found_other_quote){
          Log::log_error("cannot find closing quote for string litteral", filename, line, col);
          break;
        }
        int string_length = j - 1;
        create_token(Lexer_Token_Type::STRING_LITTERAL, string_length, 1);
        pos += j;
        col += j; 
        continue;
      }


      case '+': {
        create_token_single_maybe_equals(OPERATOR_FLAG_ADD);
        continue;
      }


      case '-': {
        create_token_single_maybe_equals(OPERATOR_FLAG_SUB);
        continue;
      }


      case '*' : {
        create_token_single_maybe_equals(OPERATOR_FLAG_MULT);
        continue;
      }


      case '^' : {
        create_token_single_maybe_equals(OPERATOR_FLAG_POW);
        continue;
      }


      case '%' : {
        create_token_single_maybe_equals(OPERATOR_FLAG_MOD);
        continue;
      }


      case ':' : {
        create_token(Lexer_Token_Type::COLON, 1, 0);
        continue;
      }

      
      case ';' : {
        create_token(Lexer_Token_Type::SEMICOLON, 1, 0);
        continue;
      }


      case '=' : {
        create_token(Lexer_Token_Type::EQUAL, 1, 0);
        continue;
      }


      case '<' : {
         char n = peek_next();
         if (n == '<') {
           unsigned int flags = OPERATOR_FLAG_SHIFT_L | OPERATOR_FLAG_IS_OPERATOR;
           char next_next = buffer[pos+2];
           if (next_next == '<') {
             create_token(Lexer_Token_Type::OPERATOR, 3, 0, flags |= OPERATOR_FLAG_SHIFT_ARITHEMTIC);
             this->pos++;
             this->col++;
           } else {
             create_token(Lexer_Token_Type::OPERATOR, 2, 0, flags);
           }
           this->pos++;
           this->col++;
         } else {
           create_token_single_maybe_equals(OPERATOR_FLAG_LT);
         }
         continue;
      }


      case '>' : {
         char n = peek_next();
         if (n == '>') {
           unsigned int flags = OPERATOR_FLAG_SHIFT_R | OPERATOR_FLAG_IS_OPERATOR;
           char next_next = buffer[pos+2];
           if (next_next == '>') {
             create_token(Lexer_Token_Type::OPERATOR, 3, 0, flags |= OPERATOR_FLAG_SHIFT_ARITHEMTIC);
             this->pos++;
             this->col++;
           } else {
             create_token(Lexer_Token_Type::OPERATOR, 2, 0, flags);
           }
           this->pos++;
           this->col++;
         } else {
           create_token_single_maybe_equals(OPERATOR_FLAG_GT);
         }
         continue;
      }


      case '(' : {
        create_token(Lexer_Token_Type::PAREN_OPEN, 1);
        continue;
      }


      case ')' : {
        create_token(Lexer_Token_Type::PAREN_CLOSE, 1);
        continue;
      }


      case '[' : {
        create_token(Lexer_Token_Type::SQ_BRACKET_OPEN, 1);
        continue;
      }


      case ']' : {
        create_token(Lexer_Token_Type::SQ_BRACKET_CLOSE, 1);
        continue;
      }


      case '{' : {
        create_token(Lexer_Token_Type::CURLY_OPEN, 1);
        continue;
      }

 
      case '}' : {
        create_token(Lexer_Token_Type::CURLY_CLOSE, 1);
        continue;
      }

    } // end switch case

    // when all else fails, try lex a random identifier 
   try_lex_identifier(c); 
cnt:;
  } // end loop

  return this->tokens;
}    

void Lexer::try_lex_identifier(char c) {
  int j = 1;
  pos++;
  while(is_allowed_identifier_char((c = buffer[pos]))) {
    j++;
    pos++;
  }
  create_token(Lexer_Token_Type::IDENTIFIER, j, -j);
  pos--;
}

bool Lexer::is_allowed_identifier_char(char c) {
  std::string allowed_char = "_1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  return std::string::npos != allowed_char.find(c); 
}

#ifdef _RUN_LEXER
int main() {

  Lexer *l = new Lexer("test.clike");
  l->lex_file();
  std::cout  << "=============" << std::endl
             << "LEX COMPLETED" << std::endl
             << "=============" << std::endl << std::endl
             << l->tokens->size() << " tokens read!" << std::endl;
  int i = 0;
  if (_DEBUG_LEXER) {
    for (Lexer_Token t : *l->tokens) {
      std::cout << std::left << std::setw(6) << ++i << ": "<< t.to_string() << std::endl;
    } 
  } 
  std::cout << std::endl << "=== END ===" << std::endl;
 
  return 0;
}
#endif
