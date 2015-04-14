enum TokenType 
{
   Error,
   And,
   Or,
   Not,
   EndOfText,
   OpenParenthesis,
   ClosedParenthesis,
   ID
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
   OperatorAnd,
   OperatorOr,
   UnaryNot,
   FunctionID
};

struct ASTNode
{
   ASTNodeType Type;
   bool      Value;
   ASTNode*    Left;
   ASTNode*    Right;
};

typedef bool (*BOOLFUNCTION)(void);

void Parse(Parser * parser, const char* text);
