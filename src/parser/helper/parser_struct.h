#include "../lexer/import.h"
#include "../lexer/tokens.h"
#include "../common.h"
#include "../parser.h"

struct Parser {
    Token current;
    Token previous;
};

enum Precedence {
    DEFAULT,
    ASSINMENT,   // :=
    ARRAY,       // []
    OR,          // or
    AND,         // and
    EQUALITY,    // == !=
    COMPARASION, // < > <= >=
    TERM,        // + -
    FACTOR,      // * /
    UNARY,       // ! -
    CALL,        // . ()
    PRIMARY,     // literals
};

typedef void (*Parse_fn)(Parser* parser, bool can_assign);

struct ParserRule {
    Parse_fn prefix;
    Parse_fn infix;
    Precedence precedence;
};

struct Local {
    Token name;
    int depth;
    bool is_captured;
};

struct upvalue {
    uint8_t index;
    bool is_local;
};

enum function_type {
    TYPE_FUNC,
    TYPE_METHOD,
    TYPE_INITIALIZER,
    TYPE_SCRIPT,
};

struct Compiler {
    Compiler* enclosing;
    // ObjFunction* function;
    function_type type;

    Local locals[UINT8_COUNT];
    int local_count;
    upvalue upvalues[UINT8_COUNT];
    int scope_depth;
};

struct ClassCompiler {
    ClassCompiler* enclosing;
    bool has_superclass;
};