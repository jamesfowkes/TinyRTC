/* syntax_parser.c
 * Parses logical expressions into abstract syntax trees
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

#define MAX_FUNCTIONS (16)

static ASTNode* expression(Parser * parser);
static ASTNode* expression1(Parser * parser);
static ASTNode* term(Parser * parser);
static ASTNode* term1(Parser * parser);
static ASTNode* Factor(Parser * parser);
static void match(Parser * parser, char expected);
static void skipWhitespaces(Parser * parser);
static void getNextToken(Parser * parser);

static bool defaultFn(void) { return false; }

static BOOLFUNCTION s_functions[MAX_FUNCTIONS];

static ASTNode* expression(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG ( printf("%s\n", __func__) );

    ASTNode* tnode = term(parser);
    ASTNode* e1node = expression1(parser);

    return AST_CreateNode(parser, OperatorAnd, tnode, e1node);
}

static ASTNode* expression1(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    ASTNode* tnode;
    ASTNode* e1node;

    switch(parser->m_crtToken.Type)
    {
        case And:
            getNextToken(parser);
            tnode = term(parser);
            e1node = expression1(parser);
            return AST_CreateNode(parser, OperatorAnd, e1node, tnode);
        break;
        default:
            break;
    }

    return AST_CreateNodeBoolValue(parser, true);
}

static ASTNode* term(Parser * parser)
{
    DEBUG( printf("%s\n", __func__) );

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    ASTNode* fnode = Factor(parser);
    ASTNode* t1node = term1(parser);

    return AST_CreateNode(parser, OperatorOr, fnode, t1node);
}

static ASTNode* term1(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    ASTNode* fnode;
    ASTNode* t1node;

    switch(parser->m_crtToken.Type)
    {
        case Or:
            getNextToken(parser);
            fnode = Factor(parser);
            t1node = term1(parser);
            return AST_CreateNode(parser, OperatorOr, t1node, fnode);
        break;
        default:
            break;
    }

    return AST_CreateNodeBoolValue(parser, false);
}

static ASTNode* Factor(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    DEBUG( printf("%s\n", __func__) );

    ASTNode* node;
    uint8_t value;

    switch(parser->m_crtToken.Type)
    {
        case OpenParenthesis:
            getNextToken(parser);
            node = expression(parser);
            match(parser, ')');
            return node;

        case Not:
            getNextToken(parser);
            node = Factor(parser);
            return AST_CreateUnaryNode(parser, node);

        case Number:
            value = parser->m_crtToken.Value;
            getNextToken(parser);
            return AST_CreateNodeBoolFunction(parser, value);

        case BoolChar:
            value = parser->m_crtToken.Value;
            getNextToken(parser);
            return AST_CreateNodeBoolValue(parser, (bool)value);
        default:
            parser->m_success = false;
            snprintf(parser->m_errorMessage, sizeof(parser->m_errorMessage), "%s: At position %d unexpected token '%c'",
                __func__, (int)parser->m_Index, parser->m_crtToken.Symbol);
            return NULL;
    }
}


/* match
 * Check that the parser is at the expected char and jump over it (do not process)
 */
static void match(Parser * parser, char expected)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);

    if(parser->m_Text[parser->m_Index-1] == expected)
    {
        getNextToken(parser);
    }
    else
    {
        parser->m_success = false;
        snprintf(parser->m_errorMessage, sizeof(parser->m_errorMessage), "At position %d unexpected token '%c' (Line %d)",
            (int)parser->m_Index, expected, __LINE__);
    }
}

/* skipWhitespaces
 * Jump the parser ahead over whitespace
 */
static void skipWhitespaces(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);
    while(isspace(parser->m_Text[parser->m_Index])) parser->m_Index++;
}

/* GetInteger
 * From the current location in the parse text, return an integer.
 */
static uint8_t GetInteger(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, 0);

    skipWhitespaces(parser);

    int index = parser->m_Index;
    while(isdigit(parser->m_Text[parser->m_Index])) parser->m_Index++;

    if(parser->m_Index - index == 0)
    {
        parser->m_success = false;
        snprintf(parser->m_errorMessage, sizeof(parser->m_errorMessage), "Number expected but not found at position %d!", (int)parser->m_Index);
        return 0;
    }

    char buffer[32] = {0};
    memcpy(buffer, &parser->m_Text[index], parser->m_Index - index);

    unsigned long result = atol(buffer);

    if (result > 255)
    {
        parser->m_success = false;
        snprintf(parser->m_errorMessage, sizeof(parser->m_errorMessage), "Function ID %lu exceeds 255!", result);
        return 0;
    }

    if (result >= MAX_FUNCTIONS)
    {
        parser->m_success = false;
        snprintf(parser->m_errorMessage, sizeof(parser->m_errorMessage), "Function ID %lu exceeds maximum allocation of MAX_FUNCTIONS!", result);
        return 0;
    }

    return (uint8_t)result;
}

/* getNextToken
 * Skip to the next token in the input string and tag appropriately for processing
 */
static void getNextToken(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);

    // ignore white spaces
    skipWhitespaces(parser);

    parser->m_crtToken.Value = 0;
    parser->m_crtToken.Symbol = 0;
    parser->m_crtToken.Type = Error;

    DEBUG( printf("Next token: '%c'\n", parser->m_Text[parser->m_Index]) );

    // test for the end of text
    if(parser->m_Text[parser->m_Index] == 0)
    {
        parser->m_crtToken.Type = EndOfText; // No more text to parse
        return;
    }

    // if the current character is a digit then parse to the end of the number
    if(isdigit(parser->m_Text[parser->m_Index]))
    {
        parser->m_crtToken.Type = Number;
        parser->m_crtToken.Value = GetInteger(parser);
        return;
    }

    // T and F represent constant True and False values
    if(parser->m_Text[parser->m_Index] == 'T' || parser->m_Text[parser->m_Index] == 'F')
    {
        parser->m_crtToken.Type = BoolChar;
        parser->m_crtToken.Value = (uint8_t)(parser->m_Text[parser->m_Index] == 'T');
        parser->m_Index++;
        return;
    }

    // check if the current character is an operator or parentheses
    switch(parser->m_Text[parser->m_Index])
    {
        case '&': parser->m_crtToken.Type = And; break;
        case '|': parser->m_crtToken.Type = Or; break;
        case '!': parser->m_crtToken.Type = Not; break;
        case '(': parser->m_crtToken.Type = OpenParenthesis; break;
        case ')': parser->m_crtToken.Type = ClosedParenthesis; break;
        default: break;
    }

    if(parser->m_crtToken.Type != Error)
    {
        parser->m_crtToken.Symbol = parser->m_Text[parser->m_Index];
        parser->m_Index++;
    }
    else
    {
        parser->m_success = false;
        if (parser->m_Text[parser->m_Index])
        {
            snprintf(parser->m_errorMessage, sizeof(parser->m_errorMessage), "%s: At position %d unexpected token '%c'",
                __func__, (int)parser->m_Index, parser->m_Text[parser->m_Index]);
        }
    }
}

/* LEP_Init
 * Initialises this module
 */
void LEP_Init(void)
{
    uint8_t i;
    for (i = 0; i < MAX_FUNCTIONS; ++i)
    {
        // Ensure all functions start off pointing at the root
        s_functions[i] = defaultFn;
    }
}

/* LEP_Evaluate
 * Evaluate the AST starting at the given root node.
 * Called recursively for each node
 */
bool LEP_Evaluate(ASTNode* ast)
{

    if(ast == NULL) { return false; }

    if(ast->Type == FunctionID)
    {
        // Invoke and return the boolean function for the node
        return s_functions[ast->Value]();
    }
    else if(ast->Type == UnaryNot)
    {
        // Return the inverse of the subtree below this node
        return !LEP_Evaluate(ast->Left);
    }
    else
    {
        // Return either the AND or OR of the two subtrees below this node
        bool v1 = LEP_Evaluate(ast->Left);
        bool v2 = LEP_Evaluate(ast->Right);
        switch(ast->Type)
        {
            case OperatorAnd:  return v1 && v2;
            case OperatorOr: return v1 || v2;
            default:
                break;
        }
    }

    return false;
}

/* LEP_Parse
 * Parse and produce an AST for the given text, but do not evaluate.
 */
ASTNode * LEP_Parse(Parser * parser, const char* text)
{
    if (!parser) { return NULL; }

    AST_Init();

    parser->m_Text = text;
    parser->m_Index = 0;
    parser->m_success = true;
    parser->m_errorMessage[0] = '\0';

    getNextToken(parser);

    return expression(parser);
}

/* LEP_RegisterFunction
 * When a number is present in the input string, it represents a function from
 * 0 to MAX_FUNCTIONS-1. The application can register functions for each ID.
 * If no function is registered for a called function, defaultFn is called.
 */
void LEP_RegisterFunction(uint8_t fid, BOOLFUNCTION fn)
{
    if (!fn) {return;}
    if (fid >= MAX_FUNCTIONS) { return; }

    s_functions[fid] = fn;
}