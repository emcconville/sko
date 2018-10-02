#include <fstream>
#include <vector>
#include <stdexcept>

#include "board.hpp"
#include "cell.hpp"
#include "stats.hpp"
#include "file.hpp"

Sko::Board::Board(unsigned s)
{
    seed(s);
    for(int y = 0; y < 9; ++y) {
        std::vector<Sko::Cell> row;
        for (int x = 0; x < 9; ++x) {
            row.push_back(Sko::Cell(y,x));
        }
        cells.push_back(row);
    }
}

void Sko::Board::seed(unsigned s)
{
    _seed = s;
    std::srand(s);
}

unsigned Sko::Board::seed()
{
    return _seed;
}

int Sko::Board::randomizer(int i)
{
    return std::rand() % i;
}

Sko::Cell * Sko::Board::at(const int row, const int column)
{
    if (row < 0 || row >= cells.size()) {
        throw std::range_error("Row outside of bounds");
    }
    if (column < 0 || column >= cells[row].size() ) {
        throw std::range_error("Column outside of bounds");
    }
    return &(cells[row][column]);
}

void Sko::Board::clearBoard()
{
    int y,x;
    for(y=0; y < 9; ++y)
        for(x=0; x < 9; ++x)
            cells[y][x].clear();
}

int Sko::Board::generate()
{
    int attempts=0, success = 0;
    while(success == 0 && attempts < 1024) {
        // Clear Board
        clearBoard();
        auto fptr = std::bind(&Sko::Board::randomizer, this, std::placeholders::_1);
        int y,x,sy,sx,i;
        Cell * c, * s;
        success = 1;
        for(y=0; y < 9; ++y) {
            for(x=0; x < 9; ++x) {
                c = at(y, x);
                if (c->assigned < 1) {
                    i = c->pick(fptr);
                    if (i == -1) {
                        attempts++;
                        success = 0;
                        break;
                    }
                    // Clear value from ROW, COLUMN, and REGION
                    for(sy=0; sy < 9; ++sy) {
                        for(sx=0; sx < 9; ++sx) {
                            s = at(sy, sx);
                            if (s->row == c->row || s->column == c->column || s->region == c->region) {
                                s->erase(c->assigned);
                            }
                        }
                    }
                }
            }
            if (success == 0) { break; }
        }
    }
    return success;
}

int Sko::Board::setUserSpaces(int num)
{
    int
        y,
        x,
        ok,
        attempt;
    Cell * c;
    for(y=0;y < 9; ++y) {
        for(x=0; x < 9; ++x) {
            c = at(y, x);
            c->clearUser();
        }
    }
    for(ok = 0, attempt=1024; ok < num && attempt > 0; --attempt) {
        c = at(std::rand() % 9, std::rand() % 9);
        if (c->makeUser()) {
            ok++;
        }
    }
    return ok;
}

bool Sko::Board::setUserAnswer(int y, int x, int num)
{
    Cell * c = at(y, x);
    bool r = c->userState != Sko::Cell::UserState::System;
    if (r) {
        c->userState = Sko::Cell::UserState::UserSet;
        c->user = num;
    }
    return r;
}

bool Sko::Board::clearUserAnswer(int y, int x)
{
    Cell * c = at(y, x);
    bool r = c->userState != Sko::Cell::UserState::System;
    if (r) {
        c->userState = Sko::Cell::UserState::UserBlank;
        c->user = 0;
    }
    return r;
}

void Sko::Board::stats(Sko::GameStats & stats)
{
    Cell * c;
    int y,x;
    // Always default
    stats.total = 0;
    stats.set = 0;
    stats.correct = 0;
    for (y=0; y < 9; ++y) {
        for (x=0; x < 9; ++x) {
            c = at(y, x);
            if (c->userState != Sko::Cell::UserState::System) {
                stats.total++;
                if (c->userState == Sko::Cell::UserState::UserSet) {
                    stats.set++;
                    if (c->user == c->assigned) {
                        stats.correct++;
                    }
                }
            }
        }
    }
}

int Sko::Board::write(const char * filename)
{
    Sko::FileHeader fh = FileHeader();
    Sko::FileRow fr[9];
    fh.version[0] = 0x01;
    fh.version[1] = 0x00;
    fh.seed = seed();
    fh.data_length = sizeof(FileRow) * 9;
    for(int row = 0; row < 9; ++row) {
        int idx = 0;
        for(int col = 0; col < 9; ++col) {
            Cell * left = at(row,col);
            Cell * right = col < 8 ? at(row, ++col) : new Cell();
            fr[row].assigned[idx] = ((left->assigned - 1) << 4) | ((right->assigned - 1) & 0xF);
            fr[row].state[idx] = ((char)left->userState << 4) | ((char)right->userState & 0xF);
            fr[row].user[idx] = ((left->user - 1) << 4) | ((right->user - 1) & 0xF);
            ++idx;
        }
    }
    std::fstream fd(filename, std::ios::out | std::ios::binary);
    fd.write((char *)&fh, sizeof(FileHeader));
    fd.write((char *)&fr, sizeof(FileRow) * 9);
    int r = fd.tellp();
    fd.close();
    return r;
}

void Sko::Board::read(const char * filename)
{
    std::fstream fd(filename, std::ios::in | std::ios::binary);
    Sko::FileHeader fh;
    Sko::FileRow fr[9];
    fd.read((char *)&fh, sizeof(FileHeader));
    /* Verfiy game file */
    bool ok;
    ok  = fh.signature[0] == 's'
       && fh.signature[1] == 'k'
       && fh.signature[2] == 'o'
       && fh.signature[3] == '\0';
    if (!ok) {
        fd.close();
        throw std::runtime_error("Invalid signature, or not a valid game file.");
    }
    ok = fh.version[0] == 0x01 && fh.version[1] == 0x00;
    if (!ok) {
        fd.close();
        throw std::runtime_error("Could not identify version number.");
    }
    ok = fh.data_length == (sizeof(Sko::FileRow) * 9);
    if (!ok) {
        fd.close();
        throw std::runtime_error("Data-size check failed.");
    }
    seed(fh.seed);
    fd.read((char *)fr, fh.data_length);
    fd.close();
    for(int row = 0; row < 9; ++row) {
        int idx = 0;
        for(int col = 0; col < 9; ++col) {
            Cell * left = at(row,col);
            Cell * right = col < 8 ? at(row, ++col) : new Cell();
            unsigned char assigned, state, user;
            assigned = fr[row].assigned[idx];
            state = fr[row].state[idx];
            user = fr[row].user[idx];
            left->assigned = ((assigned >> 4) + 1) & 0xF;
            right->assigned = (assigned + 1) & 0xF;
            left->userState = (Sko::Cell::UserState)(state >> 4);
            right->userState = (Sko::Cell::UserState)(state & 0xF);
            left->user = ((user >> 4) + 1) & 0xF;
            right->user = (user + 1) & 0xF;
            ++idx;
        }
    }
}

std::ostream & Sko::operator<<(std::ostream & left, const Sko::Board &right)
{
    left << ".---.---.---.---.---.---.---.---.---." << std::endl;
    Sko::Cell c;
    for (int y = 0; y < 9; ++y) {
        left << "|";
        for (int x = 0; x < 9; ++x) {
            c = right.cells[y][x];
            if (c.userState == Sko::Cell::UserState::System) {
                left << " " << c.assigned << " ";
            } else {
                left << "   ";
            }
            if ( x < 8 ) left << "|";
        }
        left << "|" << std::endl;
        if ( y < 8 ) {
            left << ":---+---+---+---+---+---+---+---+---:" << std::endl;
        }
    }
    left << "'---'---'---'---'---'---'---'---'---'" << std::endl;
    return left;
}
