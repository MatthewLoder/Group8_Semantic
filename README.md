# Phase 2: Syntax Analysis - Parser Implementation

## Overview
In this phase, you'll build upon your **custom lexical analyzer to create a parser** that constructs an Abstract Syntax Tree (AST) from the token stream. The parser validates the syntactic structure of the input according to the language grammar and creates a tree representation suitable for further processing.

## Understanding Parser Implementation and AST Transformation

### What is Parsing?
Parsing is the process of converting a flat sequence of tokens into a hierarchical tree structure that represents the syntactic organization of the program.

### Abstract Syntax Tree (AST)
An Abstract Syntax Tree is a tree representation that:
- Captures the essential syntactic elements
- Removes unnecessary syntactic details
- Provides a clean, hierarchical view of the code

## Getting Started

### Project Structure
```
phase2/
├── include/
│   ├── tokens.h        # Token definitions from Phase 1
│   ├── lexer.h        # Lexer interface
│   └── parser.h       # Parser definitions
├── src/
│   ├── lexer/
│   │   └── lexer.c    # Lexer implementation from Phase 1
│   └── parser/
│       └── parser.c   # Parser implementation
└── test/
    ├── input_valid.txt
    └── input_invalid.txt
```

### Current Implementation
The starter code provides:
- Basic AST node structure
- Simple statement parsing (declarations and assignments)
- Error reporting framework
- AST printing utilities

Example of currently supported syntax:
```c
int x;      // Variable declaration
x = 42;     // Assignment statement
```

## Parser Implementation Guide

### Step 1: Understand the Lexer Output
1. Review your lexer's token generation
2. Print out tokens to understand the input
3. Verify token sequence before parsing

```c
// Example of examining tokens
void print_token_stream(const char* input) {
    int position = 0;
    Token token;
    
    do {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);
}
```

### Step 2: Parser Initialization
1. Create a parser context
2. Set up initial parsing state
3. Prepare for token consumption

```c
// Basic parser initialization
void parser_init(const char *input) {
    source = input;
    position = 0;
    advance(); // Get first token
}
```

### Step 3: Implement Parsing Functions

#### Token Matching and Consumption
```c
// Check if current token matches expected type
static int match(TokenType type) {
    return current_token.type == type;
}

// Consume current token and move to next
static void advance(void) {
    current_token = get_next_token(source, &position);
}

// Expect specific token or report error
static void expect(TokenType type) {
    if (match(type)) {
        advance();
    } else {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
    }
}
```

## Tasks to Complete

### 1. Expression Parsing
- [ ] Implement operator precedence
- [ ] Add support for:
  - Binary operations (+, -, *, /)
  - Comparison operators (<, >, ==, !=)
  - Parenthesized expressions

### 2. Statement Types
- [ ] If statements: `if (condition) { statements }`
- [ ] While loops: `while (condition) { statements }`
- [ ] Repeat-Until: `repeat { statements } until (condition)`
- [ ] Print statements: `print expression;`
- [ ] Block statements: `{ statement1; statement2; }`

### 3. Special Features
- [ ] Factorial function support
- [ ] Block scoping

### 4. Error Handling
- [ ] Extend error types
- [ ] Improve error messages
- [ ] Add error recovery
- [ ] Track line and column information

## Detailed Implementation Strategies

### AST Node Creation
```c
// Create AST node with basic information
static ASTNode *create_node(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node) {
        node->type = type;
        node->token = current_token;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}
```

### Sample Parsing Functions
```c
// Parse variable declaration
static ASTNode *parse_declaration(void) {
    ASTNode *node = create_node(AST_VARDECL);
    advance(); // consume 'int'

    if (!match(TOKEN_IDENTIFIER)) {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        return NULL;
    }

    node->token = current_token;
    advance();

    expect(TOKEN_SEMICOLON);
    return node;
}

// Parse assignment statement
static ASTNode *parse_assignment(void) {
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;
    advance();

    expect(TOKEN_EQUALS);
    node->right = parse_expression();
    expect(TOKEN_SEMICOLON);

    return node;
}
```

## Testing and Debugging

### Sample Test Cases
```c
// Valid syntax
int x;
x = 42;
if (x > 0) {
    print x;
}

// Error cases
if (x > ) {     // Invalid expression
while x > 0 {   // Missing parentheses
int ;           // Missing identifier
```

### AST Printing for Debugging
```c
void print_ast(ASTNode *node, int level) {
    if (!node) return;

    // Indent and print node details
    for (int i = 0; i < level; i++) printf("  ");
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            break;
        case AST_VARDECL:
            printf("VarDecl: %s\n", node->token.lexeme);
            break;
        // Add more node type printings
    }

    // Recursively print children
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}
```

## Tips for Success
- Start with the most straightforward possible implementation
- Add complexity incrementally
- Test each feature thoroughly
- Focus on error handling
- Document your changes
- Update AST printing for debugging

## Common Challenges
1. Handling complex expressions
2. Implementing error recovery
3. Managing memory for AST nodes
4. Supporting various language constructs

## Submission Requirements
1. Complete implementation of your custom parser.c
2. Test files demonstrating your features
3. Documentation of your:
   - Grammar rules
   - Error handling strategy
   - AST structure
   - Test cases

## Learning Milestones
- [ ] Parse simple declarations
- [ ] Handle basic assignments
- [ ] Implement expression parsing
- [ ] Add error handling
- [ ] Support more complex statements
- [ ] Optimize AST generation

## Recommended Resources
- Compiler design textbooks
- Online parsing tutorials
- Lecture notes on syntax analysis

Good luck! 🚀
