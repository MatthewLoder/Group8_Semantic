/* semantic.h */
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"

// Basic symbol structure
typedef struct Symbol {
    char name[100];      // Variable name
    int type;            // Data type (int, etc.)
    int scope_level;     // Scope nesting level
    int line_declared;   // Line where declared
    int is_initialized;  // Has been assigned a value?
    struct Symbol *next; // For linked list implementation
} Symbol;

// Symbol table
typedef struct {
    Symbol *head;      // First symbol in the table
    int current_scope; // Current scope level
} SymbolTable;

// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable *init_symbol_table();

// Add a symbol to the table
// Inserts a new variable with given name, type, and line number into the
// current scope
void add_symbol(SymbolTable *table, const char *name, int type, int line);

// Look up a symbol in the table
// Searches for a variable by name across all accessible scopes
// Returns the symbol if found, NULL otherwise
Symbol *lookup_symbol(SymbolTable *table, const char *name);

// Enter a new scope level
// Increments the current scope level when entering a block (e.g., if, while)
void enter_scope(SymbolTable *table);

// Exit the current scope
// Decrements the current scope level when leaving a block
// Optionally removes symbols that are no longer in scope
void exit_scope(SymbolTable *table);

// Remove symbols from the current scope
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable *table);

// Free the symbol table memory
// Releases all allocated memory when the symbol table is no longer needed
void free_symbol_table(SymbolTable *table);

// Main semantic analysis function
int analyze_semantics(ASTNode *ast);

// Check a variable declaration
int check_declaration(ASTNode *node, SymbolTable *table);

// Check a variable assignment
int check_assignment(ASTNode *node, SymbolTable *table);

// Check an expression for type correctness
int check_expression(ASTNode *node, SymbolTable *table);

// Check a block of statements, handling scope
int check_block(ASTNode *node, SymbolTable *table);

// Check a condition (e.g., in if statements)
int check_condition(ASTNode *node, SymbolTable *table);

// Check program node
int check_program(ASTNode *node, SymbolTable *table);

// Check statement node
int check_statement(ASTNode *node, SymbolTable *table);

typedef enum {
    SEM_ERROR_NONE,
    SEM_ERROR_UNDECLARED_VARIABLE,
    SEM_ERROR_REDECLARED_VARIABLE,
    SEM_ERROR_TYPE_MISMATCH,
    SEM_ERROR_UNINITIALIZED_VARIABLE,
    SEM_ERROR_INVALID_OPERATION,
    SEM_ERROR_SEMANTIC_ERROR // Generic semantic error
} SemanticErrorType;

// Report semantic errors
void semantic_error(SemanticErrorType error, const char *name, int line);

#endif /* SEMANTIC_H */
