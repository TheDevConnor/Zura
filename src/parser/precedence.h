#include "lexer/tokens.h"
#include "lexer/import.h"
#include "ast.h"

enum Precedence {
    Lowest,
    Assinment,      // :=
    Equals,         // ==
    LessGreater,    // > or <
    Sum,            // +
    Product,        // *
    Prefix,         // -X or !X
    Postfix,        // X++
    Call,           // myFunction(X)
    Index           // array[index]
};

typedef std::shared_ptr<expr_node>(*Led)(Parser*, std::shared_ptr<expr_node>, Precedence);
typedef std::shared_ptr<expr_node>(*Nud)(Parser*);
typedef std::shared_ptr<stmt_node>(*Stmt)(Parser*);

extern std::unordered_map<TokenKind, Precedence> precedence;
extern std::unordered_map<TokenKind, Led> led;
extern std::unordered_map<TokenKind, Nud> nud;
extern std::unordered_map<TokenKind, Stmt> stmt;

void init_precedence();
void symbol (TokenKind, Precedence);
void literal (TokenKind);
void infix (TokenKind, Led, Precedence);
void postfix (TokenKind, Nud, Precedence);
void prefix (TokenKind, Nud, Precedence);
void statement (TokenKind, Stmt);

int get_precedence(TokenKind);