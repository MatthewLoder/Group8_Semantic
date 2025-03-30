/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"

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

static Token current_token;
static int position = 0;
static const char *source;

static void parse_error(ParseError error, Token token) {
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

//create new AST node
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

//get next token
static void advance(void) {
    current_token = get_next_token(source, &position);
}


static int match(TokenType type) {
    return current_token.type == type;
}


static void expect(TokenType type) {
    if (match(type)) {
        advance();
    } else {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1); 
    }
}


//parse factorial function
static ASTNode *parse_factorial(void) {
    ASTNode *node = create_node(AST_FACTORIAL);
    advance();

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


//forward declarations
static ASTNode *parse_statement(void);

//parse block
static ASTNode *parse_block(void) {
    ASTNode *node = create_node(AST_BLOCK);
    ASTNode *current = node;
    advance();

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
    advance();
    return node;
}

//parse if statement 
static ASTNode *parse_if_statement(void) {
    ASTNode *node = create_node(AST_IF);
    advance();

    expect(TOKEN_LPAREN);
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

//parse while statement
static ASTNode *parse_while_statement(void) {
    ASTNode *node = create_node(AST_WHILE);
    advance(); 

    expect(TOKEN_LPAREN);
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

//Parse repeat until statement
static ASTNode *parse_repeat_statement(void) {
    ASTNode *node = create_node(AST_REPEAT);
    advance();

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

//parse print statement
static ASTNode *parse_print_statement(void) {
    ASTNode *node = create_node(AST_PRINT);
    advance();
    ASTNode *expression = parse_expression();
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

//parse variable declaration: int x;
static ASTNode *parse_declaration(void) {
    ASTNode *node = create_node(AST_VARDECL);
    advance();

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

//Parse assignment: x = 5;
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
    advance(); 

    return node;
}

//Parse statement
static ASTNode *parse_statement(void) {
    if (match(TOKEN_INT) || match(TOKEN_FLOAT) || match(TOKEN_CHAR) || match(TOKEN_STRING))    return parse_declaration();
    else if (match(TOKEN_IDENTIFIER))   return parse_assignment();
    else if (match(TOKEN_LBRACE))   return parse_block();
    else if (match(TOKEN_IF))   return parse_if_statement();
    else if (match(TOKEN_WHILE))    return parse_while_statement();
    else if (match(TOKEN_REPEAT))   return parse_repeat_statement();
    else if (match(TOKEN_PRINT))    return parse_print_statement();
    else if (match(TOKEN_FACTORIAL))    return parse_factorial();
    else if (match(TOKEN_OPERATOR)) return parse_binop();
    printf("Syntax Error: Unexpected token\n");
    exit(1);
}

//Parse expression
static ASTNode *parse_expression(void) {
    //parse primary expression
    ASTNode *node = parse_primary();

    while (match(TOKEN_OPERATOR) || match(TOKEN_COMPARISON)) {
        if (match(TOKEN_COMPARISON)) {
            ASTNode *condNode = create_node(AST_CONDITION);
            ASTNode *compNode = create_node(AST_COMPARISON);
            compNode->token = current_token;
            compNode->left = node;
            advance();
            compNode->right = parse_primary();
            condNode->left = compNode;
            node = condNode;
        }
        else {
            ASTNode *binopNode = create_node(AST_BINOP);
            binopNode->token = current_token;
            binopNode->left = node;
            advance();
            binopNode->right = parse_primary();
            node = binopNode;
        }
    }

    return node;
}

static ASTNode *parse_primary(void) {
    if (match(TOKEN_LPAREN)) {
        advance();
        ASTNode *sub_expr = parse_expression();

        if (!match(TOKEN_RPAREN)) {
            parse_error(PARSE_ERROR_MISSING_RPAREN, current_token);
            exit(1);
        }
        advance();

        return sub_expr;
    }
    else if (match(TOKEN_NUMBER)) {
        ASTNode *node = create_node(AST_NUMBER);
        advance();
        return node;
    }
    else if (match(TOKEN_STRING_LITERAL)) {
        ASTNode *node = create_node(AST_STRING_LITERAL);
        advance();
        return node;
    }
    else if (match(TOKEN_IDENTIFIER)) {
        ASTNode *node = create_node(AST_IDENTIFIER);
        advance();
        return node;
    }
    else {
        parse_error(PARSE_ERROR_INVALID_EXPRESSION, current_token);
        exit(1);
    }
}

//parse program
static ASTNode *parse_program(void) {
    ASTNode *program = create_node(AST_PROGRAM);
    ASTNode *current = program;

    while (!match(TOKEN_EOF)) {
        current->next = parse_statement();
        current = current->next;
    }

    return program;
}

//initialize parser
void parser_init(const char *input) {
    source = input;
    position = 0;
    advance();
}

//Main parse function
ASTNode *parse(void) {
    return parse_program();
}

//print AST tree
void print_ast(ASTNode *node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("--");
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
        default:
            printf("Unknown type of node\n");
    }
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
                print_ast(node->left, level + 1);
                print_ast(node->right, level + 1);
                if (node->next) print_ast(node->next, level);
                break;
        }
}

//print all the tokens, like lexer output
void print_token_stream(const char* input) {
    int position = 0;
    Token token;
    do {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);
}

//free memory for ast
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}
