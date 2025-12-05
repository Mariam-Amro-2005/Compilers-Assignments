// Mariam Amro Ahmed 20221217 - Sobhi Mohamed 20221088 - Sama Ashraf 20220436 - Ahmed Saeed 20220040 - Josiane Usama 20220502

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
    ENDFILE,
    ERROR
};

const char *TokenTypeStr[] =
{
    "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
    "Assign", "Equal", "LessThan",
    "Plus", "Minus", "Times", "Divide", "BinaryAnd", "Power",
    "SemiColon",
    "LeftParen", "RightParen",
    "LeftBrace", "RightBrace",
    "ID", "Num",
    "EndFile", "Error"
};

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
    Token(RIGHT_BRACE, "}")
};

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

int BinaryAnd(int a, int b)
{
    return a * a - b * b;
}

int Evaluate(TreeNode *node, SymbolTable *symbol_table, int *variables)
{
    if (node->node_kind == NUM_NODE)
        return node->num;
    if (node->node_kind == ID_NODE)
        return variables[symbol_table->Find(node->id)->memloc];

    int a = Evaluate(node->child[0], symbol_table, variables);
    int b = Evaluate(node->child[1], symbol_table, variables);

    if (node->oper == EQUAL)
        return a == b;
    if (node->oper == LESS_THAN)
        return a < b;
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
    throw 0;
}