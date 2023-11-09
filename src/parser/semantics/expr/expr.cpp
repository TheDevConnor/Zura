#include "expr.hpp"
#include "../../../helper/errors/parser_error.hpp"
#include "../../../lexer/tokens.hpp"
#include "../../../object/object.hpp"
#include "../../../types/type.hpp"
#include "../../compiler.hpp"
#include "../../precedence/precedence.hpp"

using namespace Zura;

void Expr::string(bool canAssign)
{
    (void)canAssign;
    parserClass.emitConstant(OBJECT_VAL(copyString(parserClass.parser.previous.start + 1, parserClass.parser.previous.length - 2)));
}

void Expr::number(bool canAssign)
{
    double value = std::strtod(parserClass.parser.previous.start, nullptr);
    parserClass.emitConstant(NUMBER_VAL(value));
}

void Expr::binary(bool canAssign)
{
    (void)canAssign;
    TokenKind  operatorKind = parserClass.parser.previous.kind;
    ParseRule* rule         = prec.getRule(operatorKind);
    prec.ParsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorKind) {
    // Numeric operations
    case PLUS: {
        parserClass.emitByte(OP_ADD);
        break;
    }
    case MINUS: {
        parserClass.emitByte(OP_SUBTRACT);
        break;
    }
    case STAR: {
        parserClass.emitByte(OP_MULTIPLY);
        break;
    }
    case SLASH: {
        parserClass.emitByte(OP_DIVIDE);
        break;
    }
    case POWER: {
        parserClass.emitByte(OP_POW);
        break;
    }
    case MODULO: {
        parserClass.emitByte(OP_MOD);
        break;
    }

    // Comparison operations
    case GREATER: {
        parserClass.emitByte(OP_GREATER);
        break;
    }
    case GREATER_EQUAL: {
        parserClass.emitBytes(OP_LESS, OP_NOT);
        break;
    }
    case LESS: {
        parserClass.emitByte(OP_LESS);
        break;
    }
    case LESS_EQUAL: {
        parserClass.emitBytes(OP_GREATER, OP_NOT);
        break;
    }
    case BANG_EQUAL: {
        parserClass.emitBytes(OP_EQUAL, OP_NOT);
        break;
    }
    case EQUAL: {
        parserClass.emitByte(OP_EQUAL);
        break;
    }
    default:
        return;
    }
}

void Expr::grouping(bool canAssign)
{
    (void)canAssign;
    Expr::expression();
    parserClass.consume(RIGHT_PAREN, "Expected ')' after expression.");
}

void Expr::literal(bool canAssign)
{
    (void)canAssign;
    switch (parserClass.parser.previous.kind) {
    case TK_FALSE:
        parserClass.emitByte(OP_FALSE);
        break;
    case NIL:
        parserClass.emitByte(OP_NIL);
        break;
    case TK_TRUE:
        parserClass.emitByte(OP_TRUE);
        break;
    }
}

void Expr::unary(bool canAssign)
{
    (void)canAssign;
    TokenKind operatorKind = parserClass.parser.previous.kind;

    prec.ParsePrecedence(Precedence::UNARY);

    switch (operatorKind) {
    case BANG:
        parserClass.emitByte(OP_NOT);
        break;
    case MINUS:
        parserClass.emitByte(OP_NEGATE);
        break;
    default:
        return;
    }
}

void Expr::expression()
{
    prec.ParsePrecedence(Precedence::ASSIGNMENT);
    return;
}

void namedVariable(Token name, bool canAssign)
{
    uint8_t arg = prec.identifierConstant(&name);

    if (parserClass.match(EQUAL) || parserClass.match(COLON))
        ParserError::errorAt(&parserClass.parser.previous, "Invalid assignment target. Please use ':=' instead of '=' or ':'.");

    else if (canAssign && parserClass.match(WALRUS)) {
        Expr::expression();
        parserClass.emitBytes(OP_SET_GLOBAL, arg);
    } else {
        parserClass.emitBytes(OP_GET_GLOBAL, arg);
    }
}

void Expr::var(bool canAssign)
{
    namedVariable(parserClass.parser.previous, canAssign);
}