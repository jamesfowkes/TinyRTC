enum TokenType
{
   Error,
   Plus,
   Minus,
   Mul,
   Div,
   EndOfText,
   OpenParenthesis,
   ClosedParenthesis,
   Number
};

struct Token
{
   TokenType    Type;
   double       Value;
   char     Symbol;
};

struct Parser
{
    Token m_crtToken;
    const char* m_Text;
    size_t m_Index;
    bool m_success;
    char m_errorMessage[30];
};

enum ASTNodeType
{
   Undefined,
   OperatorPlus,
   OperatorMinus,
   OperatorMul,
   OperatorDiv,
   UnaryMinus,
   NumberValue
};

struct ASTNode
{
   ASTNodeType Type;
   double      Value;
   ASTNode*    Left;
   ASTNode*    Right;
};

ASTNode * Parse(Parser * parser, const char* text);
double Evaluate(ASTNode* ast);