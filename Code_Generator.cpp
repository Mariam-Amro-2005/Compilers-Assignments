#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

/*
{
Test Cases for Binary AND (&)
-------------------------------------
{ Test 1: Simple AND }
read x;
read y;
z := x & y;
write z
;-------------------------------------
{ Test 2: AND inside addition }
read a;
a := (a & 3) + 5;
write a
;-------------------------------------
{ Test 3: Double AND }
read x;
read y;
read z;
r := x & y & z;
write r
;-------------------------------------
{ Test 4: AND mixed with * }
read a;
read b;
c := a & b * 2;
write c
;-------------------------------------
{ Test 5: AND inside repeat loop }
read x;
repeat
        x := x & 2
until 100 < x;
write x
;-------------------------------------
{ Test 6: AND inside IF }
read x;
if x < 10 then
        y := x & 3;
write y
else
y := x & 5;
write y
end
;-------------------------------------
{ Test 7: Nested parentheses }
read x;
x := ( (x & 2) & (x & 3) );
write x
;-------------------------------------

{ Test 8: AND with power }
read x;
y := (x ^ 2) & (x ^ 3);
write y
;-------------------------------------
{ Test 9: AND combined with minus }
read a;
read b;
c := a - (a & b);
write c
;-------------------------------------

{ Test 10: AND inside equality check }
read x;
read y;
if (x & y) = 5 then
        write x
else
write y
end
;-------------------------------------
{ Test 11: Multiple AND with other ops }
read a;
b := a & 5 + a & 3;
write b
;-------------------------------------
{ Test 12: Stress AND }
read x;
read y;
read z;
k := (x & y) * (y & z) - (z & x);
write k
}
;-------------------------------------
{ Test 13: Power has higher precedence than & }
read x;
read y;
z := x ^ 3 & y;
write z
;-------------------------------------
{ Test 14: AND has higher precedence than multiplication }
read a;
read b;
c := a & b * 3;
write c
;-------------------------------------
{ Test 15: Multiplication higher than addition, with & in between }
read a;
read b;
read c;
r := a + b & c * 2;
write r
;-------------------------------------
{ Test 16: ^ evaluated before & even when AND looks larger }
read x;
y := x ^ 2 & x ^ 3;
write y
;-------------------------------------
{ Test 17: AND binds before both additions }
read a;
read b;
read c;
k := a + b & c + 4;
write k
;-------------------------------------
{ Test 18: division lower than & }
read a;
read b;
read c;
k := a / b & c / 2;
write k

;-------------------------------------
{ Test 19: Left associativity stress }
read a;
read b;
read c;
x := a & b & c & 2;
write x

;-------------------------------------
{ Test 20: AND with zero }
read x;
y := x & 0;
write y

;-------------------------------------
{ Test 21: Zero AND something }
read x;
y := 0 & x;
write y

;-------------------------------------
{ Test 22: AND inside subtraction chain }
read a;
read b;
read c;
x := a - b & c - 1;
write x

;-------------------------------------
{ Test 23: AND with division both sides }
read a;
read b;
read c;
x := (a / b) & (c / 2);
write x

;-------------------------------------
{ Test 24: Deep nested AND }
read x;
y := ( (x & 2) & ( (x & 3) & (x & 4) ) );
write y

;-------------------------------------
{ Test 25: Boolean from AND in IF }
read x;
if (x & 2) < 10 then
        write x
else
        write 0
end

;-------------------------------------
{ Test 26: AND in repeat condition }
read x;
repeat
        x := x + 1
until (x & 3) > 20;
write x

;-------------------------------------
{ Test 27: AND with same operands }
read x;
y := x & x;
write y

;-------------------------------------
{ Test 28: AND with mixed sign result }
read a;
read b;
x := a & b - a;
write x

;-------------------------------------
{ Test 29: Long operator chain }
read a;
read b;
read c;
read d;
x := a + b & c * d ^ 2 - 5;
write x

;-------------------------------------
{ Test 30: Full precedence stress test }
read a;
read b;
read c;
read d;
x := a ^ 2 + b & c * d - a / 2;
write x

*/

// sequence of statements separated by ;
// no procedures - no declarations
// all variables are integers
// variables are declared simply by assigning values to them :=
// if-statement: if (boolean) then [else] end
// repeat-statement: repeat until (boolean)
// boolean only in if and repeat conditions < = and two mathematical expressions
// math expressions integers only, + - * / ^
// I/O read write
// Comments {}

////////////////////////////////////////////////////////////////////////////////////
// Strings /////////////////////////////////////////////////////////////////////////

bool Equals(const char *a, const char *b)
{
    return strcmp(a, b) == 0;
}

bool StartsWith(const char *a, const char *b)
{
    int nb = strlen(b);
    return strncmp(a, b, nb) == 0;
}

void Copy(char *a, const char *b, int n = 0)
{
    if (n > 0)
    {
        strncpy(a, b, n);
        a[n] = 0;
    }
    else
        strcpy(a, b);
}

void AllocateAndCopy(char **a, const char *b)
{
    if (b == 0)
    {
        *a = 0;
        return;
    }
    int n = strlen(b);
    *a = new char[n + 1];
    strcpy(*a, b);
}

////////////////////////////////////////////////////////////////////////////////////
// Input and Output ////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE *file;
    int cur_line_num;

    char line_buf[MAX_LINE_LENGTH];
    int cur_ind, cur_line_size;

    InFile(const char *str)
    {
        file = 0;
        if (str)
            file = fopen(str, "r");
        cur_line_size = 0;
        cur_ind = 0;
        cur_line_num = 0;
    }
    ~InFile()
    {
        if (file)
            fclose(file);
    }

    void SkipSpaces()
    {
        while (cur_ind < cur_line_size)
        {
            char ch = line_buf[cur_ind];
            if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n')
                break;
            cur_ind++;
        }
    }

    bool SkipUpto(const char *str)
    {
        while (true)
        {
            SkipSpaces();
            while (cur_ind >= cur_line_size)
            {
                if (!GetNewLine())
                    return false;
                SkipSpaces();
            }

            if (StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind += strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }

    bool GetNewLine()
    {
        cur_ind = 0;
        line_buf[0] = 0;
        if (!fgets(line_buf, MAX_LINE_LENGTH, file))
            return false;
        cur_line_size = strlen(line_buf);
        if (cur_line_size == 0)
            return false; // End of file
        cur_line_num++;
        return true;
    }

    char *GetNextTokenStr()
    {
        SkipSpaces();
        while (cur_ind >= cur_line_size)
        {
            if (!GetNewLine())
                return 0;
            SkipSpaces();
        }
        return &line_buf[cur_ind];
    }

    void Advance(int num)
    {
        cur_ind += num;
    }
};

struct OutFile
{
    FILE *file;
    OutFile(const char *str)
    {
        file = 0;
        if (str)
            file = fopen(str, "w");
    }
    ~OutFile()
    {
        if (file)
            fclose(file);
    }

    void Out(const char *s)
    {
        fprintf(file, "%s\n", s);
        fflush(file);
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Compiler Parameters /////////////////////////////////////////////////////////////

struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char *in_str, const char *out_str, const char *debug_str)
        : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Scanner /////////////////////////////////////////////////////////////////////////

#define MAX_TOKEN_LEN 40

enum TokenType
{
    IF,
    THEN,
    ELSE,
    END,
    REPEAT,
    UNTIL,
    READ,
    WRITE,
    ASSIGN,
    EQUAL,
    LESS_THAN,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    BINARY_AND,
    POWER,
    SEMI_COLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    ID,
    NUM,
    INT,
    INT_NUM,
    REAL,
    REAL_NUM,
    BOOL,
    TRUE,
    FALSE,
    ENDFILE,
    ERROR
};

// Used for debugging only /////////////////////////////////////////////////////////
const char *TokenTypeStr[] =
    {
        "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
        "Assign", "Equal", "LessThan",
        "Plus", "Minus", "Times", "Divide", "BinaryAnd", "Power",
        "SemiColon",
        "LeftParen", "RightParen",
        "LeftBrace", "RightBrace",
        "ID", "Num", "Int", "IntNum", "Real", "RealNum", "Bool", "True", "False",
        "EndFile", "Error"};

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN + 1];

    Token()
    {
        str[0] = 0;
        type = ERROR;
    }
    Token(TokenType _type, const char *_str)
    {
        type = _type;
        Copy(str, _str);
    }
};

const Token reserved_words[] =
    {
        Token(INT, "int"),
        Token(REAL, "real"),
        Token(BOOL, "bool"),
        Token(TRUE, "true"),
        Token(FALSE, "false"),
        Token(IF, "if"),
        Token(THEN, "then"),
        Token(ELSE, "else"),
        Token(END, "end"),
        Token(REPEAT, "repeat"),
        Token(UNTIL, "until"),
        Token(READ, "read"),
        Token(WRITE, "write")};
const int num_reserved_words = sizeof(reserved_words) / sizeof(reserved_words[0]);

// if there is tokens like < <=, sort them such that sub-tokens come last: <= <
// the closing comment should come immediately after opening comment
const Token symbolic_tokens[] =
    {
        Token(ASSIGN, ":="),
        Token(EQUAL, "="),
        Token(LESS_THAN, "<"),
        Token(PLUS, "+"),
        Token(MINUS, "-"),
        Token(TIMES, "*"),
        Token(DIVIDE, "/"),
        Token(BINARY_AND, "&"),
        Token(POWER, "^"),
        Token(SEMI_COLON, ";"),
        Token(LEFT_PAREN, "("),
        Token(RIGHT_PAREN, ")"),
        Token(LEFT_BRACE, "{"),
        Token(RIGHT_BRACE, "}")};
const int num_symbolic_tokens = sizeof(symbolic_tokens) / sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch) { return (ch >= '0' && ch <= '9'); }
inline bool IsLetter(char ch) { return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }
inline bool IsLetterOrUnderscore(char ch) { return (IsLetter(ch) || ch == '_'); }

void GetNextToken(CompilerInfo *pci, Token *ptoken)
{
    ptoken->type = ERROR;
    ptoken->str[0] = 0;

    int i;
    char *s = pci->in_file.GetNextTokenStr();
    if (!s)
    {
        ptoken->type = ENDFILE;
        ptoken->str[0] = 0;
        return;
    }

    for (i = 0; i < num_symbolic_tokens; i++)
    {
        if (StartsWith(s, symbolic_tokens[i].str))
            break;
    }

    if (i < num_symbolic_tokens)
    {
        if (symbolic_tokens[i].type == LEFT_BRACE)
        {
            pci->in_file.Advance(strlen(symbolic_tokens[i].str));
            if (!pci->in_file.SkipUpto(symbolic_tokens[i + 1].str))
                return;
            return GetNextToken(pci, ptoken);
        }
        ptoken->type = symbolic_tokens[i].type;
        Copy(ptoken->str, symbolic_tokens[i].str);
    }
    else if (IsDigit(s[0]))
    {
        bool decimal = false;
        int j = 1;

        while (IsDigit(s[j]))
            j++;

        if (s[j] == '.' && IsDigit(s[j + 1]))
        {
            decimal = true;
            j++;

            while (IsDigit(s[j]))
                j++;
        }

        // ptoken->type = NUM;
        if (decimal)
        {
            ptoken->type = REAL_NUM;
        }
        else
        {
            ptoken->type = INT_NUM;
        }
        Copy(ptoken->str, s, j);
    }
    else if (IsLetterOrUnderscore(s[0]))
    {
        int j = 1;
        while (IsLetterOrUnderscore(s[j]))
            j++;

        ptoken->type = ID;
        Copy(ptoken->str, s, j);

        for (i = 0; i < num_reserved_words; i++)
        {
            if (Equals(ptoken->str, reserved_words[i].str))
            {
                ptoken->type = reserved_words[i].type;
                break;
            }
        }
    }

    int len = strlen(ptoken->str);
    if (len > 0)
        pci->in_file.Advance(len);
}

////////////////////////////////////////////////////////////////////////////////////
// Parser //////////////////////////////////////////////////////////////////////////
//{ ; decl }

// program -> decls stmtseq
// decls -> decl { ; decl }
// decl -> datatype id
// stmtseq -> stmt { ; stmt }
// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
// expr -> mathexpr [ (<|=) mathexpr ]
// mathexpr -> term { (+|-) term }    left associative
// term -> andexpr { (*|/) andexpr }    left associative
// andexpr -> factor { (&) factor }    left associative
// factor -> newexpr { ^ newexpr }    right associative
// newexpr -> ( mathexpr ) | int_num | real_num | identifier | boolexpr

enum NodeKind
{
    DECL_NODE,
    IF_NODE,
    REPEAT_NODE,
    ASSIGN_NODE,
    READ_NODE,
    WRITE_NODE,
    OPER_NODE,
    NUM_NODE,
    INT_NODE,
    REAL_NODE,
    BOOL_NODE,
    ID_NODE
};

// Used for debugging only /////////////////////////////////////////////////////////
const char *NodeKindStr[] =
    {
        "Declare", "If", "Repeat", "Assign", "Read", "Write",
        "Oper", "Num", "Int", "Real", "Bool", "ID"};

enum ExprDataType
{
    VOID,
    INTEGER,
    REAL_TYPE,
    BOOLEAN
};

// Used for debugging only /////////////////////////////////////////////////////////
const char *ExprDataTypeStr[] =
    {
        "Void", "Integer", "Real", "Boolean"};

#define MAX_CHILDREN 3

struct TreeNode
{
    TreeNode *child[MAX_CHILDREN];
    TreeNode *sibling; // used for sibling statements only

    NodeKind node_kind;

    union
    {
        TokenType oper;
        int num;
        double real_num;
        bool bool_val;
        char *id;
    };

    ExprDataType expr_data_type;
    ExprDataType declared_type;

    int line_num;

    TreeNode()
    {
        int i;
        for (i = 0; i < MAX_CHILDREN; i++)
            child[i] = nullptr;
        sibling = nullptr;
        expr_data_type = VOID;
    }
};

struct ParseInfo
{
    Token next_token;
};

void Match(CompilerInfo *pci, ParseInfo *ppi, TokenType expected_token_type)
{
    pci->debug_file.Out("Start Match");

    if (ppi->next_token.type != expected_token_type)
        throw 0;
    GetNextToken(pci, &ppi->next_token);

    fprintf(pci->debug_file.file, "[%d] %s (%s)\n", pci->in_file.cur_line_num, ppi->next_token.str, TokenTypeStr[ppi->next_token.type]);
    fflush(pci->debug_file.file);
}

TreeNode *MathExpr(CompilerInfo *, ParseInfo *);

// newexpr -> ( mathexpr ) | int_num | real_num | identifier | boolexpr
TreeNode *NewExpr(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start NewExpr");

    // Compare the next token with the First() of possible statements
    // if (ppi->next_token.type == NUM)
    if (ppi->next_token.type == INT_NUM || ppi->next_token.type == REAL_NUM)
    {
        TreeNode *tree = new TreeNode;

        if (ppi->next_token.type == INT_NUM)
        {
            tree->node_kind = INT_NODE;
            tree->expr_data_type = INTEGER;
            tree->num = atoi(ppi->next_token.str);
        }
        else
        {
            tree->node_kind = REAL_NODE;
            tree->expr_data_type = REAL_TYPE;
            tree->real_num = atof(ppi->next_token.str);
        }

        tree->line_num = pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if (ppi->next_token.type == TRUE || ppi->next_token.type == FALSE)
    {
        TreeNode *tree = new TreeNode;
        tree->node_kind = BOOL_NODE;
        tree->expr_data_type = BOOLEAN;
        tree->bool_val = (ppi->next_token.type == TRUE);
        tree->line_num = pci->in_file.cur_line_num;

        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if (ppi->next_token.type == ID)
    {
        TreeNode *tree = new TreeNode;
        tree->node_kind = ID_NODE;
        AllocateAndCopy(&tree->id, ppi->next_token.str);
        tree->line_num = pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if (ppi->next_token.type == LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode *tree = MathExpr(pci, ppi);
        Match(pci, ppi, RIGHT_PAREN);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    throw 0;
}

// factor -> newexpr { ^ newexpr }    right associative
TreeNode *Factor(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Factor");

    TreeNode *tree = NewExpr(pci, ppi);

    if (ppi->next_token.type == POWER)
    {
        TreeNode *new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = ppi->next_token.type;
        new_tree->line_num = pci->in_file.cur_line_num;

        new_tree->child[0] = tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1] = Factor(pci, ppi);

        pci->debug_file.Out("End Factor");
        return new_tree;
    }
    pci->debug_file.Out("End Factor");
    return tree;
}

// andexpr -> factor { (&) factor }    left associative
TreeNode *BinaryAndExpr(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start BinaryAndExpr");

    TreeNode *tree = Factor(pci, ppi);

    while (ppi->next_token.type == BINARY_AND)
    {
        TreeNode *new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = ppi->next_token.type;
        new_tree->line_num = pci->in_file.cur_line_num;

        new_tree->child[0] = tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1] = Factor(pci, ppi);

        tree = new_tree;
    }
    pci->debug_file.Out("End BinaryAndExpr");
    return tree;
}

// term -> andexpr { (*|/) andexpr }    left associative
TreeNode *Term(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Term");

    TreeNode *tree = BinaryAndExpr(pci, ppi);

    while (ppi->next_token.type == TIMES || ppi->next_token.type == DIVIDE)
    {
        TreeNode *new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = ppi->next_token.type;
        new_tree->line_num = pci->in_file.cur_line_num;

        new_tree->child[0] = tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1] = BinaryAndExpr(pci, ppi);

        tree = new_tree;
    }
    pci->debug_file.Out("End Term");
    return tree;
}

// mathexpr -> term { (+|-) term }    left associative
TreeNode *MathExpr(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start MathExpr");

    TreeNode *tree = Term(pci, ppi);

    while (ppi->next_token.type == PLUS || ppi->next_token.type == MINUS)
    {
        TreeNode *new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = ppi->next_token.type;
        new_tree->line_num = pci->in_file.cur_line_num;

        new_tree->child[0] = tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1] = Term(pci, ppi);

        tree = new_tree;
    }
    pci->debug_file.Out("End MathExpr");
    return tree;
}

// expr -> mathexpr [ (<|=) mathexpr ]
TreeNode *Expr(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Expr");

    TreeNode *tree = MathExpr(pci, ppi);

    if (ppi->next_token.type == EQUAL || ppi->next_token.type == LESS_THAN)
    {
        TreeNode *new_tree = new TreeNode;
        new_tree->node_kind = OPER_NODE;
        new_tree->oper = ppi->next_token.type;
        new_tree->line_num = pci->in_file.cur_line_num;

        new_tree->child[0] = tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1] = MathExpr(pci, ppi);

        pci->debug_file.Out("End Expr");
        return new_tree;
    }
    pci->debug_file.Out("End Expr");
    return tree;
}

// writestmt -> write expr
TreeNode *WriteStmt(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start WriteStmt");

    TreeNode *tree = new TreeNode;
    tree->node_kind = WRITE_NODE;
    tree->line_num = pci->in_file.cur_line_num;

    Match(pci, ppi, WRITE);
    tree->child[0] = Expr(pci, ppi);

    pci->debug_file.Out("End WriteStmt");
    return tree;
}

// readstmt -> read identifier
TreeNode *ReadStmt(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start ReadStmt");

    TreeNode *tree = new TreeNode;
    tree->node_kind = READ_NODE;
    tree->line_num = pci->in_file.cur_line_num;

    Match(pci, ppi, READ);

    TreeNode *idNode = new TreeNode;
    idNode->node_kind = ID_NODE;
    AllocateAndCopy(&idNode->id, ppi->next_token.str);
    idNode->line_num = pci->in_file.cur_line_num;

    Match(pci, ppi, ID);

    tree->child[0] = idNode; // FIXED
    pci->debug_file.Out("End ReadStmt");
    return tree;
}

// assignstmt -> identifier := expr
TreeNode *AssignStmt(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start AssignStmt");

    TreeNode *tree = new TreeNode;
    tree->node_kind = ASSIGN_NODE;
    tree->line_num = pci->in_file.cur_line_num;

    // Create ID node for LHS
    TreeNode *idNode = new TreeNode;
    idNode->node_kind = ID_NODE;
    AllocateAndCopy(&idNode->id, ppi->next_token.str);
    idNode->line_num = pci->in_file.cur_line_num;

    Match(pci, ppi, ID);
    Match(pci, ppi, ASSIGN);

    tree->child[0] = idNode;         // LHS
    tree->child[1] = Expr(pci, ppi); // RHS

    pci->debug_file.Out("End AssignStmt");
    return tree;
}

TreeNode *StmtSeq(CompilerInfo *, ParseInfo *);

// repeatstmt -> repeat stmtseq until expr
TreeNode *RepeatStmt(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start RepeatStmt");

    TreeNode *tree = new TreeNode;
    tree->node_kind = REPEAT_NODE;
    tree->line_num = pci->in_file.cur_line_num;

    Match(pci, ppi, REPEAT);
    tree->child[0] = StmtSeq(pci, ppi);
    Match(pci, ppi, UNTIL);
    tree->child[1] = Expr(pci, ppi);

    pci->debug_file.Out("End RepeatStmt");
    return tree;
}

// ifstmt -> if exp then stmtseq [ else stmtseq ] end
TreeNode *IfStmt(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start IfStmt");

    TreeNode *tree = new TreeNode;
    tree->node_kind = IF_NODE;
    tree->line_num = pci->in_file.cur_line_num;

    Match(pci, ppi, IF);
    tree->child[0] = Expr(pci, ppi);
    Match(pci, ppi, THEN);
    tree->child[1] = StmtSeq(pci, ppi);
    if (ppi->next_token.type == ELSE)
    {
        Match(pci, ppi, ELSE);
        tree->child[2] = StmtSeq(pci, ppi);
    }
    Match(pci, ppi, END);

    pci->debug_file.Out("End IfStmt");
    return tree;
}

// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
TreeNode *Stmt(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Stmt");

    // Compare the next token with the First() of possible statements
    TreeNode *tree = nullptr;
    if (ppi->next_token.type == IF)
        tree = IfStmt(pci, ppi);
    else if (ppi->next_token.type == REPEAT)
        tree = RepeatStmt(pci, ppi);
    else if (ppi->next_token.type == ID)
        tree = AssignStmt(pci, ppi);
    else if (ppi->next_token.type == READ)
        tree = ReadStmt(pci, ppi);
    else if (ppi->next_token.type == WRITE)
        tree = WriteStmt(pci, ppi);
    else if (ppi->next_token.type == ENDFILE)
        return tree;
    else
        throw 0;

    pci->debug_file.Out("End Stmt");
    return tree;
}

// stmtseq -> stmt { ; stmt }
TreeNode *StmtSeq(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start StmtSeq");

    TreeNode *first_tree = Stmt(pci, ppi);
    TreeNode *last_tree = first_tree;

    // If we did not reach one of the Follow() of StmtSeq(), we are not done yet
    while (ppi->next_token.type != ENDFILE && ppi->next_token.type != END &&
           ppi->next_token.type != ELSE && ppi->next_token.type != UNTIL)
    {
        Match(pci, ppi, SEMI_COLON);
        TreeNode *next_tree = Stmt(pci, ppi);
        last_tree->sibling = next_tree;
        last_tree = next_tree;
    }

    pci->debug_file.Out("End StmtSeq");
    return first_tree;
}

// decl -> datatype id
TreeNode *Decl(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Decl");

    TreeNode *tree = new TreeNode();
    tree->node_kind = DECL_NODE;
    tree->line_num = pci->in_file.cur_line_num;

    // datatype
    if (ppi->next_token.type == INT ||
        ppi->next_token.type == REAL ||
        ppi->next_token.type == BOOL)
    {
        tree->expr_data_type = tree->declared_type =
            (ppi->next_token.type == INT ? INTEGER : (ppi->next_token.type == REAL ? REAL_TYPE : BOOLEAN));

        Match(pci, ppi, ppi->next_token.type);
    }
    else
        throw 0;

    // identifier
    if (ppi->next_token.type != ID)
        throw 0;

    TreeNode *idNode = new TreeNode;
    idNode->node_kind = ID_NODE;
    AllocateAndCopy(&idNode->id, ppi->next_token.str);
    idNode->line_num = pci->in_file.cur_line_num;
    tree->child[0] = idNode;

    Match(pci, ppi, ID);

    pci->debug_file.Out("End Decl");
    return tree;
}

// decls -> decl { ; decl }
TreeNode *Decls(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Decls");

    // If next token is not a type -> no declarations
    if (ppi->next_token.type != INT &&
        ppi->next_token.type != REAL &&
        ppi->next_token.type != BOOL)
    {
        return 0;
    }

    TreeNode *first_decl = Decl(pci, ppi);
    TreeNode *last_decl = first_decl;

    while (ppi->next_token.type == SEMI_COLON)
    {
        Match(pci, ppi, SEMI_COLON);

        // Still must see datatype next, or declarations are done
        if (ppi->next_token.type != INT &&
            ppi->next_token.type != REAL &&
            ppi->next_token.type != BOOL)
            break;

        TreeNode *next = Decl(pci, ppi);
        last_decl->sibling = next;
        last_decl = next;
    }

    pci->debug_file.Out("End Decls");
    return first_decl;
}

TreeNode *Parse(CompilerInfo *pci)
{
    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);

    // Parse declarations first
    TreeNode *decl_tree = Decls(pci, &parse_info);

    // Parse statement sequence
    TreeNode *stmt_tree = StmtSeq(pci, &parse_info);

    if (parse_info.next_token.type != ENDFILE)
        pci->debug_file.Out("Error: Code ends before file ends");

    // If no declarations, the whole program is just statements
    if (decl_tree == 0)
        return stmt_tree;

    // FIX: Connect the end of the declaration list to the statements
    TreeNode *tmp = decl_tree;
    while (tmp->sibling != nullptr) // traverse all declarations
        tmp = tmp->sibling;

    // Now tmp is the LAST declaration node
    tmp->sibling = stmt_tree;

    return decl_tree;
    // return stmt_tree;
}

void PrintTree(TreeNode *node, int sh = 0)
{
    int i, NSH = 3;
    for (i = 0; i < sh; i++)
        printf(" ");
    if (node->node_kind == DECL_NODE)
        printf("[decl=%s]", ExprDataTypeStr[node->expr_data_type]);

    if (node->node_kind == OPER_NODE)
        printf("[%s]", TokenTypeStr[node->oper]);

    if (node->node_kind == INT_NODE)
        printf("[%d]", node->num);

    if (node->node_kind == REAL_NODE)
        printf("[%lf]", node->real_num); // FIX: prints doubles correctly

    if (node->node_kind == ASSIGN_NODE)
    {
        printf("[Assign]");
    }
    else if (node->node_kind == READ_NODE)
    {
        printf("[Read]");
    }
    else if (node->node_kind == WRITE_NODE)
    {
        printf("[Write]");
    }
    else if (node->node_kind == ID_NODE)
    {
        if (node->id)
            printf("[%s]", node->id);
        else
            printf("[ID(NULL)]");
    }

    if (node->expr_data_type != VOID)
        printf("[%s]", ExprDataTypeStr[node->expr_data_type]);

    printf("\n");

    for (i = 0; i < MAX_CHILDREN; i++)
        if (node->child[i])
            PrintTree(node->child[i], sh + NSH);
    if (node->sibling)
        PrintTree(node->sibling, sh);
}

void DestroyTree(TreeNode *node)
{
    if (!node)
        return;

    // Free only ID_NODE
    if (node->node_kind == ID_NODE)
    {
        if (node->id)
            delete[] node->id;
    }

    // Recurse children
    for (int i = 0; i < MAX_CHILDREN; i++)
        if (node->child[i])
            DestroyTree(node->child[i]);

    // Recurse siblings
    if (node->sibling)
        DestroyTree(node->sibling);

    delete node;
}

////////////////////////////////////////////////////////////////////////////////////
// Analyzer ////////////////////////////////////////////////////////////////////////

const int SYMBOL_HASH_SIZE = 10007;

struct LineLocation
{
    int line_num;
    LineLocation *next;
};

struct VariableInfo
{
    ExprDataType declared_type;
    char *name;
    int memloc;
    LineLocation *head_line; // the head of linked list of source line locations
    LineLocation *tail_line; // the tail of linked list of source line locations
    VariableInfo *next_var;  // the next variable in the linked list in the same hash bucket of the symbol table
};

struct SymbolTable
{
    int num_vars;
    VariableInfo *var_info[SYMBOL_HASH_SIZE];

    SymbolTable()
    {
        num_vars = 0;
        int i;
        for (i = 0; i < SYMBOL_HASH_SIZE; i++)
            var_info[i] = 0;
    }

    int Hash(const char *name)
    {
        int i, len = strlen(name);
        int hash_val = 11;
        for (i = 0; i < len; i++)
            hash_val = (hash_val * 17 + (int)name[i]) % SYMBOL_HASH_SIZE;
        return hash_val;
    }

    VariableInfo *Find(const char *name)
    {
        int h = Hash(name);
        VariableInfo *cur = var_info[h];
        while (cur)
        {
            if (Equals(name, cur->name))
                return cur;
            cur = cur->next_var;
        }
        return 0;
    }

    void Insert(const char *name, int line_num, ExprDataType type = INTEGER)
    {
        LineLocation *lineloc = new LineLocation;
        lineloc->line_num = line_num;
        lineloc->next = 0;

        int h = Hash(name);
        VariableInfo *prev = 0;
        VariableInfo *cur = var_info[h];

        while (cur)
        {
            if (Equals(name, cur->name))
            {
                // just add this line location to the list of line locations of the existing var
                cur->tail_line->next = lineloc;
                cur->tail_line = lineloc;
                return;
            }
            prev = cur;
            cur = cur->next_var;
        }

        VariableInfo *vi = new VariableInfo;
        vi->head_line = vi->tail_line = lineloc;
        vi->next_var = 0;
        vi->memloc = num_vars++;
        AllocateAndCopy(&vi->name, name);

        vi->declared_type = type;

        if (!prev)
            var_info[h] = vi;
        else
            prev->next_var = vi;
    }

    void Print()
    {
        int i;
        for (i = 0; i < SYMBOL_HASH_SIZE; i++)
        {
            VariableInfo *curv = var_info[i];
            while (curv)
            {
                printf("[Var=%s][Mem=%d]", curv->name, curv->memloc);
                LineLocation *curl = curv->head_line;
                while (curl)
                {
                    printf("[Line=%d]", curl->line_num);
                    curl = curl->next;
                }
                printf("\n");
                curv = curv->next_var;
            }
        }
    }

    void Destroy()
    {
        int i;
        for (i = 0; i < SYMBOL_HASH_SIZE; i++)
        {
            VariableInfo *curv = var_info[i];
            while (curv)
            {
                LineLocation *curl = curv->head_line;
                while (curl)
                {
                    LineLocation *pl = curl;
                    curl = curl->next;
                    delete pl;
                }
                VariableInfo *p = curv;
                curv = curv->next_var;
                delete p;
            }
            var_info[i] = 0;
        }
    }
};

void Analyze(TreeNode *node, SymbolTable *symbol_table)
{
    if (!node)
        return;

    auto throwErr = [&](const char *msg)
    {
        printf("ERROR at line %d: %s\n", node->line_num, msg);
        throw 0;
    };

    auto isNumeric = [&](ExprDataType t)
    {
        return t == INTEGER || t == REAL_TYPE;
    };

    if (node->node_kind == DECL_NODE)
    {

        VariableInfo *existing = symbol_table->Find(node->child[0]->id);
        if (existing)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Variable '%s' was declared before.", node->child[0]->id);
            throwErr(msg);
        }

        symbol_table->Insert(node->child[0]->id, node->line_num, node->declared_type);
        // node->expr_data_type = VOID;

        if (node->sibling)
            Analyze(node->sibling, symbol_table);
        return;
    }

    for (int i = 0; i < MAX_CHILDREN; ++i)
    {
        if (node->child[i])
            Analyze(node->child[i], symbol_table);
    }

    if (node->node_kind == INT_NODE)
    {
        node->expr_data_type = INTEGER;
    }

    else if (node->node_kind == REAL_NODE)
    {
        node->expr_data_type = REAL_TYPE;
    }

    else if (node->node_kind == BOOL_NODE)
    {
        node->expr_data_type = BOOLEAN;
    }

    else if (node->node_kind == ID_NODE)
    {
        VariableInfo *var = symbol_table->Find(node->id);
        if (!var)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Variable '%s' used before declaration.", node->id);
            throwErr(msg);
        }
        symbol_table->Insert(node->id, node->line_num, var->declared_type);
        node->expr_data_type = var->declared_type;
    }

    else if (node->node_kind == OPER_NODE)
    {
        if (!node->child[0] || !node->child[1])
            throwErr("Operator node missing operand(s).");

        ExprDataType L = node->child[0]->expr_data_type;
        ExprDataType R = node->child[1]->expr_data_type;

        switch (node->oper)
        {
        case PLUS:
        case MINUS:
        case TIMES:
        case POWER:
        case DIVIDE:
        case BINARY_AND:
            if (L == BOOLEAN || R == BOOLEAN)
                throwErr("Arithmetic operator cannot be applied to BOOLEAN.");
            if (!isNumeric(L) || !isNumeric(R))
                throwErr("Arithmetic requires INTEGER or REAL operands.");

            node->expr_data_type = (L == REAL_TYPE || R == REAL_TYPE) ? REAL_TYPE : INTEGER;
            break;

        case LESS_THAN:
        case EQUAL:
            if (!isNumeric(L) || !isNumeric(R))
                throwErr("Relational operators require numeric operands.");
            node->expr_data_type = BOOLEAN;
            break;

        default:
            throwErr("Unknown operator.");
        }
    }

    else if (node->node_kind == ASSIGN_NODE)
    {
        if (!node->child[0])
            throwErr("Assignment missing LHS expression.");
        if (!node->child[1])
            throwErr("Assignment missing RHS expression.");

        TreeNode *lhs = node->child[0]; // ID
        TreeNode *rhs = node->child[1]; // expr

        VariableInfo *var = symbol_table->Find(lhs->id);
        if (!var)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Variable '%s' used before declaration.", node->id);
            throwErr(msg);
        }

        ExprDataType rhsType = rhs->expr_data_type;

        if (var->declared_type != rhsType && var->declared_type != REAL_TYPE)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Type mismatch: variable '%s' is %s but assigned %s.",
                     lhs->id, ExprDataTypeStr[var->declared_type], ExprDataTypeStr[rhsType]);
            throwErr(msg);
        }
        node->expr_data_type = VOID;
    }

    else if (node->node_kind == READ_NODE)
    {
        TreeNode *idNode = node->child[0];

        if (idNode->node_kind != ID_NODE)
        {
            printf("ERROR: READ expects an identifier at line %d\n",
                   node->line_num);
        }

        // VariableInfo *var = symbol_table->Find(node->id);
        VariableInfo *var = symbol_table->Find(idNode->id);
        if (!var)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Variable '%s' used before declaration.", idNode->id);
            throwErr(msg);
        }
        // node->expr_data_type = VOID;
        symbol_table->Insert(idNode->id, node->line_num, var->declared_type);
        idNode->expr_data_type = var->declared_type;
    }

    else if (node->node_kind == WRITE_NODE)
    {
        if (!node->child[0])
            throwErr("WRITE missing expression.");
        ExprDataType t = node->child[0]->expr_data_type;
        if (t == BOOLEAN)
            throwErr("Cannot WRITE BOOLEAN type.");
        if (!isNumeric(t))
            throwErr("WRITE requires numeric expression.");
        node->expr_data_type = VOID;
    }

    else if (node->node_kind == IF_NODE)
    {
        if (!node->child[0])
            throwErr("IF missing condition.");
        if (node->child[0]->expr_data_type != BOOLEAN)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "IF condition must be BOOLEAN, found %s.",
                     ExprDataTypeStr[node->child[0]->expr_data_type]);
            throwErr(msg);
        }
        node->expr_data_type = VOID;
    }

    else if (node->node_kind == REPEAT_NODE)
    {
        if (!node->child[1])
            throwErr("REPEAT missing test expression.");
        if (node->child[1]->expr_data_type != BOOLEAN)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "REPEAT test expression must be BOOLEAN, found %s.",
                     ExprDataTypeStr[node->child[1]->expr_data_type]);
            throwErr(msg);
        }
        node->expr_data_type = VOID;
    }

    if (node->sibling)
        Analyze(node->sibling, symbol_table);
}

////////////////////////////////////////////////////////////////////////////////////
// Code Generator //////////////////////////////////////////////////////////////////

double Power(double a, double b)
{
    if (a == 0)
        return 0;
    if (b == 0)
        return 1;
    if (b >= 1)
        return a * Power(a, b - 1);
    return 0;
}

double BinaryAnd(double a, double b)
{
    return a * a - b * b;
}

double Evaluate(TreeNode *node, SymbolTable *symbol_table, double *variables)
{
    if (node->node_kind == INT_NODE)
        return (double)node->num;
    if (node->node_kind == REAL_NODE)
        return node->real_num;
    if (node->node_kind == BOOL_NODE)
        return node->bool_val ? 1.0 : 0.0;

    if (node->node_kind == ID_NODE)
    {
        VariableInfo *vi = symbol_table->Find(node->id);
        if (!vi)
        {
            printf("RUNTIME ERROR: Undefined variable %s at line %d\n",
                   node->id, node->line_num);
            return 0.0;
        }
        return variables[vi->memloc];
    }

    double a = Evaluate(node->child[0], symbol_table, variables);
    double b = Evaluate(node->child[1], symbol_table, variables);

    if (node->oper == EQUAL)
        return a == b ? 1.0 : 0.0;
    if (node->oper == LESS_THAN)
        return a < b ? 1.0 : 0.0;
    if (node->oper == PLUS)
        return a + b;
    if (node->oper == MINUS)
        return a - b;
    if (node->oper == TIMES)
        return a * b;
    if (node->oper == DIVIDE)
        return a / b;
    if (node->oper == POWER)
        return Power(a, b);
    if (node->oper == BINARY_AND)
        return BinaryAnd(a, b);

    printf("RUNTIME ERROR: Unknown operator at line %d\n", node->line_num);
    throw 0;
}

void RunProgram(TreeNode *node, SymbolTable *symbol_table, double *variables)
{
    if (node->node_kind == IF_NODE)
    {
        double cond = Evaluate(node->child[0], symbol_table, variables);
        if (cond != 0.0)
            RunProgram(node->child[1], symbol_table, variables);
        else if (node->child[2])
            RunProgram(node->child[2], symbol_table, variables);
    }
    if (node->node_kind == ASSIGN_NODE)
    {
        TreeNode *lhs = node->child[0];
        TreeNode *rhs = node->child[1];

        VariableInfo *vi = symbol_table->Find(lhs->id);
        if (!vi)
        {
            printf("RUNTIME ERROR: Undefined variable %s at line %d\n",
                   lhs->id, node->line_num);
        }

        double value = Evaluate(rhs, symbol_table, variables);
        variables[(int)vi->memloc] = value;
    }
    if (node->node_kind == READ_NODE)
    {
        TreeNode *idNode = node->child[0];
        VariableInfo *vi = symbol_table->Find(idNode->id);

        printf("Enter %s: ", idNode->id);

        if (vi->declared_type == INTEGER)
        {
            int tmp;
            scanf("%d", &tmp);
            variables[(int)vi->memloc] = (double)tmp;
        }
        else if (vi->declared_type == REAL_TYPE)
        {
            double tmp;
            scanf("%lf", &tmp);
            variables[(int)vi->memloc] = tmp;
        }
        else if (vi->declared_type == BOOLEAN)
        {
            int tmp;
            scanf("%d", &tmp);
            variables[(int)vi->memloc] = (tmp != 0) ? 1.0 : 0.0;
        }
    }
    if (node->node_kind == WRITE_NODE)
    {
        double v = Evaluate(node->child[0], symbol_table, variables);

        if (node->child[0]->expr_data_type == INTEGER)
            printf("Val: %d\n", (int)v);
        else if (node->child[0]->expr_data_type == REAL_TYPE)
            printf("Val: %lf\n", v);
        else if (node->child[0]->expr_data_type == BOOLEAN)
            printf("Val: %s\n", v != 0 ? "true" : "false");
    }
    if (node->node_kind == REPEAT_NODE)
    {
        do
        {
            RunProgram(node->child[0], symbol_table, variables);
        } while (!Evaluate(node->child[1], symbol_table, variables));
    }
    if (node->sibling)
        RunProgram(node->sibling, symbol_table, variables);
}

void RunProgram(TreeNode *syntax_tree, SymbolTable *symbol_table)
{
    int i;
    double *variables = new double[symbol_table->num_vars];
    for (i = 0; i < symbol_table->num_vars; i++)
        variables[i] = 0;
    RunProgram(syntax_tree, symbol_table, variables);
    delete[] variables;
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner and Compiler ////////////////////////////////////////////////////////////

void StartCompiler(CompilerInfo *pci)
{
    TreeNode *syntax_tree = Parse(pci);

    SymbolTable symbol_table;
    Analyze(syntax_tree, &symbol_table);

    printf("Symbol Table:\n");
    symbol_table.Print();
    printf("---------------------------------\n");
    fflush(NULL);

    printf("Syntax Tree:\n");
    PrintTree(syntax_tree);
    printf("---------------------------------\n");
    fflush(NULL);

    printf("Run Program:\n");
    RunProgram(syntax_tree, &symbol_table);
    printf("---------------------------------\n");
    fflush(NULL);

    symbol_table.Destroy();
    DestroyTree(syntax_tree);
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner only ////////////////////////////////////////////////////////////////////

void StartScanner(CompilerInfo *pci)
{
    Token token;

    while (true)
    {
        GetNextToken(pci, &token);
        printf("[%d] %s (%s)\n", pci->in_file.cur_line_num, token.str, TokenTypeStr[token.type]);
        fflush(NULL);
        if (token.type == ENDFILE || token.type == ERROR)
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////

int main()
{
    printf("Start main()\n");
    fflush(NULL);

    CompilerInfo compiler_info("input.txt", "output.txt", "debug.txt");

    StartCompiler(&compiler_info);

    printf("End main()\n");
    fflush(NULL);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////