#pragma once

enum TColor {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    RESET
};

inline const char* set_color(TColor color) {
    switch (color) {
        case RED: return "\033[0;31m";
        case GREEN: return "\033[0;32m";
        case YELLOW: return "\033[0;33m";
        case BLUE: return "\033[0;34m";
        case MAGENTA: return "\033[0;35m";
        case CYAN: return "\033[0;36m";
        case WHITE: return "\033[0;37m";
        case RESET: return "\033[0m";
        default: return "\033[0m";
    }
}
