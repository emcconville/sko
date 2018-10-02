#pragma once
#include <vector>
#include <functional>

namespace Sko {
    class Cell {
    public:
        enum class UserState { System, UserBlank, UserSet };
        UserState userState;
        std::vector<int> choices;
        int
            assigned,
            column,
            region,
            row,
            user;
        Cell(const int _row=0,
             const int _column=0);
        int erase(int num);
        void shuffle();
        int pick(std::function<int(int)> randomizer);
        void clear();
        void clearUser();
        bool makeUser();
        friend bool operator< (const Cell & left, const Cell & right);
    };
}