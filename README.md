# README — Compilers Assignment 2

## Overview

This assignment extends the compiler from Assignment 1 to support three data types: `int`, `real`, and `bool`. The `real` type corresponds to `double` in C++. Variables must be declared at the top of the program. The compiler must enforce type rules, prevent disallowed uses (for example arithmetic on `bool`), support numeric mixing (promoting `int` to `real` where needed), and produce meaningful compile-time errors for invalid programs. You must also include a tiny test program (at least 20 statements) inside a comment block that demonstrates correctness and error handling.

---

## Key Language Rules (summary)

* Types: `int`, `real` (C++ `double`), `bool`.
* All variable declarations must appear at the beginning of the program (a block of declarations).
* `if` and `repeat … until` conditions must evaluate to `bool`. `int` and `real` are not allowed as direct conditions.
* No arithmetic operations on `bool` values.
* Arithmetic is allowed on `int` and `real`. When mixing, `int` is promoted to `real` and the result is `real`.
* Assignments must be to a variable of the same declared type. Assigning between different types is an error.
* All disallowed cases must produce meaningful compiler errors (use exceptions).

---

## Final corrected full BNF (clean format)

```
program        → decls stmtseq

decls          → decl { ; decl }

decl           → datatype id

datatype       → 'int' | 'real' | 'bool'

stmtseq        → stmt { ';' stmt }

stmt           → ifstmt
               | repeatstmt
               | assignstmt
               | readstmt
               | writestmt

ifstmt         → 'if' expr 'then' stmtseq [ 'else' stmtseq ] 'end'

repeatstmt     → 'repeat' stmtseq 'until' expr

assignstmt     → identifier ':=' expr

readstmt       → 'read' identifier

writestmt      → 'write' expr

expr           → mathexpr [ ( '<' | '=' ) mathexpr ]

mathexpr       → term { ( '+' | '-' ) term }           (left associative)

term           → andexpr { ( '*' | '/' ) andexpr }     (left associative)

andexpr        → factor { '&' factor }                 (left associative)

factor         → newexpr { '^' newexpr }               (right associative)

newexpr        → '(' mathexpr ')'
               | number
               | identifier
```

Notes:

* `decls` must be a contiguous block at the top of the program. No declarations are allowed after the first non-declaration statement.
* Numeric literals may be parsed as integers or reals depending on lexical rules; semantic analysis should assign them `INTEGER` or `REAL` appropriately.
* `&` semantics (binary and) remain: `a & b = (a^2) - (b^2)`; keep its precedence between `*`/`/` and `^` as above.

---

## Implementation roadmap (detailed tasks)

### 1) Scanner (lexical)

* Add new reserved words: `"int"`, `"real"`, `"bool"`.
* Add tokens: `INT`, `REAL`, `BOOL` (update `TokenType` enum and `TokenTypeStr[]`).
* Ensure numeric literal scanning distinguishes integer and real literal forms (e.g., `123` → NUM_INT, `1.23` → NUM_REAL) or scan as NUM and decide type later in semantic phase. Prefer scanning numeric literals with both forms so the parser/semantic phase can set correct literal types.

### 2) Parser (syntax)

* Add parsing for declarations:

    * `Decl()` to parse `datatype ID ;`
    * `Decls()` to consume zero or more `decl` at program start
    * Modify `Parse()` (or `ParseProgram`) to call `Decls()` before `StmtSeq`.
* Add `ParseDatatype()` helper if you prefer (returns a token/type).
* Do not intermix declarations and statements: parser should stop accepting declarations when the first statement token appears.
* Keep expression parsing as in the BNF above; ensure `Term()` begins with the `andexpr` rule (as fixed previously).

### 3) AST / Node updates

* Extend `ExprDataType` enum: `VOID`, `INTEGER`, `REAL`, `BOOLEAN`.
* Optionally add a `DECL_NODE` or annotate `ID_NODE` with declared type (stored in symbol table). If you add `DECL_NODE`, it helps debugging and analysis.
* Ensure numeric literal nodes carry their type (`INTEGER` or `REAL`).

### 4) Symbol table

* When inserting variable declarations:

    * Store declared type (INT/REAL/BOOL) with each `VariableInfo`.
    * Store declaration line; use it to verify declarations are only at the top.
* On first pass (analysis), verify that all variables were declared and that declarations happen before statements.

### 5) Semantic Analyzer (type checking)

* Convert existing `Analyze()` to:

    * Add type fields to variable entries.
    * For each `OPER_NODE`, determine operand types and compute result type applying rules:

        * Arithmetic ops (`+`, `-`, `*`, `/`, `^`) accept only `INTEGER` or `REAL`. If one operand is `REAL`, promote the other to `REAL` and result is `REAL`. If both `INTEGER` → result `INTEGER` (except dividing may produce `REAL` depending on spec: if integer division is allowed, document your decision; assignment asks mixing results be `real` — keep divide behavior consistent).
        * `BINARY_AND (&)` as earlier: defined in terms of integers only (assignment 1 specified it uses integer math), so if you keep it integer-only, ensure both operands are integer. If spec allows real, define and document; safer to keep integer-only.
        * Relational ops (`<`, `=`) yield `BOOLEAN`. They are allowed on numeric types (int/real). Comparing `bool` with numeric should be disallowed.
        * Logical/boolean operators (none new here) would accept booleans only.
    * For `ASSIGN_NODE`: ensure expression type exactly matches the declared variable type. Throw an exception otherwise.
    * For `IF_NODE` and `REPEAT_NODE`: ensure the controlling `expr` type is `BOOLEAN`; throw otherwise.
    * For `READ_NODE` and `WRITE_NODE`: enforce `read` and `write` semantics depending on allowed types (assignment 1 allowed `write` only for integers — update to allow `int` and `real` but not `bool` unless assignment requires).
    * Disallow arithmetic on `BOOLEAN`.
* All semantic errors must throw detailed exceptions indicating the location and the reason.

### 6) Code generation / runtime (Evaluate, RunProgram)

* `Evaluate` must handle both integer and real evaluation:

    * Decide on runtime representation: either separate paths (`int` vs `double`) or eval to `struct Value { ExprDataType type; int ival; double rval; bool bval; }`.
    * Implement coercion: arithmetic on mixed `int` + `real` should convert `int` to `real` before operation and return `REAL`.
    * For assignment, ensure runtime respects declared type. If assignment violated at compile-time, it should have thrown; runtime can assume types are consistent. Still, adding runtime checks helps catch bugs in implementation.
* `RunProgram` unchanged for statements except verify that `Evaluate` returns a `BOOLEAN` when used as a condition and that `READ` reads values into the correct typed slot.
* Memory for variables: maintain arrays for `int` and `double` or a unified `Value` array. Map symbol table `memloc` to the appropriate runtime storage.

### 7) Error messages and exceptions

* Replace `printf`-style error reports with thrown exceptions or `throw std::runtime_error("...")` (or your project's exception mechanism). Error text must be meaningful (name, line number, expected vs found).
* Examples:

    * `throw std::runtime_error("Line 3: If test must be BOOLEAN, found INTEGER")`
    * `throw std::runtime_error("Line 7: Cannot perform '+' on BOOLEAN")`
    * `throw std::runtime_error("Line 4: Assignment type mismatch: variable 'x' declared as REAL, assigned expression of type INTEGER")` (you could allow implicit promotion here if stated; assignment forbids assignments of different types so throw).

### 8) Testing

* Include a comment block in your final `.cpp` containing at least 20 test statements that:

    * Declare several variables of each type
    * Use `if` and `repeat` with boolean conditions
    * Use mixed arithmetic `int` + `real`
    * Use invalid operations on bools
    * Attempt invalid assignments
    * Edge cases (e.g., divide by zero handled at runtime or error documented)
* Provide expected outputs and expected errors as comments.

---

## Example error messages (suggested text)

* `Line X: Variable 'v' redeclared`
* `Line X: Variable 'v' used before declaration`
* `Line X: If test must be BOOLEAN, found INTEGER`
* `Line X: Cannot apply '+' to BOOLEAN`
* `Line X: Assignment type mismatch: variable 'v' declared as REAL but assigned INTEGER` (or simply "types must match")
* `Line X: Declarations must appear before any statement`

---

## Checklist (ready-to-use)

* [ ] Scanner: add `int`, `real`, `bool` tokens and numeric literal support
* [ ] Parser: add `decls`, `decl`, `datatype`; implement `Decls()` and integrate into Parsing
* [ ] AST: add `REAL` type and ensure literal nodes carry type
* [ ] Symbol table: store declared type and declaration line
* [ ] Analyzer: implement type rules, throw exceptions on violations
* [ ] Evaluate: implement typed evaluation and coercion rules
* [ ] RunProgram: handle `read`/`write` for `real` and `int`; ensure conditions are boolean
* [ ] Tests: write ≥20 test statements and annotate expected results
* [ ] Formatting: remove tabs, ensure code < 15,000 characters, follow CodingStyle.pdf

---

## Work Division

* Part 1: Scanner + Parser skeleton
* Part 2: Symbol table + AST updates
* Part 3: Semantic Analyzer
* Part 4: Evaluate / runtime + `read`/`write` updates
* Part 5: Tests + formatting + integration + full-team review 
* Note: Make at least 20 concrete statements, and annotate each with the expected behavior (valid or error message).
---
