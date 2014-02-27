#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "position.h"
#include <cfloat>

class Word;

class Move
{
public:
    Move(Position start, Position end, double time);

    Position _start;
    Position _end;

    double _time;

    double speed();

    bool changesZ();
    bool isZOnly();

    // Returns the command word (with params) that will make this move assuming
    // the current position is the start position
    virtual Word* getCommand(double feed=DBL_MAX, bool asAbsolute=true) = 0;
};

#endif // MOVEMENT_H
