#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "position.h"

class Move
{
public:
    Move(Position start, Position end, double speed);

    Position _start;
    Position _end;

    double _speed;

    double time();


};

#endif // MOVEMENT_H
