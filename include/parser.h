/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

// Basic node types for AST
typedef enum {
    AST_PROGRAM,        // Program node
    AST_VARDECL,        // Variable declaration (int x)
    AST_ASSIGN,         // Assignment (x = 5)
    AST_PRINT,          // Print statement
    AST_NUMBER,         // Number literal
    AST_STRING_LITERAL,  // String literal
    AST_IDENTIFIER,     // Variable name
    AST_IF,
    AST_CONDITION,
    AST_WHILE,
    AST_REPEAT,
    AST_BLOCK,
    AST_FACTORIAL,
    AST_BINOP,
    AST_COMPARISON
} ASTNodeType;

typedef enum {
    PARSE_ERROR_NONE,
    PARSE_ERROR_UNEXPECTED_TOKEN,
    PARSE_ERROR_MISSING_SEMICOLON,
    PARSE_ERROR_MISSING_IDENTIFIER,
    PARSE_ERROR_MISSING_EQUALS,
    PARSE_ERROR_INVALID_EXPRESSION,
    PARSE_ERROR_MISSING_LPAREN,
    PARSE_ERROR_MISSING_RPAREN,
    PARSE_ERROR_MISSING_LBRACE,
    PARSE_ERROR_MISSING_RBRACE,
    PARSE_ERROR_MISSING_LBRACK,
    PARSE_ERROR_MISSING_RBRACK,
    PARSE_ERROR_INVALID_STATEMENT,
    PARSE_ERROR_MISSING_UNTIL,
    PARSE_ERROR_INVALID_COMPARISON,
} ParseError;

typedef enum {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_FLOAT,
    TYPE_STRING
} VarType;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    Token token;               // Token associated with this node
    struct ASTNode* left;      // Left child
    struct ASTNode* right;     // Right child
    struct ASTNode* next;
    VarType var_type;         // Variable type (if applicable)
} ASTNode;

// Parser functions
void parser_init(const char* input);
ASTNode* parse(void);
void print_ast(ASTNode* node, int level);
void free_ast(ASTNode* node);
const char* var_type_to_string(VarType type);

#endif /* PARSER_H */