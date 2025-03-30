/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../../include/tokens.h"
#include "../../include/lexer.h"
#include "../../include/parser.h"

static int current_line = 1;
static char last_token_type = 'x';

// Keywords table
static struct {
    const char *word;
    TokenType type;
} 

keywords[] = {
    {"if", TOKEN_IF},
    {"int", TOKEN_INT},
    {"print", TOKEN_PRINT},
    {"else", TOKEN_ELSE},
    {"repeat", TOKEN_REPEAT},
    {"until", TOKEN_UNTIL},
    {"for", TOKEN_FOR},
    {"while", TOKEN_WHILE},
    {"break", TOKEN_BREAK},
    {"factorial", TOKEN_FACTORIAL},
    {"return", TOKEN_RETURN},
    {"void", TOKEN_VOID},
    {"float", TOKEN_FLOAT},
    {"char", TOKEN_CHAR},
    {"const", TOKEN_CONST},
    {"string", TOKEN_STRING}
};

static int is_keyword(const char *word)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (strcmp(word, keywords[i].word) == 0)
        {
            return keywords[i].type;
        }
    }
    return 0;
}

void print_error(ErrorType error, int line, const char *lexeme)
{
    printf("Lexical Error at line %d: ", line);
    switch (error)
    {
    case ERROR_INVALID_CHAR:
        printf("Invalid character '%s'\n", lexeme);
        break;
    case ERROR_INVALID_NUMBER:
        printf("Invalid number format\n");
        break;
    case ERROR_CONSECUTIVE_OPERATORS:
        printf("Consecutive operators not allowed\n");
        break;
    case ERROR_INVALID_IDENTIFIER:
        printf("Invalid identifier\n");
        break;
    case ERROR_UNEXPECTED_TOKEN:
        printf("Unexpected token '%s'\n", lexeme);
        break;
    case ERROR_UNKNOWN_ESCAPE_SEQUENCE:
        printf("Unknown escape sequence\n");
        break;
    case ERROR_UNTERMINATED_STRING:
        printf("Unterminated string\n");
    default:
        printf("Unknown error\n");
    }
}

void print_token(Token token)
{
    if (token.error != ERROR_NONE)
    {
        print_error(token.error, token.line, token.lexeme);
        return;
    }

    printf("Token: ");
    switch (token.type)
    {
    case TOKEN_NUMBER:      printf("NUMBER"); break;
        case TOKEN_OPERATOR:    printf("OPERATOR"); break;
        case TOKEN_COMPARISON:  printf("COMPARISON"); break;
        case TOKEN_IDENTIFIER:  printf("IDENTIFIER"); break;
        case TOKEN_EQUALS:      printf("EQUALS"); break;
        case TOKEN_SEMICOLON:   printf("SEMICOLON"); break;
        case TOKEN_LPAREN:      printf("LPAREN"); break;
        case TOKEN_RPAREN:      printf("RPAREN"); break;
        case TOKEN_LBRACE:      printf("LBRACE"); break;
        case TOKEN_RBRACE:      printf("RBRACE"); break;
        case TOKEN_LBRACK:      printf("LBRACK"); break;
        case TOKEN_RBRACK:      printf("RBRACK"); break;
        case TOKEN_IF:          printf("IF"); break;
        case TOKEN_ELSE:        printf("ELSE"); break;
        case TOKEN_REPEAT:      printf("REPEAT"); break;
        case TOKEN_UNTIL:       printf("UNTIL"); break;
        case TOKEN_FOR:         printf("FOR"); break;
        case TOKEN_BREAK:       printf("BREAK"); break;
        case TOKEN_VOID:        printf("VOID"); break;
        case TOKEN_CONST:       printf("CONST"); break;
        case TOKEN_INT:         printf("INT"); break;
        case TOKEN_FLOAT:       printf("FLOAT"); break;
        case TOKEN_CHAR:        printf("CHAR"); break;
        case TOKEN_PRINT:       printf("PRINT"); break;
        case TOKEN_EOF:         printf("EOF"); break;
        default:                printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n", token.lexeme, token.line);
}
int is_operator_char(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
            return TOKEN_OPERATOR;
        case '<':
        case '>':
        case '!':
            return TOKEN_COMPARISON;
        case '=':
            return TOKEN_EQUALS;
        default:
            return 0;
    }
}

int is_delimiter(char c) {
    switch (c) {
        case ';':   return TOKEN_SEMICOLON;
        case '(':   return TOKEN_LPAREN;
        case ')':   return TOKEN_RPAREN;
        case '{':   return TOKEN_LBRACE;
        case '}':   return TOKEN_RBRACE;
        case '[':   return TOKEN_LBRACK;
        case ']':   return TOKEN_RBRACK;
        default:    return 0;
            
    }
}

Token get_next_token(const char *input, int *pos)
{
    Token token = {TOKEN_ERROR, "", current_line, ERROR_NONE};
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t'))
    {
        if (c == '\n')
        {
            current_line++;
        }
        (*pos)++;
    }

    if (input[*pos] == '\0')
    {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = input[*pos];

    // Handle numbers
    if (isdigit(c))
    {
        int i = 0;
        do
        {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // Handle identifiers and keywords
    if (isalpha(c) || c == '_')
    {
        int i = 0;
        do
        {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';

        // Check if it's a keyword
        TokenType keyword_type = is_keyword(token.lexeme);
        if (keyword_type)
        {
            token.type = keyword_type;
        }
        else
        {
            token.type = TOKEN_IDENTIFIER;
        }
        return token;
    }

    //String handeling
    if (c == '"') {
        int i = 0;

        (*pos)++; // skip opening quote
        c = input[*pos];
        
        while (c != '\0' && c != '"') {
            // handle escape sequences like \n, \t, etc
            if (c == '\\') {
                (*pos)++;
                c = input[*pos];
                switch (c) {
                    case 'n':
                        token.lexeme[i++] = '\n';
                        break;
                    case 't':
                        token.lexeme[i++] = '\t';
                        break;
                    case '\\':
                        token.lexeme[i++] = '\\';
                        break;
                    case '"':
                        token.lexeme[i++] = '"';
                        break;
                    default:
                        token.error = ERROR_UNKNOWN_ESCAPE_SEQUENCE;
                        token.lexeme[i++] = c;
                }
            } else {
                token.lexeme[i++] = c;
            }
            (*pos)++;
            c = input[*pos];
            if (i >= (int)sizeof(token.lexeme) - 1) break; // Avoid overflow
        }
        if (c == '"') {
            // Found closing quote
            (*pos)++; // skip closing quote
            token.lexeme[i] = '\0';
            token.type = TOKEN_STRING_LITERAL;
        } else {
            // Unterminated string
            token.type = TOKEN_ERROR;
            token.error = ERROR_UNTERMINATED_STRING;
            token.lexeme[i] = '\0';
        }
        return token;
    }

    TokenType operator_type = is_operator_char(c);
    if (operator_type) {
        int i = 0;
        token.type = operator_type;
        token.lexeme[i++] = c;
        (*pos)++;
        char next_c = input[*pos];

        // raise error for consecutive operators
        if ((c == '+' || c == '-' || c == '*' || c == '/') && (next_c == '+' || next_c == '-' || next_c == '*' || next_c == '/')) {
            token.type = TOKEN_ERROR;
            token.error = ERROR_CONSECUTIVE_OPERATORS;
            return token;
        }
        /* Check for double-character operators (==, !=, etc.) */
        if (
            (c == '=' && next_c == '=') ||
            (c == '!' && next_c == '=') ||
            (c == '|' && next_c == '|') ||
            (c == '<' && next_c == '=') ||
            (c == '>' && next_c == '=') ) {
            token.lexeme[i++] = next_c;
            (*pos)++;
        }

        if (c == '=' && next_c == '=') {
            token.type = TOKEN_COMPARISON; // or else is token_equals by default
        }

        token.lexeme[i] = '\0';
        return token;
    }
    // TODO: Add delimiter handling here
    TokenType delimeter_type = is_delimiter(c);
    if (delimeter_type) {
        token.type = delimeter_type;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;
        return token;
    }
    

    // Handle invalid characters
    token.type = TOKEN_ERROR;
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    (*pos)++;
    return token;

    return token;
}