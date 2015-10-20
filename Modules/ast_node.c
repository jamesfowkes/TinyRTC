/* ast_node.c
 * Handler for creation and storage of ast nodes
 */

/*
 * C Library Includes
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

/*
 * Local Application Includes
 */
 
#include "syntax_parser.h"
#include "ast_node.h"

/*
 * Defines and Typedefs
 */

#define MAX_NODE_NUMBER (128)

/*
 * Private Variables
 */
static ASTNode s_freeNodes[MAX_NODE_NUMBER];
static uint8_t s_freeNodeIndex = 0;

/*
 * getNextFreeNode
 * Returns the next free node from the pool
 */
static ASTNode* getNextFreeNode(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    if (s_freeNodeIndex < MAX_NODE_NUMBER)
    {
        return &s_freeNodes[s_freeNodeIndex++];
    }
    else
    {
        parser->m_success = false;
        sprintf(parser->m_errorMessage, "Maximum number of nodes (%d) exceeded", MAX_NODE_NUMBER);
        return NULL;
    }
}

/*
 * AST_Init
 * Initialises this module
 */
void AST_Init(void)
{
    s_freeNodeIndex = 0;
}

/*
 * AST_CreateNode
 * Creates a AND or OR node with the provided left and right nodes
 */
ASTNode* AST_CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    ASTNode* node = getNextFreeNode(parser);

    if (node)
    {
        node->Type = type;
        node->Left = left;
        node->Right = right;
        DEBUG( printf("%s (%s)\n", __func__, type == OperatorOr ? "or" : "and") );
    }

    return node;
}

/*
 * AST_CreateUnaryNode
 * Creates a NOT node with the provided child
 */
ASTNode* AST_CreateUnaryNode(Parser * parser, ASTNode* child)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    ASTNode* node = getNextFreeNode(parser);

    if (node)
    {
        node->Type = UnaryNot;
        node->Left = child;
        node->Right = NULL;
        
        DEBUG( printf("%s (not)\n", __func__) );
    }

    return node;
}

/*
 * AST_CreateNodeBoolFunction
 * Creates a node representing a boolean function to be evaulated
 */
ASTNode* AST_CreateNodeBoolFunction(Parser * parser, uint8_t value)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    ASTNode* node = getNextFreeNode(parser);

    if (node)
    {
        node->Type = FunctionID;
        node->Value = value;
        
        DEBUG( printf("%s (%d)\n", __func__, value) );
    }

    return node;
}

/*
 * AST_CreateNodeBoolValue
 * Creates a node representing either True or False
 */
ASTNode* AST_CreateNodeBoolValue(Parser * parser, bool value)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    ASTNode* node = getNextFreeNode(parser);

    if (node)
    {
        node->Type = FunctionID;
        node->Value = value;

        DEBUG( printf("%s (%s)\n", __func__, value ? "true" : "false") );
    }

    return node;
}
