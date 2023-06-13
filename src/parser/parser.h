#include "lexer/import.h"
#include "lexer/tokens.h"
#include "precedence.h"

struct source_file {
    std::string name;
    std::string source;
};

class Parser {
    private:
        std::vector<Token> tokens;
        source_file source;
        Token previous;
        size_t position;

    public:
        Parser();
        Token current();
};