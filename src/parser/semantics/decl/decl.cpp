#include "../../../helper/errors/parser_error.hpp"
#include "../../precedence/precedence.hpp"
#include "../../compiler.hpp"
#include "../expr/expr.hpp"

using namespace Zura;

void synchronize() {
    parserClass.parser_error = false;   

    while (!parserClass.match(SEMICOLON)) {
        switch (parserClass.parser.current.kind) {
            case CLASS:
            case FN:
            case HAVE:
            case FOR:
            case IF:
            case LOOP:
            case INFO:
            case RETURN:
                return;
            default:
                break;
        }
        parserClass.advance();
    }
}

void varDeclaration() {
    uint8_t global = prec.parseVariable("Expect variable name after the 'have' keyword.");

    if (parserClass.match(WALRUS)) {
        Expr::expression();
    }
    else if (parserClass.match(EQUAL)) {
        ParserError::errorAt(&parserClass.parser.previous, "Expect ':= (value);' or '; (for a nil value)' after variable name.");
    }
    else {
        parserClass.emitByte(OP_NIL);
    }
    
    parserClass.consume(SEMICOLON, "Expect ';' after variable declaration.");
    prec.defineVariable(global);
}

void ParserClass::declaration() {
    if (match(HAVE)) varDeclaration();
    else statement();

    if (parser_error) synchronize();
}