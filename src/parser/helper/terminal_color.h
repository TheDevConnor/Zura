#ifndef AZURA_TERMINAL_COLOR_H
#define AZURA_TERMINAL_COLOR_H

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

const char* set_color(TColor color);

#endif // AZURA_TERMINAL_COLOR_H