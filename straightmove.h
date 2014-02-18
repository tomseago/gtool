#ifndef STRAIGHTMOVE_H
#define STRAIGHTMOVE_H

#include "move.h"

class StraightMove : public Move
{
public:
    StraightMove(Position from, Position to, double speed);
};

#endif // STRAIGHTMOVE_H
