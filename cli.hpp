#pragma once

#include <ncurses.h>
#include <vector>

#include "board.hpp"
#include "stats.hpp"

namespace Sko {
    class Cli {
    private:
        enum class Style {
            NONE = 0,
            WHITE = COLOR_PAIR(1),
            YELLOW = COLOR_PAIR(2),
            GREEN = COLOR_PAIR(3),
            RED = COLOR_PAIR(4),
            OPTION = WA_BOLD | WA_UNDERLINE,
            BOLD = WA_BOLD,
            INVERT = WA_STANDOUT,
            BOLD_YELLOW = WA_BOLD | COLOR_PAIR(2)
        };
        WINDOW * win;
        int
            ch,
            win_c,
            win_r,
            state,
            px,
            py;
        bool
            clearStatusBar,
            hint;
        GameStats stats;
        Board board;
        std::vector<Style> styles;
    public:
        Cli();
        ~Cli();
        void clearSubmenu();
        bool clearUserAnswer();
        void clearRow(int y);
        void drawBoard();
        void drawMenu();
        void drawStats();
        template<typename... Args> void drawStatusBar(const char * fmt, Args... vargs);
        void drawSubmenu();
        int style_push(Style s);
        Style style_pop();
        int run();
        bool setUserAnswer();
        int writeToFile();
        void loadSave(const char * filename);
    };
}