#include "../../precedence/precedence.hpp"
#include "../../../lexer/tokens.hpp"
#include "../../compiler.hpp"
#include "expr.hpp"

using namespace Zura;

void Expr::number() {
    double value = std::strtod(parserClass.parser.previous.start, nullptr);
}

void Expr::binary() {
    TokenKind operatorKind = parserClass.parser.previous.kind;
    ParseRule* rule = prec.getRule(operatorKind);
    prec.ParsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorKind) {
        case PLUS: parserClass.emitByte(OP_ADD); break;
        case MINUS: parserClass.emitByte(OP_SUBTRACT); break;
        case STAR: parserClass.emitByte(OP_MULTIPLY); break;
        case SLASH: parserClass.emitByte(OP_DIVIDE); break;
        case POWER: parserClass.emitByte(OP_POW); break;
        case MODULO: parserClass.emitByte(OP_MOD); break;
        default: return;
    }
}

void Expr::grouping() {
    Expr::expression();
    parserClass.consume(RIGHT_PAREN, "Expected ')' after expression.");
}

void Expr::unary() {
    TokenKind operatorKind = parserClass.parser.previous.kind;

    prec.ParsePrecedence(Precedence::UNARY);

    switch (operatorKind) {
        case MINUS: parserClass.emitByte(OP_NEGATE); break;
        default: return;
    }
}

void Expr::expression() {
    prec.ParsePrecedence(Precedence::ASSIGNMENT);
    return;
}