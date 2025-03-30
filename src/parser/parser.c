/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"


// TODO 1: Add more parsing function declarations for:
// - if statements: if (condition) { ... }
// - while loops: while (condition) { ... }
// - repeat-until: repeat { ... } until (condition)
// - print statements: print x;
// - blocks: { statement1; statement2; }
// - factorial function: factorial(x)

static ASTNode *parse_program(void);
static ASTNode *parse_expression(void);
static ASTNode *parse_primary(void);
static ASTNode *parse_statement(void);
static ASTNode *parse_assignment(void);
static ASTNode* parse_if_statement(void);
static ASTNode* parse_while_statement(void);
static ASTNode* parse_repeat_statement(void);
static ASTNode* parse_print_statement(void);
static ASTNode* parse_block(void);
static ASTNode* parse_factorial(void);

// Current token being processed
static Token current_token;
static int position = 0;
static const char *source;


static void parse_error(ParseError error, Token token) {
    // TODO 2: Add more error types for:
    // - Missing parentheses
    // - Missing condition
    // - Missing block braces
    // - Invalid operator
    // - Function call errors

    printf("Parse Error at line %d: ", token.line);
    switch (error) {
        case PARSE_ERROR_UNEXPECTED_TOKEN:
            printf("Unexpected token '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_SEMICOLON:
            printf("Missing semicolon after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_IDENTIFIER:
            printf("Expected identifier after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_EQUALS:
            printf("Expected '=' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_EXPRESSION:
            printf("Invalid expression after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_STATEMENT:
            printf("Invalid statement after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_LPAREN:
            printf("Expected '(' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_RPAREN:
            printf("Expected ')' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_LBRACE:
            printf("Expected '{' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_RBRACE:
            printf("Expected '}' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_LBRACK:
            printf("Expected '[' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_RBRACK:
            printf("Expected ']' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_UNTIL:
            printf("Expected 'until' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_COMPARISON:
            printf("Invalid comparison at '%s'\n", token.lexeme);
        default:
            printf("Unknown error\n");
    }
}
// Get next token
static void advance(void) {
    current_token = get_next_token(source, &position);
}

// Create a new AST node
static ASTNode *create_node(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node) {
        node->type = type;
        node->token = current_token;
        node->left = NULL;
        node->right = NULL;
        node->next = NULL;
    }
    return node;
}


// Match current token with expected type
static int match(TokenType type) {
    return current_token.type == type;
}

// Expect a token type or error
static void expect(TokenType type) {
    if (match(type)) {
        advance();
    } else {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1); // Or implement error recovery
    }
}


// Parse factorial function
static ASTNode *parse_factorial(void) {
    ASTNode *node = create_node(AST_FACTORIAL);
    advance(); // consume 'factorial'

    expect(TOKEN_LPAREN);

    ASTNode *expression = parse_expression();
    if (expression) {
        node->left = expression;
    } else {
        parse_error(PARSE_ERROR_INVALID_EXPRESSION, current_token);
        exit(1);
    }

    expect(TOKEN_RPAREN);
    expect(TOKEN_SEMICOLON);

    return node;
}


// Forward declarations
static ASTNode *parse_statement(void);

// TODO 3: Add parsing functions for each new statement type
// static ASTNode* parse_if_statement(void) { ... }
// static ASTNode* parse_while_statement(void) { ... }
// static ASTNode* parse_repeat_statement(void) { ... }
// static ASTNode* parse_print_statement(void) { ... }
// static ASTNode* parse_block(void) { ... }
// static ASTNode* parse_factorial(void) { ... }

// Parse block
static ASTNode *parse_block(void) {
    ASTNode *node = create_node(AST_BLOCK);
    ASTNode *current = node;
    advance(); // consume '{'

    while (!match(TOKEN_RBRACE)) {
        ASTNode *statement = parse_statement();
        if (statement) {
            current->next = statement;
            current = current->next;
        } else {
            parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
            exit(1);
        }
    }
    if (!match(TOKEN_RBRACE)) {
        parse_error(PARSE_ERROR_MISSING_RBRACE, current_token);
        exit(1);
    }
    advance(); // consume '}'

    return node;
}

// Parse if statement (else case not handled)
static ASTNode *parse_if_statement(void) {
    ASTNode *node = create_node(AST_IF);
    advance(); // consume 'if'

    expect(TOKEN_LPAREN);

    // node->left = create_node(AST_CONDITION);
    // node->left->left = parse_expression();;
    node->left = parse_expression();

    expect(TOKEN_RPAREN);

    if (match(TOKEN_LBRACE)) {
        node->right = parse_block();
    } else {
        ASTNode *statement = parse_statement();
        if (statement) {
            node->right = statement;
        } else {
            parse_error(PARSE_ERROR_INVALID_STATEMENT, current_token);
            exit(1);
        }
    }

    return node;
}

// Parse while statement
static ASTNode *parse_while_statement(void) {
    ASTNode *node = create_node(AST_WHILE);
    advance(); // consume 'while'

    expect(TOKEN_LPAREN);

    // node->left = create_node(AST_CONDITION);
    // node->left->left = parse_expression();
    node->left = parse_expression();

    expect(TOKEN_RPAREN);

    if (match(TOKEN_LBRACE)) {
        node->right = parse_block();
    } else {
        ASTNode *statement = parse_statement();
        if (statement) {
            node->right = statement;
        } else {
            parse_error(PARSE_ERROR_INVALID_STATEMENT, current_token);
            exit(1);
        }
    }

    return node;
}

// Parse repeat until statement
static ASTNode *parse_repeat_statement(void) {
    ASTNode *node = create_node(AST_REPEAT);
    advance(); // consume 'repeat'

    if (!match(TOKEN_LBRACE)) {
        parse_error(PARSE_ERROR_MISSING_LBRACE, current_token);
        exit(1);
    }

    node->left = parse_block();

    expect(TOKEN_UNTIL);
    expect(TOKEN_LPAREN);

    node->right = create_node(AST_CONDITION);
    node->right->left = parse_expression();

    expect(TOKEN_RPAREN);
    expect(TOKEN_SEMICOLON);

    return node;
}

// Parse print statement
static ASTNode *parse_print_statement(void) {
    // printf("parsing print\n");
    ASTNode *node = create_node(AST_PRINT);
    advance(); // consume 'print'

    // printf("b4 parse: %s\n", current_token.lexeme);
    ASTNode *expression = parse_expression();
    // printf("after parse: %s\n", current_token.lexeme);
    if (expression) {
        node->left = expression;
    } else {
        parse_error(PARSE_ERROR_INVALID_EXPRESSION, current_token);
        exit(1);
    }

    expect(TOKEN_SEMICOLON);

    return node;
}

static ASTNode *parse_expression(void);

// Parse variable declaration: int x;
static ASTNode *parse_declaration(void) {
    ASTNode *node = create_node(AST_VARDECL);
    advance(); // consume 'int'

    if (!match(TOKEN_IDENTIFIER)) {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        exit(1);
    }

    node->token = current_token;
    advance();

    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse assignment: x = 5;
static ASTNode *parse_assignment(void) {
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;
    advance();

    if (!match(TOKEN_EQUALS)) {
        parse_error(PARSE_ERROR_MISSING_EQUALS, current_token);
        exit(1);
    }
    advance();

    node->right = parse_expression();

    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

static ASTNode *parse_binop(void) {
    ASTNode *node = parse_expression(); 
    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    // Consume semicolon
    advance(); 

    return node;
}

// Parse statement
static ASTNode *parse_statement(void) {
    if (match(TOKEN_INT) || match(TOKEN_FLOAT) || match(TOKEN_CHAR))    return parse_declaration();
    else if (match(TOKEN_IDENTIFIER))   return parse_assignment();
    else if (match(TOKEN_LBRACE))   return parse_block();
    else if (match(TOKEN_IF))   return parse_if_statement();
    else if (match(TOKEN_WHILE))    return parse_while_statement();
    else if (match(TOKEN_REPEAT))   return parse_repeat_statement();
    else if (match(TOKEN_PRINT))    return parse_print_statement();
    else if (match(TOKEN_FACTORIAL))    return parse_factorial();
    else if (match(TOKEN_OPERATOR)) return parse_binop();

    // TODO 4: Add cases for new statement types
    // else if (match(TOKEN_IF)) return parse_if_statement();
    // else if (match(TOKEN_WHILE)) return parse_while_statement();
    // else if (match(TOKEN_REPEAT)) return parse_repeat_statement();
    // else if (match(TOKEN_PRINT)) return parse_print_statement();
    // ...

    printf("Syntax Error: Unexpected token\n");
    exit(1);
}

// Parse expression (currently only handles numbers and identifiers)

// TODO 5: Implement expression parsing
// Current expression parsing is basic. Need to implement:
// - Binary operations (+-*/)
// - Comparison operators (<, >, ==, etc.)
// - Operator precedence
// - Parentheses grouping
// - Function calls

static ASTNode *parse_expression(void) {
    // First, parse a primary expression (number, identifier, or parenthesized)
    ASTNode *node = parse_primary();

    // As long as the next token is an operator or comparison, consume it
    while (match(TOKEN_OPERATOR) || match(TOKEN_COMPARISON)) {
        if (match(TOKEN_COMPARISON)) {
            // 1) Create the container AST_CONDITION node
            ASTNode *condNode = create_node(AST_CONDITION);

            // 2) Create the AST_COMPARISON node for the actual op
            ASTNode *compNode = create_node(AST_COMPARISON);

            // The comparison token (==, !=, <, etc.) is stored here
            compNode->token = current_token;

            // The left side of the comparison is what we’ve parsed so far
            compNode->left = node;

            // Consume the operator (==, <, etc.)
            advance();

            // The right side is another primary expression
            compNode->right = parse_primary();

            // Attach the AST_COMPARISON node as a child of the AST_CONDITION
            condNode->left = compNode;

            // Now 'condNode' becomes the top node so far
            node = condNode;
        }
        else {
            // Normal binary operator (+, -, *, /, etc.)
            ASTNode *binopNode = create_node(AST_BINOP);
            binopNode->token = current_token;

            // Left child is what we’ve parsed so far
            binopNode->left = node;
            advance();  // consume the operator

            // Right child is another “primary”
            binopNode->right = parse_primary();

            // binopNode becomes the top node
            node = binopNode;
        }
    }

    return node;
}

static ASTNode *parse_primary(void) {
    // If we see '(', parse a subexpression until ')'
    if (match(TOKEN_LPAREN)) {
        // Consume '('
        advance();

        // Recursively parse whatever is inside the parentheses
        ASTNode *sub_expr = parse_expression();

        // Expect the closing ')'
        if (!match(TOKEN_RPAREN)) {
            parse_error(PARSE_ERROR_MISSING_RPAREN, current_token);
            exit(1);
        }
        // Consume ')'
        advance();

        return sub_expr;
    }
    // If it’s a number
    else if (match(TOKEN_NUMBER)) {
        ASTNode *node = create_node(AST_NUMBER);
        advance(); // consume the number token
        return node;
    }
    // If it’s an identifier
    else if (match(TOKEN_IDENTIFIER)) {
        ASTNode *node = create_node(AST_IDENTIFIER);
        advance(); // consume the identifier token
        return node;
    }
    // If none of the above, it’s an invalid expression
    else {
        parse_error(PARSE_ERROR_INVALID_EXPRESSION, current_token);
        exit(1);
    }
}

// Parse program (multiple statements)
static ASTNode *parse_program(void) {
    ASTNode *program = create_node(AST_PROGRAM);
    ASTNode *current = program;

    while (!match(TOKEN_EOF)) {
        current->next = parse_statement();
        current = current->next;
        // if (!match(TOKEN_EOF)) {
        //     current->right = create_node(AST_PROGRAM);
        //     current = current->right;
        // }
    }

    return program;
}

// Initialize parser
void parser_init(const char *input) {
    source = input;
    position = 0;
    advance(); // Get first token
}

// Main parse function
ASTNode *parse(void) {
    return parse_program();
}

// Print AST (for debugging)
void print_ast(ASTNode *node, int level) {
    if (!node) return;

    // Indent based on level
    for (int i = 0; i < level; i++) printf("--");

    // Print node info
    const char *lexeme = (node->token.lexeme ? node->token.lexeme : "(null)");
    
    switch (node->type) {
        case AST_PROGRAM:       printf("Program\n"); break;
        case AST_VARDECL:       printf("VarDecl: %s\n", lexeme); break;
        case AST_ASSIGN:        printf("Assign\n"); break;
        case AST_NUMBER:        printf("Number: %s\n", lexeme); break;
        case AST_IDENTIFIER:    printf("Identifier: %s\n", lexeme); break;
        case AST_CONDITION:     printf("Condition\n"); break;
        case AST_IF:            printf("If\n"); break;
        case AST_WHILE:         printf("While\n"); break;
        case AST_REPEAT:        printf("Repeat-Until\n"); break;
        case AST_BLOCK:         printf("Block\n"); break;
        case AST_BINOP:         printf("BinaryOp: %s\n", lexeme); break;
        case AST_PRINT:         printf("Print\n"); break;
        case AST_FACTORIAL:     printf("Factorial\n"); break;
        case AST_COMPARISON:    printf("Comparison: %s\n", lexeme); break;
        // TODO 6: Add cases for new node types
        // case AST_IF: printf("If\n"); break;
        // case AST_WHILE: printf("While\n"); break;
        // case AST_REPEAT: printf("Repeat-Until\n"); break;
        // case AST_BLOCK: printf("Block\n"); break;
        // case AST_BINOP: printf("BinaryOp: %s\n", node->token.lexeme); break;
        default:
            printf("Unknown node type\n");
    }

        // Print next node
        switch (node->type) {
            case AST_PROGRAM:
            case AST_BLOCK:
                print_ast(node->next, level + 1);
                break;
            case AST_VARDECL:
            case AST_ASSIGN:
            case AST_NUMBER:
            case AST_IDENTIFIER:
            case AST_CONDITION:
            case AST_COMPARISON:
            case AST_IF:
            case AST_WHILE:
            case AST_REPEAT:
            case AST_BINOP:
            case AST_PRINT:
            case AST_FACTORIAL:
                // Print children
                print_ast(node->left, level + 1);
                print_ast(node->right, level + 1);
                if (node->next) print_ast(node->next, level);
                break;
            // TODO 6: Add cases for new node types
            // case AST_IF: printf("If\n"); break;
            // case AST_WHILE: printf("While\n"); break;
            // case AST_REPEAT: printf("Repeat-Until\n"); break;
            // case AST_BLOCK: printf("Block\n"); break;
            // case AST_BINOP: printf("BinaryOp: %s\n", node->token.lexeme); break;
        }
}

// Example of examining tokens
void print_token_stream(const char* input) {
    int position = 0;
    Token token;
    
    do {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);
}


// Free AST memory
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

// Main function for testing
// int main() {
//     // Test with both valid and invalid inputs
//     const char *input = "int x;\n" // Valid declaration
//                         "x = 42;\n" // Valid assignment;
//                         "if (1) {\nx = 5;\n}"  // Valid if statement
//                         "while (1) {\nx = 5;\ny = 4;\n}"
//                         "repeat {\nx = 5;\n} until (1);"
//                         "print x;\n"
//                         "y = x + 5;\n"
//                         "if (x = 1) {\nx = 5;\n}"  // Valid if statement
//                         "factorial(4);\n"
//                         "x = (3 + 7) * (10 - 4);"; // Valid assignment;
//     // TODO 8: Add more test cases and read from a file:
//     const char *invalid_input = "int x;\n"
//                                 "x = 42;\n"
//                                 "int ;";

//     printf("Parsing input:\n%s\n", input);
//     parser_init(input);
//     ASTNode *ast = parse();

//     printf("\nAbstract Syntax Tree:\n");
//     print_ast(ast, 0);

//     free_ast(ast);
//     return 0;
// }

// Main function for testing
int main() {
    // Test with both valid and invalid inputs
    const char *valid_input = "int x;\n" // Valid declaration
                        "x = 42;\n" // Valid assignment;
                        "if (1) {x = 5;\n}"  // Valid if statement
                        "while (1) {x = 5;y = 4;}\n"
                        "repeat {x = 5;} until (1);\n"
                        "print x;\n"
                        "y = x + 5;\n"
                        "if (x == 1) {x = 5;}\n"  // Valid if statement
                        "factorial(4);\n"
                        "x = (3 + 7) * (10 - 4);";

    // TODO 8: Add more test cases and read from a file:
    const char *invalid_input = "int x;\n"
                                "x = 42;\n"
                                "int ;\n"
                                "x@ + 4\n;"
                                "x +- y;\n"
                                "x = (x + 1;";

    printf("Parsing input:\n%s\n", invalid_input);
    parser_init(invalid_input);
    ASTNode *ast = parse();
    print_ast(ast, 0);
    free_ast(ast);
    return 0;
}
