#ifndef STRAIGHTMOVE_H
#define STRAIGHTMOVE_H

#include "move.h"

class StraightMove : public Move
{
public:
    StraightMove(Position from, Position to, double time);

    virtual Word* getCommand(double feed=DBL_MAX, bool asAbsolute=true);
};

#endif // STRAIGHTMOVE_H
