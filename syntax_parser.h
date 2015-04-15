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

enum ASTNodeType
{
   Undefined,
   OperatorAnd,
   OperatorOr,
   UnaryNot,
   FunctionID,
   BoolValue
};

struct ASTNode
{
   ASTNodeType Type;
   bool      Value;
   ASTNode*    Left;
   ASTNode*    Right;
};

typedef bool (*BOOLFUNCTION)(void);

void Parser_Init(void);
bool Evaluate(ASTNode* ast);
ASTNode * Parse(Parser * parser, const char* text);
void RegisterFunction(uint8_t fid, BOOLFUNCTION fn);
