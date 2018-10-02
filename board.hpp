#pragma once
#include <vector>
#include <ostream>
#include "cell.hpp"
#include "stats.hpp"
#include "file.hpp"

namespace Sko {
    class Board {
    private:
        int _seed;
    public:
        std::vector< std::vector<Cell> > cells;
        Board(unsigned seed);
        Cell * at(const int row, const int column);
        int generate();
        int setUserSpaces(int num);
        bool setUserAnswer(int y, int x, int num);
        bool clearUserAnswer(int y, int x);
        void clearBoard();
        void seed(unsigned seed);
        unsigned seed();
        int randomizer(int i);
        void stats(GameStats & stats);
        int write(const char * filename);
        void read(const char * filename);
        friend std::ostream & operator<<(std::ostream & left, const Board & right);
    };
}