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

//#define DEBUG

static ASTNode* Expression(Parser * parser);
static ASTNode* Expression1(Parser * parser);
static ASTNode* Term(Parser * parser);
static ASTNode* Term1(Parser * parser);
static ASTNode* Factor(Parser * parser);
static void Match(Parser * parser, char expected);
static void SkipWhitespaces(Parser * parser);
static void GetNextToken(Parser * parser);
static ASTNode* CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right);
static ASTNode* CreateUnaryNode(Parser * parser, ASTNode* left);
static ASTNode* CreateNodeBoolValue(Parser * parser, bool value);
static ASTNode* CreateNodeBoolFunction(Parser * parser, uint8_t fid);
static ASTNode* GetNextFreeNode(Parser * parser);
static bool EvaluateSubtree(ASTNode* ast);

static ASTNode s_freeNodes[MAX_NODE_NUMBER];
static uint8_t s_freeNodeIndex = 0;

static bool defaultFn(void) { return false; }

static BOOLFUNCTION s_functions[MAX_FUNCTIONS];

static ASTNode* Expression(Parser * parser)
{
    #ifdef DEBUG
    printf("%s\n", __func__);
    #endif

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    ASTNode* tnode = Term(parser);
    ASTNode* e1node = Expression1(parser);

    return CreateNode(parser, OperatorAnd, tnode, e1node);
}

static ASTNode* Expression1(Parser * parser)
{
    #ifdef DEBUG
    printf("%s\n", __func__);
    #endif
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

    return CreateNodeBoolValue(parser, true);
}

static ASTNode* Term(Parser * parser)
{
    #ifdef DEBUG
    printf("%s\n", __func__);
    #endif

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    ASTNode* fnode = Factor(parser);
    ASTNode* t1node = Term1(parser);

    return CreateNode(parser, OperatorOr, fnode, t1node);
}

static ASTNode* Term1(Parser * parser)
{
    #ifdef DEBUG
    printf("%s\n", __func__);
    #endif

    ASTNode* fnode;
    ASTNode* t1node;

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    switch(parser->m_crtToken.Type)
    {
        case Or:
        GetNextToken(parser);
        fnode = Factor(parser);
        t1node = Term1(parser);
        return CreateNode(parser, OperatorOr, t1node, fnode);
        break;
    }

    return CreateNodeBoolValue(parser, false);
}

static ASTNode* Factor(Parser * parser)
{
    #ifdef DEBUG
    printf("%s\n", __func__);
    #endif

    ASTNode* node;
    uint8_t value;

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

        case Number:
            value = parser->m_crtToken.Value;
            GetNextToken(parser);
            return CreateNodeBoolFunction(parser, value);

        case BoolChar:
            value = parser->m_crtToken.Value;
            GetNextToken(parser);
            return CreateNodeBoolValue(parser, (bool)value);
        default:
            parser->m_success = false;
            sprintf(parser->m_errorMessage, "%s: At position %d unexpected token '%c'",
                __func__, parser->m_Index, parser->m_crtToken.Symbol);
            return NULL;
    }
}

static ASTNode* CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right)
{
    #ifdef DEBUG
    printf("%s\n", __func__);
    #endif

    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = type;
        node->Left = left;
        node->Right = right;
        #ifdef DEBUG
        printf("%s (%s)\n", __func__, type == OperatorOr ? "or" : "and");
        #endif
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
        #ifdef DEBUG
        printf("%s (not)\n", __func__);
        #endif
    }

    return node;
}

static ASTNode* CreateNodeBoolFunction(Parser * parser, uint8_t value)
{
    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = FunctionID;
        node->Value = value;
        #ifdef DEBUG
        printf("%s (%d)\n", __func__, value);
        #endif
    }

    return node;
}

static ASTNode* CreateNodeBoolValue(Parser * parser, bool value)
{
    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = FunctionID;
        node->Value = value;
        #ifdef DEBUG
        printf("%s (%s)\n", __func__, value ? "true" : "false");
        #endif
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
        sprintf(parser->m_errorMessage, "At position %d unexpected token '%c' (Line %d)",
            parser->m_Index, expected, __LINE__);
    }
}

static void SkipWhitespaces(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);
    while(isspace(parser->m_Text[parser->m_Index])) parser->m_Index++;
}

static uint8_t GetInteger(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, 0);
    
    SkipWhitespaces(parser);

    int index = parser->m_Index;
    while(isdigit(parser->m_Text[parser->m_Index])) parser->m_Index++;
    
    if(parser->m_Index - index == 0)
    {
        parser->m_success = false;
        sprintf(parser->m_errorMessage, "Number expected but not found at position %d!", parser->m_Index);
        return 0;
    }

    char buffer[32] = {0};
    memcpy(buffer, &parser->m_Text[index], parser->m_Index - index);

    unsigned long result = atol(buffer);

    if (result > 255)
    {
        parser->m_success = false;
        sprintf(parser->m_errorMessage, "Function ID %lu exceeds 255!", result);
        return 0;
    }

    if (result >= MAX_FUNCTIONS)
    {
        parser->m_success = false;
        sprintf(parser->m_errorMessage, "Function ID %lu exceeds maximum allocation of MAX_FUNCTIONS!", result);
        return 0;   
    }

    return (uint8_t)result;
}

static void GetNextToken(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY(parser);

    // ignore white spaces
    SkipWhitespaces(parser);

    parser->m_crtToken.Value = 0;
    parser->m_crtToken.Symbol = 0;
    parser->m_crtToken.Type = Error;

    #ifdef DEBUG
    printf("Next token: '%c'\n", parser->m_Text[parser->m_Index]);
    #endif

    // test for the end of text
    if(parser->m_Text[parser->m_Index] == 0)
    {
        parser->m_crtToken.Type = EndOfText;
        return;
    }

    // if the current character is a digit it is a function ID
    if(isdigit(parser->m_Text[parser->m_Index]))
    {
        parser->m_crtToken.Type = Number;
        parser->m_crtToken.Value = GetInteger(parser);
        return;
    }

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
            sprintf(parser->m_errorMessage, "%s: At position %d unexpected token '%c'",
                __func__, parser->m_Index, parser->m_Text[parser->m_Index]);
        }
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

void Parser_Init(void)
{
    uint8_t i;
    for (i = 0; i < MAX_FUNCTIONS; ++i)
    {
        s_functions[i] = defaultFn;
    }
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