#ifndef _PARSER_TYPES_H_
#define _PARSER_TYPES_H_

enum tokentype
{
   Error,
   And,
   Or,
   Not,
   EndOfText,
   OpenParenthesis,
   ClosedParenthesis,
   Number,
   BoolChar
};
typedef enum tokentype TokenType;

struct token
{
   TokenType  Type;
   uint8_t    Value;
   char       Symbol;
};
typedef struct token Token;

struct parser
{
    Token m_crtToken;
    const char* m_Text;
    size_t m_Index;
    bool m_success;
    char m_errorMessage[30];
};
typedef struct parser Parser;

enum astnodetype
{
   Undefined,
   OperatorAnd,
   OperatorOr,
   UnaryNot,
   FunctionID,
   BoolValue
};
typedef enum astnodetype ASTNodeType;

struct astnode
{
   ASTNodeType Type;
   bool      Value;
   struct astnode*    Left;
   struct astnode*    Right;
};
typedef struct astnode ASTNode;

#endif
