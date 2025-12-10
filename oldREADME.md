# README -- Compilers Assignment 1

## Overview

This assignment requires modifying the provided compiler implementation
(`CompilersTask_3_CodeGenerator.cpp`) to add support for a new operator:
**binary &**.\
This operator is *not* a bitwise AND. Its meaning is explicitly defined
as:

    a & b  =  (a^2) - (b^2)

Example:\
`3 & 2 = 9 - 4 = 5`

The operator must be fully supported in the **scanner**, **parser**,
**semantic analyzer**, and **code generator / runtime evaluator**.

You must also include a test tiny program (20+ test cases) and ensure
the entire file follows the strict coding style, character limit, and
allowed library rules.

------------------------------------------------------------------------

## Operator Definition

### Name

Binary AND (`&`)

### Associativity

Left-associative

    a & b & c → (a & b) & c

### Precedence

-   Higher than `*`, `/`
-   Lower than `^`

### Required Grammar Change

The updated grammar including the new operator is:

    // program -> stmtseq
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
    // newexpr -> ( mathexpr ) | number | identifier

------------------------------------------------------------------------

## Constraints

-   Only allowed headers: `<cstdlib>`, `<cstdio>`, `<cstring>`,
    `<iostream>`
-   **No STL** (no string, no vector, no map, no algorithm)
-   Max file size: **15,000 characters**
-   No tab characters (only spaces)
-   Coding style must strictly follow **CodingStyle.pdf**
-   Each team member must be able to explain all changes
-   Comments and test cases form **half of the grade**

------------------------------------------------------------------------

## Required Implementation Changes

### 1. Scanner (Lexical Analyzer)

-   Add `BINARY_AND` to `TokenType`
-   Add `"BinaryAnd"` to `TokenTypeStr`
-   Add `BINARY_AND, &` Token to `symbolic_tokens` list

### 2. Parser (Syntax Analyzer)

-   Add rule:

        andexpr -> factor { (&) factor }

-   Implement `AndExpr`

-   Update `term` to call `andexpr`

-   Ensure precedence ordering is correct

### 3. Semantic Analyzer

Add type checks: - Node type for `BINARY_AND` must be `INTEGER` - Both
operands must be `INTEGER`

### 4. Code Generator / Runtime Evaluator

-   Implement:

        int BinaryAnd(int a, int b) { return (a*a) - (b*b); }

-   Add evaluation logic for `BINARY_AND`

-   Update `RunProgram(TreeNode*, SymbolTable*, int*)`

------------------------------------------------------------------------

## Miscellaneous Requirements

-   Include **20+ test statements** involving `&` inside a comment block
    in the final `.cpp` file
-   Reformat code to match CodingStyle.pdf
-   Ensure no tabs and file below 15k characters

------------------------------------------------------------------------

# Task Breakdown for the Team

### Scanner

-   [x] Add `BINARY_AND` to `TokenType`
-   [x] Add `"BinaryAnd"` to `TokenTypeStr`
-   [x] Add `BINARY_AND, &` Token to `symbolic_tokens` list

### Parser

-   [x] Add BNF rule for `andexpr`
-   [x] Implement parser for `&`:  `BinaryAndExpr`
-   [x] Update `term` to use `andexpr`
-   [x] Validate precedence correctness

### Semantic Analyzer

-   [x] Add type-checking for `BINARY_AND`
-   [x] Ensure operands are integers

### Code Generator / Evaluator

-   [x] Implement `BinaryAnd(int,int)`
-   [x] Update `Evaluate`
-   [x] Update `RunProgram`

### Misc

-   [x] Write 20+ `&` operator test cases
-   [x] Reformat code to meet CodingStyle.pdf
-   [x] Remove tabs and keep file \< 15000 chars
-   [x] Modify main