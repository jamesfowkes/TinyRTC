#ifndef _AST_NODE_H_
#define _AST_NODE_H_

#include "parser_types.h"

/*
 * Public Function Declarations
 */

void AST_Init(void);
ASTNode* AST_CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right);
ASTNode* AST_CreateUnaryNode(Parser * parser, ASTNode* child);
ASTNode* AST_CreateNodeBoolValue(Parser * parser, bool value);
ASTNode* AST_CreateNodeBoolFunction(Parser * parser, uint8_t fid);

#endif
