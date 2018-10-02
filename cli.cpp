#include <chrono>
#include <ncurses.h>
#include <sstream>

#include "cli.hpp"
#include "board.hpp"

Sko::Cli::Cli() :
    state(0),
    px(0),
    py(0),
    hint(false),
    board(0xDEADBEEF)
{
    initscr();
    getmaxyx(stdscr,win_r,win_c);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    clear();
    noecho();
    cbreak();
    curs_set(0);
    win = newwin(win_r,win_c,0,0);
    keypad(win, TRUE);

    drawMenu();
    drawBoard();
}
void Sko::Cli::drawMenu()
{
    /* Draw header GUI.*/
    style_push(Style::YELLOW);
    mvwprintw(win, 0,0, " [sko] : ");
    style_push(Style::OPTION);
    wprintw(win, "N");
    style_pop();
    wprintw(win, "ew : ");
    if (state == 2) {
        style_push(Style::OPTION);
        wprintw(win, "W");
        style_pop();
        wprintw(win, "rite : ");
        style_push(Style::OPTION);
        wprintw(win, "H");
        style_pop();
        wprintw(win, "int : ");
        style_push(Style::OPTION);
        wprintw(win, "S");
        style_pop();
        wprintw(win, "eed : ");
    }
    style_push(Style::OPTION);
    wprintw(win, "Q");
    style_pop();
    wprintw(win, "uit");
    wprintw(win, "%*s", win_c - getcurx(win) - 3, " ");
    style_pop();
    wrefresh(win);
}
void Sko::Cli::drawSubmenu()
{
    style_push(Style::WHITE);
    drawStatusBar(" Generating new game....");
    mvwprintw(win, 1, 0, " Choose a difficulty level: ");
    style_push(Style::OPTION);
    wprintw(win, "S");
    style_pop();
    wprintw(win, "uper Easy : ");
    style_push(Style::OPTION);
    wprintw(win, "E");
    style_pop();
    wprintw(win, "asy : ");
    style_push(Style::OPTION);
    wprintw(win, "M");
    style_pop();
    wprintw(win, "edium : ");
    style_push(Style::OPTION);
    wprintw(win, "A");
    style_pop();
    wprintw(win, "dvanced : ");
    style_push(Style::OPTION);
    wprintw(win, "H");
    style_pop();
    wprintw(win, "ard%*s", win_c - getcurx(win)-3, " ");
    style_pop();
}
void Sko::Cli::clearSubmenu()
{
    clearRow(1);
}
void Sko::Cli::clearRow(int y)
{
    wmove(win, y, 0);
    wclrtoeol(win);
}
void Sko::Cli::drawBoard()
{
    const char * top = ".---.---.---.---.---.---.---.---.---.";
    const char * row = ":---+---+---+---+---+---+---+---+---:";
    const char * end = "'---'---'---'---'---'---'---'---'---'";
    wmove(win, 2, 1);
    wprintw(win, top);
    style_push(Style::BOLD_YELLOW);
    mvwaddch(win, getcury(win), 13, '.');
    mvwaddch(win, getcury(win), 25, '.');
    style_pop();

    for(int r = 0; r < 9; ++r) {
        wmove(win, getcury(win)+1, 1);
        wprintw(win, "|");
        for (int c = 0; c < 9; ++c) {
            switch (state) {
                case 0 :
                case 1 :
                {
                    wprintw(win, " ? ");
                    break;
                }
                case 2 :
                case 3 :
                {
                    const Sko::Cell * cell = board.at(r, c);
                    char rh,lh;
                    if (py == r && px == c && state == 2) {
                        rh = '[';
                        lh = ']';
                    } else {
                        rh = ' ';
                        lh = ' ';
                    }
                    wprintw(win, "%c", rh);
                    if (cell->userState == Sko::Cell::UserState::UserBlank) {
                        wprintw(win, " ");
                    } else if (cell->userState == Sko::Cell::UserState::UserSet) {
                        style_push(Style::GREEN);
                        if (hint && cell->user != cell->assigned) {
                            style_push(Style::INVERT);
                            wprintw(win, "%c", 48 + cell->user);
                            style_pop();
                        } else {
                            wprintw(win, "%c", 48 + cell->user);
                        }
                        style_pop();
                    } else if (cell->userState == Sko::Cell::UserState::System) {
                        style_push(Style::RED);
                        wprintw(win, "%c", 48 + cell->assigned);
                        style_pop();
                    } else {
                        wprintw(win, "?");
                    }
                    wprintw(win, "%c", lh);
                    break;
                }
            }
            if (c == 2 || c == 5) {
                style_push(Style::BOLD_YELLOW);
                wprintw(win, "|");
                style_pop();
            } else {
                wprintw(win, "|");
            }
        }
        if (r < 8) {
            if (r == 2 || r == 5) {
                style_push(Style::BOLD_YELLOW);
                mvwprintw(win, getcury(win)+1, 1, row);
                style_pop();
            } else {
                mvwprintw(win, getcury(win)+1, 1, row);
                style_push(Style::BOLD_YELLOW);
                mvwaddch(win, getcury(win), 13, '+');
                mvwaddch(win, getcury(win), 25, '+');
                style_pop();
            }
        }
    }
    mvwprintw(win, getcury(win)+1, 1, end);
    style_push(Style::BOLD_YELLOW);
    mvwaddch(win, getcury(win), 13, '\'');
    mvwaddch(win, getcury(win), 25, '\'');
    style_pop();
    drawStats();
}
bool Sko::Cli::setUserAnswer()
{
    return board.setUserAnswer(py, px, ch-48);
}
bool Sko::Cli::clearUserAnswer()
{
    return board.clearUserAnswer(py, px);
}

void Sko::Cli::drawStats()
{
    board.stats(stats);
    mvwprintw(win, 5, 42, "Total     : %02d", stats.total);
    mvwprintw(win, 6, 42, "Remaining : %02d", stats.total - stats.set);
    mvwprintw(win, 7, 42, "Correct   : %02d", stats.correct);
    if (stats.total > 0 && stats.total == stats.correct) {
        state = 3;
        style_push(Style::GREEN);
        style_push(Style::INVERT);
        mvwprintw(win, 8, 42, "*~~~~~~~~~~~~*", stats.correct);
        mvwprintw(win, 9, 42, ":   WINNER   :", stats.correct);
        mvwprintw(win,10, 42, "*~~~~~~~~~~~~*", stats.correct);
        style_pop();
        style_pop();
    }
}

template<typename... Args> void Sko::Cli::drawStatusBar(const char * fmt, Args... vargs)
{
    clearRow(win_r - 1);
    mvwprintw(win, win_r-1, 0, fmt, vargs...);
    clearStatusBar = true;
}

int Sko::Cli::run()
{
    int running = 1;
    while(running == 1) {
        ch = wgetch(win);
        if (clearStatusBar) { clearRow(win_r - 1); }
        switch(ch)
        {
            case KEY_UP:
            {
                if (--py < 0) py = 0;
                break;
            }
            case KEY_DOWN:
            {
                if (++py > 8) py = 8;
                break;
            }
            case KEY_LEFT:
            {
                if (--px < 0) px = 0;
                break;
            }
            case KEY_RIGHT:
            {
                if (++px > 8) px = 8;
                break;
            }
            case KEY_DC:
            case KEY_CLEAR:
            case KEY_BACKSPACE:
            {
                if (state == 2) {
                    clearUserAnswer();
                }
                break;
            }
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                if (state == 2) {
                    setUserAnswer();
                }
                break;
            }
            case 'a':
            case 'A':
            {
                if (state == 1) {
                    (void)board.setUserSpaces(60);
                    state = 2;
                    clearSubmenu();
                    drawMenu();
                }
                break;
            }
            case 'e':
            case 'E':
            {
                if (state == 1) {
                    (void)board.setUserSpaces(40);
                    state = 2;
                    clearSubmenu();
                    drawMenu();
                }
                break;
            }
            case 'h':
            case 'H':
            {
                if (state == 1) {
                    (void)board.setUserSpaces(70);
                    state = 2;
                    clearSubmenu();
                    drawMenu();
                } else if (state == 2) {
                    hint = !hint;
                    drawStatusBar(hint ? " Hints on" : " Hints off");
                }
                break;
            }
            case 'm':
            case 'M':
            {
                if (state == 1) {
                    (void)board.setUserSpaces(50);
                    state = 2;
                    clearSubmenu();
                    drawMenu();
                }
                break;
            }
            case 'n':
            case 'N':
            {
                // Please wait
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                board.seed(seed);
                state = board.generate();
                if (state == 1) {
                    drawSubmenu();
                } else {
                    drawStatusBar(" Failed to generate. Try again.");
                }
                break;
            }
            case 'q':
            case 'Q':
            case 27:
            case ERR:
            {
                drawStatusBar(" Quitting...");
                running = 0;
            }
            case 's':
            case 'S':
            {
                if (state == 1) {
                    (void)board.setUserSpaces(30);
                    state = 2;
                    clearSubmenu();
                    drawMenu();
                } else if (state == 2) {
                    drawStatusBar(" Seed : %lu", board.seed());
                }
                break;
            }
            case 'w':
            case 'W':
            {
                if (state == 2) {
                    writeToFile();
                }
            }
        }
        if (state == 2) {
            drawBoard();
        }
    }
    return running;
}
int Sko::Cli::writeToFile()
{
    std::ostringstream filename;
    filename << board.seed() << ".sko";
    int written = board.write(filename.str().c_str());
    if (written > 0) {
        drawStatusBar(" Wrote to %s", filename.str().c_str());
    }
    return written;
}
void Sko::Cli::loadSave(const char * filename)
{
    board.read(filename);
    state = 2;
    drawBoard();
    drawStats();
    drawStatusBar(" Game %s loaded", filename);
}
int Sko::Cli::style_push(Sko::Cli::Style s)
{
    wattron(win, s);
    styles.push_back(s);
    return styles.size();
}
Sko::Cli::Style Sko::Cli::style_pop()
{
    Style s = Style::NONE;
    if (styles.size() > 0) {
        s = styles.back();
        wattroff(win, s);
        styles.pop_back();
    }
    return s;
}
Sko::Cli::~Cli(){
    clrtoeol();
    refresh();
    endwin();
}