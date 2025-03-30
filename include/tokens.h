/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

typedef enum
{
    TOKEN_NUMBER,           // e.g., "123", "3.14" (if you add floating point support)
    TOKEN_IDENTIFIER,       // e.g., variable names, function names
    TOKEN_STRING_LITERAL,   // e.g., "Hello World"
    TOKEN_OPERATOR,         // e.g., "+", "-", "*", "/"
    TOKEN_COMPARISON,       // e.g., "<", ">", "==", "<=", ">=", "!=", "&&", "||"
    TOKEN_EQUALS,           // =
    TOKEN_SEMICOLON,        // ;
    TOKEN_LPAREN,           // (
    TOKEN_RPAREN,           // )
    TOKEN_LBRACE,           // {
    TOKEN_RBRACE,           // }
    TOKEN_LBRACK,           // [
    TOKEN_RBRACK,           // ]
    TOKEN_IF,               // if 
    TOKEN_ELSE,             // else
    TOKEN_REPEAT,           // repeat
    TOKEN_UNTIL,            // until
    TOKEN_FOR,              // for
    TOKEN_WHILE,            // while
    TOKEN_BREAK,            // break 
    TOKEN_PRINT,            // print statement keyword
    TOKEN_FACTORIAL,        // print statement keyword
    TOKEN_RETURN,           // return
    TOKEN_VOID,             // void
    TOKEN_CONST,            // const
    TOKEN_INT,              // int 
    TOKEN_FLOAT,            // float
    TOKEN_CHAR,             // char
    TOKEN_STRING,           // string
    TOKEN_EOF,              // End Of File
    TOKEN_ERROR             // Generic error token
} TokenType;

typedef enum
{
    ERROR_NONE,
    ERROR_INVALID_CHAR,
    ERROR_INVALID_NUMBER,
    ERROR_CONSECUTIVE_OPERATORS,
    ERROR_INVALID_IDENTIFIER,
    ERROR_UNEXPECTED_TOKEN,
    ERROR_UNTERMINATED_STRING,     // e.g., "Hello
    ERROR_UNKNOWN_ESCAPE_SEQUENCE  // e.g., "\q"
} ErrorType;

typedef struct
{
    TokenType type;
    char lexeme[100]; // Actual text of the token
    int line;         // Line number in source file
    ErrorType error;  // Error type if any
} Token;

#endif /* TOKENS_H */