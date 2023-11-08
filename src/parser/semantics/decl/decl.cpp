#include "../../../helper/errors/parser_error.hpp"
#include "../../precedence/precedence.hpp"
#include "../../compiler.hpp"
#include "../expr/expr.hpp"

using namespace Zura;
using namespace Element;

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

    if (parserClass.match(WALRUS)) Expr::expression();
    else if (parserClass.match(COLON)) {
        switch (parserClass.parser.current.kind)
        {
            case Element::INT:
                std::cout << "INT_TYPE not yet implemented" << std::endl;
                Zura::Exit(Zura::Exit_Value::PARSER_ERROR);
                break;
            case Element::FLOAT:
                std::cout << "FLOAT_TYPE not yet implemented" << std::endl;
                Zura::Exit(Zura::Exit_Value::PARSER_ERROR);
                break;
            case STRING_TYPE:
                std::cout << "STRING_TYPE not yet implemented" << std::endl;
                Zura::Exit(Zura::Exit_Value::PARSER_ERROR);
                break;
            case TK_BOOL:
                std::cout << "BOOL_TYPE not yet implemented" << std::endl;
                Zura::Exit(Zura::Exit_Value::PARSER_ERROR);
                break;
            case ANY:
                std::cout << "ANY_TYPE not yet implemented" << std::endl;
                Zura::Exit(Zura::Exit_Value::PARSER_ERROR);
                break;
            case NIL:
                parserClass.emitByte(OP_NIL);
                parserClass.advance();
                break;
            default:
                break;
        }
    } 
    else if (parserClass.match(EQUAL))
        ParserError::errorAt(&parserClass.parser.previous, "Expect ': (type);' or ':= (value);' after variable name.");
    else
        ParserError::errorAt(&parserClass.parser.previous, "Expect ': (type);' or ':= (value);' after variable name.");
    
    parserClass.consume(SEMICOLON, "Expect ';' after variable declaration.");
    prec.defineVariable(global);
}

void ParserClass::declaration() {
    if (match(HAVE)) varDeclaration();
    else statement();

    if (parser_error) synchronize();
}
