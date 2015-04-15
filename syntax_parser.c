#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "syntax_parser.h"

#define ON_PARSER_ERROR_EXIT_EARLY(parser) { if (!parser->m_success) { return; }}
#define ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, rtn) { if (!parser->m_success) { return rtn; }}

#define MAX_NODE_NUMBER (128)
#define MAX_FUNCTIONS (16)

static ASTNode* Expression(Parser * parser);
static ASTNode* Expression1(Parser * parser);
static ASTNode* Term(Parser * parser);
static ASTNode* Term1(Parser * parser);
static ASTNode* Factor(Parser * parser);
static void Match(Parser * parser, char expected);
static void SkipWhitespaces(Parser * parser);
static void GetNextToken(Parser * parser);
static bool GetBoolean(Parser * parser);
static ASTNode* CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right);
static ASTNode* CreateUnaryNode(Parser * parser, ASTNode* left);
static ASTNode* CreateNodeBool(Parser * parser, bool value);
static ASTNode* GetNextFreeNode(Parser * parser);
static bool EvaluateSubtree(ASTNode* ast);

static ASTNode s_freeNodes[MAX_NODE_NUMBER];
static uint8_t s_freeNodeIndex = 0;

static BOOLFUNCTION s_functions[MAX_FUNCTIONS];

static ASTNode* Expression(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    ASTNode* tnode = Term(parser);
    ASTNode* e1node = Expression1(parser);

    return CreateNode(parser, OperatorAnd, tnode, e1node);
}

static ASTNode* Expression1(Parser * parser)
{
    ASTNode* tnode;
    ASTNode* e1node;

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    switch(parser->m_crtToken.Type)
    {
        case And:
        GetNextToken(parser);
        tnode = Term(parser);
        e1node = Expression1(parser);
        return CreateNode(parser, OperatorAnd, e1node, tnode);
        break;
    }

    return CreateNodeBool(parser, 0);
}

static ASTNode* Term(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    ASTNode* fnode = Factor(parser);
    ASTNode* t1node = Term1(parser);

    return CreateNode(parser, OperatorOr, fnode, t1node);
}

static ASTNode* Term1(Parser * parser)
{
    ASTNode* fnode;
    ASTNode* t1node;

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    switch(parser->m_crtToken.Type)
    {
        case Or:
        GetNextToken(parser);
        fnode = Factor(parser);
        Term1(parser);
        return CreateNode(parser, OperatorOr, t1node, fnode);
        break;
    }

    return CreateNodeBool(parser, 1);
}

static ASTNode* Factor(Parser * parser)
{
    ASTNode* node;
    double value;

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    switch(parser->m_crtToken.Type)
    {
        case OpenParenthesis:
            GetNextToken(parser);
            node = Expression(parser);
            Match(parser, ')');
            return node;

        case Not:
            GetNextToken(parser);
            node = Factor(parser);
            return CreateUnaryNode(parser, node);

        case ID:
            value = parser->m_crtToken.Value;
            GetNextToken(parser);
            return CreateNodeBool(parser, value);

        default:
            parser->m_success = false;
            sprintf(parser->m_errorMessage, "Unexpected token '%c' at position %d", parser->m_crtToken.Symbol, parser->m_Index);
            return NULL;
    }
}

static ASTNode* CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right)
{
    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = type;
        node->Left = left;
        node->Right = right;
    }

    return node;
}

static ASTNode* CreateUnaryNode(Parser * parser, ASTNode* left)
{
    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = UnaryNot;
        node->Left = left;
        node->Right = NULL;
    }

    return node;
}

static ASTNode* CreateNodeBool(Parser * parser, bool value)
{
    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = FunctionID;
        node->Value = value;
    }

    return node;
}

static ASTNode* GetNextFreeNode(Parser * parser)
{

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);

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

static void Match(Parser * parser, char expected)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);
    if(parser->m_Text[parser->m_Index-1] == expected)
    {
        GetNextToken(parser);
    }
    else
    {
        parser->m_success = false;
        sprintf(parser->m_errorMessage, "Unexpected token '%c' at position %d", expected, parser->m_Index);
    }
}

static void SkipWhitespaces(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);
    while(isspace(parser->m_Text[parser->m_Index])) parser->m_Index++;
}

static void GetNextToken(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);

    // ignore white spaces
    SkipWhitespaces(parser);

    parser->m_crtToken.Value = 0;
    parser->m_crtToken.Symbol = 0;

    // test for the end of text
    if(parser->m_Text[parser->m_Index] == 0)
    {
        parser->m_crtToken.Type = EndOfText;
        return;
    }

    // if the current character is a digit read a number
    if(isdigit(parser->m_Text[parser->m_Index]))
    {
        parser->m_crtToken.Type = ID;
        parser->m_crtToken.Value = GetBoolean(parser);
        return;
    }

    parser->m_crtToken.Type = Error;

    // check if the current character is an operator or parentheses
    switch(parser->m_Text[parser->m_Index])
    {
        case '&': parser->m_crtToken.Type = And; break;
        case '|': parser->m_crtToken.Type = Or; break;
        case '(': parser->m_crtToken.Type = OpenParenthesis; break;
        case ')': parser->m_crtToken.Type = ClosedParenthesis; break;
    }

    if(parser->m_crtToken.Type != Error)
    {
        parser->m_crtToken.Symbol = parser->m_Text[parser->m_Index];
        parser->m_Index++;
    }
    else
    {
        parser->m_success = false;
        sprintf(parser->m_errorMessage, "Unexpected token '%c' at position %d", parser->m_Text[parser->m_Index], parser->m_Index);
    }
}

static bool GetBoolean(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, false);

    SkipWhitespaces(parser);

    switch(parser->m_Text[parser->m_Index])
    {
        case 'T':
            return true;
        case 'F':
            return false;
        default:
            parser->m_success = false;
            sprintf(parser->m_errorMessage, "T or F expected but not found at position %d!", parser->m_Index);
            return false;
    }
}

static bool EvaluateSubtree(ASTNode* ast)
{
    if(ast == NULL) { return false; }

    if(ast->Type == FunctionID)
    {
        return s_functions[ast->Value]();
    }
    else if(ast->Type == UnaryNot)
    {
        return !EvaluateSubtree(ast->Left);
    }
    else
    {
        bool v1 = EvaluateSubtree(ast->Left);
        bool v2 = EvaluateSubtree(ast->Right);
        switch(ast->Type)
        {
            case OperatorAnd:  return v1 && v2;
            case OperatorOr: return v1 || v2;
        }
    }

    return false;
}

bool Evaluate(ASTNode* ast)
{
    if(ast == NULL) { return false; }

    return EvaluateSubtree(ast);
}

ASTNode * Parse(Parser * parser, const char* text)
{
    if (!parser) { return NULL; }

    s_freeNodeIndex = 0;

    parser->m_Text = text;
    parser->m_Index = 0;
    parser->m_success = true;
    parser->m_errorMessage[0] = '\0';

    GetNextToken(parser);

    return Expression(parser);
}

void RegisterFunction(uint8_t fid, BOOLFUNCTION fn)
{
    if (!fn) {return;}
    if (fid >= MAX_FUNCTIONS) { return; }

    s_functions[fid] = fn;
}