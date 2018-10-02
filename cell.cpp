#include <algorithm>
#include <functional>
#include "cell.hpp"
Sko::Cell::Cell(const int _row,
                const int _column)
:
    choices(),
    column(_column),
    row(_row)
{
    clear();
    int
        rc,
        rr;
    rc = _column / 3;
    rr = _row / 3;
    if (rc == 0 && rr == 0) {
        region = 0;
    } else if (rc == 1 && rr == 0) {
        region = 1;
    } else if (rc == 2 && rr == 0) {
        region = 2;
    } else if (rc == 0 && rr == 1) {
        region = 3;
    } else if (rc == 1 && rr == 1) {
        region = 4;
    } else if (rc == 2 && rr == 1) {
        region = 5;
    } else if (rc == 0 && rr == 2) {
        region = 6;
    } else if (rc == 1 && rr == 2) {
        region = 7;
    } else if (rc == 2 && rr == 2) {
        region = 8;
    } else {
        region = -1;
    }
    userState = UserState::System;
}
void Sko::Cell::shuffle()
{
    std::random_shuffle(choices.begin(), choices.end());
}
void Sko::Cell::clear()
{
    choices.clear();
    for(int i = 1; i < 10; ++i) { choices.push_back(i); }
    user = 0;
    assigned = 0;
}
int Sko::Cell::pick(std::function<int(int)> randomizer)
{
    if (choices.size() == 0) {
        assigned = -1;
    } else {
        if (choices.size() > 1) {
            //shuffle();
            std::random_shuffle(choices.begin(), choices.end(), randomizer);
        }
        assigned = choices.back();
        choices.pop_back();
    }
    return assigned;
}
int Sko::Cell::erase(int num)
{
    std::vector<int>::iterator it;
    for(it = choices.begin(); it != choices.end(); ++it) {
        if (*it == num) {
            choices.erase(it);
            break;
        }
    }
    return choices.size();
}

void Sko::Cell::clearUser()
{
    user = 0;
    userState = UserState::System;
}

bool Sko::Cell::makeUser()
{
    if (userState == UserState::System) {
        userState = UserState::UserBlank;
        return true;
    } else {
        // Was already set.
        return false;
    }
}

bool Sko::operator< (const Sko::Cell & left, const Sko::Cell & right)
{
    return left.assigned < right.assigned;
}