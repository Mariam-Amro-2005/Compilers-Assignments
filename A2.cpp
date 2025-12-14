// Mariam Amro 20221217 Sobhi Mohamed 20221088 Sama Ashraf 20220436 Ahmed Saeed 20220040 Josiane Usama 20220502
TreeNode *NewExpr(CompilerInfo *pci, ParseInfo *ppi)
{
    TokenType p = ppi->next_token.type;
    pci->debug_file.Out("Start NewExpr");
    if (p == INT_NUM || p == REAL_NUM)
    {
        TreeNode *tree = new TreeNode;
        if (p == INT_NUM)
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
        Match(pci, ppi, p);
        pci->debug_file.Out("End NewExpr");
        return tree;
    }
    if (p == TRUE || p == FALSE)
    {
        TreeNode *tree = new TreeNode;
        tree->node_kind = BOOL_NODE;
        tree->expr_data_type = BOOLEAN;
        tree->bool_val = (p == TRUE);
        tree->line_num = pci->in_file.cur_line_num;
        Match(pci, ppi, p);
        pci->debug_file.Out("End NewExpr");
        return tree;
    }
    if (p == ID)
    {
        TreeNode *tree = new TreeNode;
        tree->node_kind = ID_NODE;
        AllocateAndCopy(&tree->id, ppi->next_token.str);
        tree->line_num = pci->in_file.cur_line_num;
        Match(pci, ppi, p);
        pci->debug_file.Out("End NewExpr");
        return tree;
    }
    if (p == LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode *tree = MathExpr(pci, ppi);
        Match(pci, ppi, RIGHT_PAREN);
        pci->debug_file.Out("End NewExpr");
        return tree;
    }
    throw 0;
}
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
    tree->child[0] = idNode;
    pci->debug_file.Out("End ReadStmt");
    return tree;
}
TreeNode *AssignStmt(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start AssignStmt");
    TreeNode *tree = new TreeNode;
    tree->node_kind = ASSIGN_NODE;
    tree->line_num = pci->in_file.cur_line_num;
    TreeNode *idNode = new TreeNode;
    idNode->node_kind = ID_NODE;
    AllocateAndCopy(&idNode->id, ppi->next_token.str);
    idNode->line_num = pci->in_file.cur_line_num;
    Match(pci, ppi, ID);
    Match(pci, ppi, ASSIGN);
    tree->child[0] = idNode;
    tree->child[1] = Expr(pci, ppi);
    pci->debug_file.Out("End AssignStmt");
    return tree;
}
TreeNode *Decl(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Decl");
    TokenType p = ppi->next_token.type;
    TreeNode *tree = new TreeNode();
    tree->node_kind = DECL_NODE;
    tree->line_num = pci->in_file.cur_line_num;
    if (p == INT || p == REAL || p == BOOL)
    {
        tree->expr_data_type = tree->declared_type = (p == INT ? INTEGER : (p == REAL ? REAL_TYPE : BOOLEAN));
        Match(pci, ppi, p);
    }
    else
        throw 0;
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
TreeNode *Decls(CompilerInfo *pci, ParseInfo *ppi)
{
    pci->debug_file.Out("Start Decls");
    TokenType p = ppi->next_token.type;
    if (p != INT && p != REAL && p != BOOL)
        return 0;
    TreeNode *first_decl = Decl(pci, ppi);
    TreeNode *last_decl = first_decl;
    while (p == SEMI_COLON)
    {
        Match(pci, ppi, SEMI_COLON);
        if (p != INT && p != REAL && p != BOOL)
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
    TreeNode *decl_tree = Decls(pci, &parse_info);
    TreeNode *stmt_tree = StmtSeq(pci, &parse_info);
    if (parse_info.next_token.type != ENDFILE)
        pci->debug_file.Out("Error: Code ends before file ends");
    if (decl_tree == 0)
        return stmt_tree;
    TreeNode *tmp = decl_tree;
    while (tmp->sibling != nullptr)
        tmp = tmp->sibling;
    tmp->sibling = stmt_tree;
    return decl_tree;
}
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
    NodeKind nkind = node->node_kind;
    if (nkind == DECL_NODE)
    {
        VariableInfo *existing = symbol_table->Find(node->child[0]->id);
        if (existing)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Variable '%s' was declared before.", node->child[0]->id);
            throwErr(msg);
        }
        symbol_table->Insert(node->child[0]->id, node->line_num, node->declared_type);
        if (node->sibling)
            Analyze(node->sibling, symbol_table);
        return;
    }
    for (int i = 0; i < MAX_CHILDREN; ++i)
    {
        if (node->child[i])
            Analyze(node->child[i], symbol_table);
    }
    if (nkind == INT_NODE)
    {
        node->expr_data_type = INTEGER;
    }
    else if (nkind == REAL_NODE)
    {
        node->expr_data_type = REAL_TYPE;
    }
    else if (nkind == BOOL_NODE)
    {
        node->expr_data_type = BOOLEAN;
    }
    else if (nkind == ID_NODE)
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
    else if (nkind == OPER_NODE)
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
    else if (nkind == ASSIGN_NODE)
    {
        if (!node->child[0])
            throwErr("Assignment missing LHS expression.");
        if (!node->child[1])
            throwErr("Assignment missing RHS expression.");
        TreeNode *lhs = node->child[0];
        TreeNode *rhs = node->child[1];
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
            snprintf(msg, sizeof(msg), "Type mismatch: variable '%s' is %s but assigned %s.", lhs->id, ExprDataTypeStr[var->declared_type], ExprDataTypeStr[rhsType]);
            throwErr(msg);
        }
        node->expr_data_type = VOID;
    }
    else if (nkind == READ_NODE)
    {
        TreeNode *idNode = node->child[0];
        if (idNode->node_kind != ID_NODE)
            printf("ERROR: READ expects an identifier at line %d\n", node->line_num);
        VariableInfo *var = symbol_table->Find(idNode->id);
        if (!var)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Variable '%s' used before declaration.", idNode->id);
            throwErr(msg);
        }
        symbol_table->Insert(idNode->id, node->line_num, var->declared_type);
        idNode->expr_data_type = var->declared_type;
    }
    else if (nkind == WRITE_NODE)
    {
        if (!node->child[0])
            throwErr("WRITE missing expression.");
        node->expr_data_type = VOID;
    }
    else if (nkind == IF_NODE)
    {
        if (!node->child[0])
            throwErr("IF missing condition.");
        if (node->child[0]->expr_data_type != BOOLEAN)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "IF condition must be BOOLEAN, found %s.", ExprDataTypeStr[node->child[0]->expr_data_type]);
            throwErr(msg);
        }
        node->expr_data_type = VOID;
    }
    else if (nkind == REPEAT_NODE)
    {
        if (!node->child[1])
            throwErr("REPEAT missing test expression.");
        if (node->child[1]->expr_data_type != BOOLEAN)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "REPEAT test expression must be BOOLEAN, found %s.", ExprDataTypeStr[node->child[1]->expr_data_type]);
            throwErr(msg);
        }
        node->expr_data_type = VOID;
    }
    if (node->sibling)
        Analyze(node->sibling, symbol_table);
}
double Evaluate(TreeNode *node, SymbolTable *symbol_table, double *variables)
{
    NodeKind nkind = node->node_kind;
    if (nkind == INT_NODE)
        return (double)node->num;
    if (nkind == REAL_NODE)
        return node->real_num;
    if (nkind == BOOL_NODE)
        return node->bool_val ? 1.0 : 0.0;
    if (nkind == ID_NODE)
    {
        VariableInfo *vi = symbol_table->Find(node->id);
        if (!vi)
        {
            printf("RUNTIME ERROR: Undefined variable %s at line %d\n", node->id, node->line_num);
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
    if (node->oper == DIVIDE){
        if (b == 0){
            printf("RUNTIME ERROR: Division by zero at line %d\n", node->line_num);
            throw 0;
        }
        return a / b;
    }
    if (node->oper == POWER)
        return Power(a, b);
    if (node->oper == BINARY_AND)
        return BinaryAnd(a, b);
    printf("RUNTIME ERROR: Unknown operator at line %d\n", node->line_num);
    throw 0;
}
void RunProgram(TreeNode *node, SymbolTable *symbol_table, double *variables)
{
    NodeKind nkind = node->node_kind;
    if (nkind == IF_NODE)
    {
        double cond = Evaluate(node->child[0], symbol_table, variables);
        if (cond != 0.0)
            RunProgram(node->child[1], symbol_table, variables);
        else if (node->child[2])
            RunProgram(node->child[2], symbol_table, variables);
    }
    if (nkind == ASSIGN_NODE)
    {
        TreeNode *lhs = node->child[0];
        TreeNode *rhs = node->child[1];
        VariableInfo *vi = symbol_table->Find(lhs->id);
        if (!vi)
            printf("RUNTIME ERROR: Undefined variable %s at line %d\n", lhs->id, node->line_num);
        double value = Evaluate(rhs, symbol_table, variables);
        variables[(int)vi->memloc] = value;
    }
    if (nkind == READ_NODE)
    {
        TreeNode *idNode = node->child[0];
        VariableInfo *vi = symbol_table->Find(idNode->id);
        printf("Enter %s: ", idNode->id);
        ExprDataType vt = vi->declared_type;
        if (vt == INTEGER)
        {
            int tmp;
            scanf("%d", &tmp);
            variables[(int)vi->memloc] = (double)tmp;
        }
        else if (vt == REAL_TYPE)
        {
            double tmp;
            scanf("%lf", &tmp);
            variables[(int)vi->memloc] = tmp;
        }
        else if (vt == BOOLEAN)
        {
            int tmp;
            scanf("%d", &tmp);
            variables[(int)vi->memloc] = (tmp != 0) ? 1.0 : 0.0;
        }
    }
    if (nkind == WRITE_NODE)
    {
        double v = Evaluate(node->child[0], symbol_table, variables);
        ExprDataType vt = node->child[0]->expr_data_type;
        if (vt == INTEGER)
            printf("Val: %d\n", (int)v);
        else if (vt == REAL_TYPE)
            printf("Val: %lf\n", v);
        else if (vt == BOOLEAN)
            printf("Val: %s\n", v != 0 ? "true" : "false");
    }
    if (nkind == REPEAT_NODE)
    {
        do
        {
            RunProgram(node->child[0], symbol_table, variables);
        } while (!Evaluate(node->child[1], symbol_table, variables));
    }
    if (node->sibling)
        RunProgram(node->sibling, symbol_table, variables);
}