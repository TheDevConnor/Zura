#pragma once

#include "../../compiler.hpp"

namespace Zura{
    class Expr {
        public:
            void ParsePrecedence(ParserClass::Precedence precedence);
            void expression();

            void grouping();
            void number();
            void binary();
            void unary();
        private:
    };
};