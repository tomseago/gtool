#include "move.h"

Move::Move(Position start, Position end, double time) :
    _start(start), _end(end), _time(time)
{
}

double
Move::speed()
{
    double distance = _end.distanceFrom(_start);

    return distance / _time;
}

bool
Move::changesZ()
{
    return _end._z != _start._z;
}

bool
Move::isZOnly()
{
    return (_start._x == _end._x) && (_start._y == _end._y);
}
