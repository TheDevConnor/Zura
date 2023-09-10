#include <iostream>
#include <cmath>

#include "../debug/debug.hpp"
#include "../common.hpp"
#include "vm.hpp"

static constexpr auto READ_BYTE = []() -> uint8_t { return *vm.ip++; };
static constexpr auto READ_CONSTANT = []() -> Value { 
    return vm.chunk->constants.values[READ_BYTE()]; 
};

static void BINARY_OP(double (*op)(double, double)) {
    double b = pop();
    double a = pop();
    push(op(a, b));
}

using OpCodeHandler = void (*)();

static void opConstant() {
    Value constant = READ_CONSTANT();
    push(constant);
}

static void opAdd() {
    BINARY_OP([](double a, double b) { return a + b; });
}

static void opSubtract() {
    BINARY_OP([](double a, double b) { return a - b; });
}

static void opMultiply() {
    BINARY_OP([](double a, double b) { return a * b; });
}

static void opDivide() {
    BINARY_OP([](double a, double b) { return a / b; });
}

static void opPow() {
    BINARY_OP([](double a, double b) { return pow(a, b); });
}

static void opMod() {
    BINARY_OP([](double a, double b) { return fmod(a, b); });
}

static void opNegate() {
    push(-pop());
}

static void opReturn() {
    printValue(pop());
    std::cout << std::endl;
    exit(OK);
}

static OpCodeHandler opCodeHandlers[] = {
    &opConstant,    // OP_CONSTANT
    &opAdd,         // OP_ADD
    &opSubtract,    // OP_SUBTRACT
    &opMultiply,    // OP_MULTIPLY
    &opDivide,      // OP_DIVIDE
    &opPow,         // OP_POW
    &opMod,         // OP_MOD
    &opNegate,      // OP_NEGATE
    &opReturn,      // OP_RETURN
};