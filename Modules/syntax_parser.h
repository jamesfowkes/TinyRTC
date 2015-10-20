#ifndef _SYNTAX_PARSER_H_
#define _SYNTAX_PARSER_H_

#include "parser_types.h"

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

typedef bool (*BOOLFUNCTION)(void);

typedef struct astnode ASTNode;

/*
 * Public Function Declarations
 */

void LEP_Init(void);
bool LEP_Evaluate(ASTNode *);
ASTNode * LEP_Parse(Parser * parser, const char* text);
void LEP_RegisterFunction(uint8_t fid, BOOLFUNCTION fn);

#endif
