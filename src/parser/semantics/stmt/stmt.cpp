#include <iostream>

#include "../../../opCode/chunk.hpp"
#include "../../compiler.hpp"
#include "../expr/expr.hpp"

using namespace Zura; 
using namespace Element;

void expressionStatement() {
    Expr::expression();
    parserClass.consume(SEMICOLON, "Expect ';' after value.");
    parserClass.emitByte(OP_POP);
}

void printStatement() {
    // TODO: Make the print statement work more like c++'s std::cout
    Expr::expression();
    parserClass.consume(SEMICOLON, "Expect ';' after value.");
    parserClass.emitByte(OP_PRINT);
}

void ParserClass::statement() {
    if (ParserClass::match(INFO)) printStatement();
    else expressionStatement();
}
