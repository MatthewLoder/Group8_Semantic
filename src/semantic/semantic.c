#include "../../include/semantic.h"
#include "../../include/tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEMANTIC_INPUT_FILE "test/input_semantic_error.txt"

// Initialize symbol table
SymbolTable *init_symbol_table() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table) {
        table->head = NULL;
        table->current_scope = 0;
    }
    return table;
}

// Add symbol to table
void add_symbol(SymbolTable *table, const char *name, VarType type, int line) {
    Symbol *symbol = malloc(sizeof(Symbol));
    if (symbol) {
        strcpy(symbol->name, name);
        symbol->type = type;
        symbol->scope_level = table->current_scope;
        symbol->line_declared = line;
        symbol->is_initialized = 0;

        // Add to beginning of list
        symbol->next = table->head;
        table->head = symbol;

        printf("Added symbol: %s, Type: %s, Scope: %d, Line: %d\n", name, var_type_to_string(type), table->current_scope, line);
    }
}

// Look up symbol by name
Symbol *lookup_symbol(SymbolTable *table, const char *name) {
    Symbol *current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Look up symbol in current scope only
Symbol *lookup_symbol_current_scope(SymbolTable *table, const char *name) {
    Symbol *current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0 &&
            current->scope_level == table->current_scope) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Analyze AST semantically
int analyze_semantics(ASTNode *ast) {
    SymbolTable *table = init_symbol_table();
    int result = check_program(ast, table);
    free_symbol_table(table);
    return result;
}

// Check program node
int check_program(ASTNode *node, SymbolTable *table) {
    if (!node)
        return 1;

    int result = 1;

    if (node->type == AST_PROGRAM) {
        result = check_statement(node->next, table) && result;
    }

    return result;
}

// Check statement node
int check_statement(ASTNode *node, SymbolTable *table) {
    if (!node)
        return 1;

    int result = 1;

    switch (node->type) {
    case AST_VARDECL:
        result = check_declaration(node, table) && result;
        result = check_statement(node->next, table) && result;
        break;
    case AST_ASSIGN:
        result = check_assignment(node, table) && result;
        result = check_statement(node->next, table) && result;
        break;
    case AST_IF:
        result = check_expression(node->left, table) && result;
        result = check_statement(node->right, table) && result;
        result = check_statement(node->next, table) && result;
        break;
    case AST_PRINT:
        result = check_expression(node, table) && result;
        result = check_statement(node->next, table);
        break;
    case AST_WHILE:
        result = check_expression(node->left, table) && result;
        result = check_statement(node->right, table) && result;
        result = check_statement(node->next, table) && result;
        break;
    case AST_REPEAT: // TODO: come back to this
        result = check_expression(node->left, table) && result;
        result = check_statement(node->right, table) && result;
        result = check_statement(node->next, table) && result;
        break;
    case AST_BLOCK:
        enter_scope(table);
        result = check_statement(node->next, table) && result;
        exit_scope(table);
        break;
    case AST_FACTORIAL:
        result = check_expression(node, table) && result;
        result = check_statement(node->next, table);
        break;
    default:
        semantic_error(SEM_ERROR_SEMANTIC_ERROR, "Unknown Statement", node->token.line);
        return 0;
    }

    return result;
}


// Check declaration node
int check_declaration(ASTNode *node, SymbolTable *table) {
    if (node->type != AST_VARDECL) {
        return 0;
    }

    const char *name = node->token.lexeme;

    // Check if variable already declared in current scope
    Symbol *existing = lookup_symbol_current_scope(table, name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // Add to symbol table
    add_symbol(table, name, node->var_type, node->token.line);
    return 1;
}

// Check assignment node
int check_assignment(ASTNode *node, SymbolTable *table) {
    if (node->type != AST_ASSIGN || !node->left || !node->right) {
        return 0;
    }

    const char *name = node->left->token.lexeme;

    Symbol *symbol = lookup_symbol(table, name);
    if (!symbol) {
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    //check expression
    int expr_valid = check_expression(node->right, table);
    if (!expr_valid) return 0;

    //assume number since they are allowed to interchange using implicit conversion
    VarType expr_type = TYPE_INT;

    ASTNode *expr = node->right;
    if (expr->type == AST_IDENTIFIER) {
        Symbol *sym = lookup_symbol(table, expr->token.lexeme);
        if (sym) expr_type = sym->type;
    } else if (expr->type == AST_STRING_LITERAL) {
        expr_type = TYPE_STRING;
    } else if (expr->type == AST_NUMBER) {
        expr_type = TYPE_INT;
    } else if (expr->type == AST_BINOP) {
        //handle binops
        VarType left_type = TYPE_INT, right_type = TYPE_INT;

        if (expr->left->type == AST_IDENTIFIER) {
            Symbol *sym = lookup_symbol(table, expr->left->token.lexeme);
            if (sym) left_type = sym->type;
        } else if (expr->left->type == AST_STRING_LITERAL) {
            left_type = TYPE_STRING;
        } else if (expr->left->type == AST_NUMBER) {
            left_type = TYPE_INT;
        }

        if (expr->right->type == AST_IDENTIFIER) {
            Symbol *sym = lookup_symbol(table, expr->right->token.lexeme);
            if (sym) right_type = sym->type;
        } else if (expr->right->type == AST_STRING_LITERAL) {
            right_type = TYPE_STRING;
        } else if (expr->right->type == AST_NUMBER) {
            right_type = TYPE_INT;
        }

        if (left_type == TYPE_STRING || right_type == TYPE_STRING) {
            if (left_type == TYPE_STRING && right_type == TYPE_STRING &&
                strcmp(expr->token.lexeme, "+") == 0) {
                expr_type = TYPE_STRING; //string + string
            } else {
                semantic_error(SEM_ERROR_TYPE_MISMATCH, expr->token.lexeme, expr->token.line);
                return 0;
            }
        } else {
            //number ops are allowed to interchange (int float and char)
            expr_type = TYPE_INT;
        }
    }

    //check if type is compatible with the variable type
    VarType var_type = symbol->type;

    if (var_type == TYPE_STRING) {
        if (expr_type != TYPE_STRING) {
            semantic_error(SEM_ERROR_TYPE_MISMATCH, name, node->token.line);
            return 0;
        }
    } else {
        if (expr_type == TYPE_STRING) {
            semantic_error(SEM_ERROR_TYPE_MISMATCH, name, node->token.line);
            return 0;
        }
    }

    symbol->is_initialized = 1;
    return 1;
}

void semantic_error(SemanticErrorType error, const char *name, int line) {
    printf("Semantic Error at line %d: ", line);

    switch (error) {
    case SEM_ERROR_UNDECLARED_VARIABLE:
        printf("Undeclared variable '%s'\n", name);
        break;
    case SEM_ERROR_REDECLARED_VARIABLE:
        printf("Variable '%s' already declared in this scope\n", name);
        break;
    case SEM_ERROR_TYPE_MISMATCH:
        printf("Type mismatch involving '%s'\n", name);
        break;
    case SEM_ERROR_UNINITIALIZED_VARIABLE:
        printf("Variable '%s' may be used uninitialized\n", name);
        break;
    case SEM_ERROR_INVALID_OPERATION:
        printf("Invalid operation involving '%s'\n", name);
        break;
    default:
        printf("Unknown semantic error with '%s'\n", name);
    }
}

void enter_scope(SymbolTable *table) { table->current_scope++; }

void exit_scope(SymbolTable *table) {
    remove_symbols_in_current_scope(table);
    table->current_scope--;
}

void free_symbol_table(SymbolTable *table) {
    Symbol *current = table->head;
    while (current) {
        Symbol *temp = current;
        current = current->next;
        free(temp);
    }
    free(table);
}

void remove_symbols_in_current_scope(SymbolTable *table) {
    Symbol *current = table->head;
    Symbol *prev = NULL;

    while (current) {
        if (current->scope_level == table->current_scope) {
            if (prev) {
                prev->next = current->next;
            } else {
                table->head = current->next;
            }
            free(current);
            current = (prev) ? prev->next : table->head;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

// check expression temporary for testing
int check_expression(ASTNode *node, SymbolTable *table) {
    if (!node)
        return 1;

    int result = 1;

    switch (node->type) {
        case AST_IDENTIFIER: {
            const char *name = node->token.lexeme;
            Symbol *symbol = lookup_symbol(table, name);
            if (!symbol) {
                semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
                return 0;
            }
            if (!symbol->is_initialized) {
                semantic_error(SEM_ERROR_UNINITIALIZED_VARIABLE, name, node->token.line);
                return 0;
            }
            break;
        }

        case AST_STRING_LITERAL:
        case AST_NUMBER:
            // Literals are always valid
            break;

        //factorial function
        case AST_FACTORIAL: {
            result = check_expression(node->left, table);
            break;
        }

        //binary operations and comparisions
        case AST_BINOP: {
            result = check_expression(node->left, table) && check_expression(node->right, table);
            
            //defaults to int, because any number can work with any other number
            VarType left_type = TYPE_INT;
            VarType right_type = TYPE_INT;
        
            if (node->left->type == AST_IDENTIFIER) {
                Symbol *sym = lookup_symbol(table, node->left->token.lexeme);
                if (sym) left_type = sym->type;
            } else if (node->left->type == AST_STRING_LITERAL) {
                left_type = TYPE_STRING;
            }
        
            if (node->right->type == AST_IDENTIFIER) {
                Symbol *sym = lookup_symbol(table, node->right->token.lexeme);
                if (sym) right_type = sym->type;
            } else if (node->right->type == AST_STRING_LITERAL) {
                right_type = TYPE_STRING;
            }
        
            // Handle string compatibility
            if (left_type == TYPE_STRING || right_type == TYPE_STRING) {
                if (!(left_type == TYPE_STRING && right_type == TYPE_STRING && strcmp(node->token.lexeme, "+") == 0)) {
                    semantic_error(SEM_ERROR_TYPE_MISMATCH, node->token.lexeme, node->token.line);
                    result = 0;
                }
            }
        
            break;
        }

        case AST_CONDITION:
            result = check_expression(node->left, table);
            break;

            case AST_COMPARISON: {
                result = check_expression(node->left, table) && check_expression(node->right, table);
            
                VarType left_type = TYPE_INT;
                VarType right_type = TYPE_INT;
            
                if (node->left->type == AST_IDENTIFIER) {
                    Symbol *sym = lookup_symbol(table, node->left->token.lexeme);
                    if (sym) left_type = sym->type;
                } else if (node->left->type == AST_STRING_LITERAL) {
                    left_type = TYPE_STRING;
                }
            
                if (node->right->type == AST_IDENTIFIER) {
                    Symbol *sym = lookup_symbol(table, node->right->token.lexeme);
                    if (sym) right_type = sym->type;
                } else if (node->right->type == AST_STRING_LITERAL) {
                    right_type = TYPE_STRING;
                }
            
                if ((left_type == TYPE_STRING && right_type != TYPE_STRING) ||
                    (left_type != TYPE_STRING && right_type == TYPE_STRING)) {
                    semantic_error(SEM_ERROR_TYPE_MISMATCH, node->token.lexeme, node->token.line);
                    result = 0;
                }
            
                break;
        }

        default:
            semantic_error(SEM_ERROR_INVALID_OPERATION, node->token.lexeme, node->token.line);
            return 0;
    }

    // printf("Checked expression: %s, Result: %d\n", node->token.lexeme, result);

    return result;
}

char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(length + 1);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    //remove carriagre returns, keep only \n
    char *src = buffer, *dst = buffer;
    while (*src) {
        if (*src != '\r') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';

    return buffer;
}

int main() {
    char *sem_input = read_file(SEMANTIC_INPUT_FILE);

    if (sem_input) {
        parser_init(sem_input);
        ASTNode *ast = parse();

        // print_ast(ast, 0);

        int result = analyze_semantics(ast);

        if (result) {
            printf("Semantic analysis passed.\n");
        } else {
            printf("Semantic analysis failed.\n");
        }

        free_ast(ast);
        free(sem_input);
    }

    return 0;
}
