#pragma once

#include "../../compiler.hpp"

namespace Expr {
    void expression();

    void grouping(bool canAssign);
    void literal(bool canAssign);
    void string(bool canAssign);
    void number(bool canAssign);
    void binary(bool canAssign);
    void unary(bool canAssign);
    void var(bool canAssign);
}