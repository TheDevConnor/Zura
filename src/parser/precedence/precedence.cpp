#include "../semantics/expr/expr.hpp"
#include "precedence.hpp"

using namespace Zura;

void Prec::ParsePrecedence(Prec::Precedence precedence) {
    parserClass.advance();
    switch (parserClass.parser.previous.kind) {
        case LEFT_PAREN: expr.grouping(); break;
        case MINUS: expr.unary(); break;
        case NUMBER: expr.number(); break;
        default: return;
    }
}
