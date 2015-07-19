#ifndef _SYNTAX_PARSER_H_
#define _SYNTAX_PARSER_H_

/*
 * Defines and Typedefs
 */

#define ON_PARSER_ERROR_EXIT_EARLY(parser) { if (!parser->m_success) { return; }}
#define ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, rtn) { if (!parser->m_success) { return rtn; }}

#ifdef _DEBUG
#define DEBUG(x) {x;}
#else
#define DEBUG(x) {}
#endif

enum TokenType
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

struct Token
{
   TokenType  Type;
   uint8_t    Value;
   char       Symbol;
};

struct Parser
{
    Token m_crtToken;
    const char* m_Text;
    size_t m_Index;
    bool m_success;
    char m_errorMessage[30];
};

typedef bool (*BOOLFUNCTION)(void);

/*
 * Public Function Declarations
 */

void LEP_Init(void);
bool LEP_Evaluate(ASTNode* ast);
ASTNode * LEP_Parse(Parser * parser, const char* text);
void LEP_RegisterFunction(uint8_t fid, BOOLFUNCTION fn);

#endif
