#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "syntax_parser_old.h"

#define ON_PARSER_ERROR_EXIT_EARLY(parser) { if (!parser->m_success) { return; }}
#define ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, rtn) { if (!parser->m_success) { return rtn; }}

#define MAX_NODE_NUMBER (128)

static ASTNode* Expression(Parser * parser);
static ASTNode* Expression1(Parser * parser);
static ASTNode* Term(Parser * parser);
static ASTNode* Term1(Parser * parser);
static ASTNode* Factor(Parser * parser);
static void Match(Parser * parser, char expected);
static void SkipWhitespaces(Parser * parser);
static void GetNextToken(Parser * parser);
static double GetNumber(Parser * parser);
static ASTNode* CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right);
static ASTNode* CreateUnaryNode(Parser * parser, ASTNode* left);
static ASTNode* CreateNodeNumber(Parser * parser, double value);
static ASTNode* GetNextFreeNode(Parser * parser);
static double EvaluateSubtree(ASTNode* ast);

static ASTNode s_freeNodes[MAX_NODE_NUMBER];
static uint8_t s_freeNodeIndex = 0;

static ASTNode* Expression(Parser * parser)
{
    printf("%s\n", __func__);
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    ASTNode* tnode = Term(parser);
    ASTNode* e1node = Expression1(parser);

    return CreateNode(parser, OperatorPlus, tnode, e1node);
}

static ASTNode* Expression1(Parser * parser)
{
    printf("%s\n", __func__);
    ASTNode* tnode;
    ASTNode* e1node;

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    switch(parser->m_crtToken.Type)
    {
        case Plus:
        GetNextToken(parser);
        tnode = Term(parser);
        e1node = Expression1(parser);
        return CreateNode(parser, OperatorPlus, e1node, tnode);
        break;

        case Minus:
        GetNextToken(parser);
        tnode = Term(parser);
        e1node = Expression1(parser);
        return CreateNode(parser, OperatorMinus, e1node, tnode);
        break;
    }

    return CreateNodeNumber(parser, 0);
}

static ASTNode* Term(Parser * parser)
{
    printf("%s\n", __func__);
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    ASTNode* fnode = Factor(parser);
    ASTNode* t1node = Term1(parser);

    return CreateNode(parser, OperatorMul, fnode, t1node);
}

static ASTNode* Term1(Parser * parser)
{
    printf("%s\n", __func__);
    ASTNode* fnode;
    ASTNode* t1node;

    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, NULL);
    switch(parser->m_crtToken.Type)
    {
        case Mul:
        GetNextToken(parser);
        fnode = Factor(parser);
        t1node = Term1(parser);
        return CreateNode(parser, OperatorMul, t1node, fnode);
        break;

        case Div:
        GetNextToken(parser);
        fnode = Factor(parser);
        t1node = Term1(parser);
        return CreateNode(parser, OperatorDiv, t1node, fnode);
        break;
    }

    return CreateNodeNumber(parser, 1);
}

static ASTNode* Factor(Parser * parser)
{
    printf("%s\n", __func__);
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

        case Minus:
            GetNextToken(parser);
            node = Factor(parser);
            return CreateUnaryNode(parser, node);

        case Number:
            value = parser->m_crtToken.Value;
            GetNextToken(parser);
            return CreateNodeNumber(parser, value);

        default:
            parser->m_success = false;
            sprintf(parser->m_errorMessage, "Unexpected token '%c' at position %d", parser->m_crtToken.Symbol, parser->m_Index);
            return NULL;
    }
}

static ASTNode* CreateNode(Parser * parser, ASTNodeType type, ASTNode* left, ASTNode* right)
{
    printf("%s\n", __func__);
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
    printf("%s\n", __func__);    
    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = UnaryMinus;
        node->Left = left;
        node->Right = NULL;
    }

    return node;
}

static ASTNode* CreateNodeNumber(Parser * parser, double value)
{
    printf("%s\n", __func__);    
    ASTNode* node = GetNextFreeNode(parser);

    if (node)
    {
        node->Type = NumberValue;
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
    printf("%s\n", __func__);    
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
        parser->m_crtToken.Type = Number;
        parser->m_crtToken.Value = GetNumber(parser);
        return;
    }

    parser->m_crtToken.Type = Error;

    // check if the current character is an operator or parentheses
    switch(parser->m_Text[parser->m_Index])
    {
        case '+': parser->m_crtToken.Type = Plus; break;
        case '-': parser->m_crtToken.Type = Minus; break;
        case '*': parser->m_crtToken.Type = Mul; break;
        case '/': parser->m_crtToken.Type = Div; break;
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

static double GetNumber(Parser * parser)
{
    ON_PARSER_ERROR_EXIT_EARLY_WITH_RTN(parser, 0.0);

    SkipWhitespaces(parser);

    int index = parser->m_Index;
    while(isdigit(parser->m_Text[parser->m_Index])) parser->m_Index++;
    if(parser->m_Text[parser->m_Index] == '.') parser->m_Index++;
    while(isdigit(parser->m_Text[parser->m_Index])) parser->m_Index++;

    if(parser->m_Index - index == 0)
    {
        sprintf(parser->m_errorMessage, "Number expected but not found at position %d!", parser->m_Index);
    }


    char buffer[32] = {0};
    memcpy(buffer, &parser->m_Text[index], parser->m_Index - index);

    return atof(buffer);
}

static double EvaluateSubtree(ASTNode* ast)
{
    if(ast == NULL) { return 0.0; }

    if(ast->Type == NumberValue)
    {
        return ast->Value;
    }
    else if(ast->Type == UnaryMinus)
    {
        return -EvaluateSubtree(ast->Left);
    }
    else
    {
        double v1 = EvaluateSubtree(ast->Left);
        double v2 = EvaluateSubtree(ast->Right);
        switch(ast->Type)
        {
            case OperatorPlus:  return v1 + v2;
            case OperatorMinus: return v1 - v2;
            case OperatorMul:   return v1 * v2;
            case OperatorDiv:   return v1 / v2;
        }
    }

    return 0.0;
}

double Evaluate(ASTNode* ast)
{
    if(ast == NULL) { return 0.0; }

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
