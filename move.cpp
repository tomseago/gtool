#include "move.h"

Move::Move(Position start, Position end, double speed) :
    _start(start), _end(end), _speed(speed)
{
}

double
Move::time()
{
    double distance = _end.distanceFrom(_start);

    return distance / _speed;
}
