#include <iostream>
#include <cmath>

#include "../debug/debug.hpp"
#include "../types/type.hpp"
#include "../common.hpp"
#include "vm.hpp"

using namespace Zura;

static constexpr auto READ_BYTE = []() -> uint8_t { return *vm.ip++; };
static constexpr auto READ_CONSTANT = []() -> Types::Value { 
    return vm.chunk->constants.values[READ_BYTE()]; 
};

template <typename ValueType, typename Operator>
void BINARY_OP(Operator op) {
    do {
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {
            std::cerr << "Operands must be numbers" << std::endl;
            exit(RUNTIME_ERROR);
        }
        ValueType b = AS_NUMBER(pop());
        ValueType a = AS_NUMBER(pop());
        push(Types::NUMBER_VAL(op(a, b)));
    } while (false);
}

using OpCodeHandler = void (*)();

static void opConstant() {
    std::cout << "opConstant" << std::endl;
    Types::Value constant = READ_CONSTANT();
    push(constant);
}

static void opAdd() {
    std::cout << "opAdd" << std::endl;
    BINARY_OP<double>(std::plus<double>());
}

static void opReturn() {
    printValue(pop());
    std::cout << std::endl;
    exit(OK);
}

static OpCodeHandler opCodeHandlers[] = {
    &opConstant,    // OP_CONSTANT
    &opAdd,         // OP_ADD
    // &opSubtract,    // OP_SUBTRACT
    // &opMultiply,    // OP_MULTIPLY
    // &opDivide,      // OP_DIVIDE
    // &opPow,         // OP_POW
    // &opMod,         // OP_MOD
    // &opNegate,      // OP_NEGATE
    &opReturn,      // OP_RETURN
};