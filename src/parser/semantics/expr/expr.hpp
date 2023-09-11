#pragma once

#include "../../compiler.hpp"

namespace Zura{
    class Expr {
        public:
            void expression();

            void grouping();
            void number();
            void binary();
            void unary();
        private:
    };
};

inline Zura::Expr expr;