#pragma once

enum TColor {
// FG
    FG_BLACK,
    FG_DARK_RED,
    FG_DARK_GREEN,
    FG_DARK_YELLOW,
    FG_DARK_BLUE,
    FG_DARK_MAGENTA,
    FG_DARK_CYAN,
    FG_DARK_GRAY,
    FG_LIGHT_GRAY,
    FG_BRIGHT_RED,
    FG_BRIGHT_GREEN,
    FG_BRIGHT_YELLOW,
    FG_BRIGHT_BLUE,
    FG_BRIGHT_MAGENTA,
    FG_BRIGHT_CYAN,
    FG_WHITE,
    // BG
    BG_BLACK,
    BG_DARK_RED,
    BG_DARK_GREEN,
    BG_DARK_YELLOW,
    BG_DARK_BLUE,
    BG_DARK_MAGENTA,
    BG_DARK_CYAN,
    BG_DARK_GRAY,
    BG_LIGHT_GRAY,
    BG_BRIGHT_RED,
    BG_BRIGHT_GREEN,
    BG_BRIGHT_YELLOW,
    BG_BRIGHT_BLUE,
    BG_BRIGHT_MAGENTA,
    BG_BRIGHT_CYAN,
    BG_WHITE,


    // SPECIAL
    RESET, 
    BOLD, 
    UNDERLINE,
    NO_UNDERLINE,
    REVERSE_TEXT,
    POSITIVE_TEXT
};

inline const char* set_color(TColor color) {
    switch (color) {

        // FG 
        case FG_BLACK: return "\033[0;30m";
        case FG_DARK_RED: return "\033[0;31m";
        case FG_DARK_GREEN: return "\033[0;32m";
        case FG_DARK_YELLOW: return "\033[0;33m";
        case FG_DARK_BLUE: return "\033[0;34m";
        case FG_DARK_MAGENTA: return "\033[0;35m";
        case FG_DARK_CYAN: return "\033[0;36m";
        case FG_DARK_GRAY: return "\033[0;37m";
        case FG_LIGHT_GRAY: return "\033[0;90m";
        case FG_BRIGHT_RED: return "\033[0;91m";
        case FG_BRIGHT_GREEN: return "\033[0;92m";
        case FG_BRIGHT_YELLOW: return "\033[0;93m";
        case FG_BRIGHT_BLUE: return "\033[0;94m";
        case FG_BRIGHT_MAGENTA: return "\033[0;95m";
        case FG_BRIGHT_CYAN: return "\033[0;96m";
        case FG_WHITE: return "\033[0;97m";


        // BG
        case BG_BLACK: return "\033[0;40m";
        case BG_DARK_RED: return "\033[0;41m";
        case BG_DARK_GREEN: return "\033[0;42m";
        case BG_DARK_YELLOW: return "\033[0;43m";
        case BG_DARK_BLUE: return "\033[0;44m";
        case BG_DARK_MAGENTA: return "\033[0;45m";
        case BG_DARK_CYAN: return "\033[0;46m";
        case BG_DARK_GRAY: return "\033[0;47m";
        case BG_LIGHT_GRAY: return "\033[0;100m";
        case BG_BRIGHT_RED: return "\033[0;101m";
        case BG_BRIGHT_GREEN: return "\033[0;102m";
        case BG_BRIGHT_YELLOW: return "\033[0;103m";
        case BG_BRIGHT_BLUE: return "\033[0;104m";
        case BG_BRIGHT_MAGENTA: return "\033[0;105m";
        case BG_BRIGHT_CYAN: return "\033[0;106m";
        case BG_WHITE: return "\033[0;107m";


        // SPECIAL
        case RESET: return "\033[0m";
        case BOLD: return "\033[1m";
        case UNDERLINE: return "\033[4m";
        case NO_UNDERLINE: return "\033[24m";
        case REVERSE_TEXT: return "\033[7m";
        case POSITIVE_TEXT: return "\033[27m";
        default: return "\033[0m";
    }
}
