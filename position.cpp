#include "position.h"

#include <math.h>

Position::Position(double x, double y, double z) :
    _x(x), _y(y), _z(z)
{

}

//Position::Position(Position& other)
//{
//    _x = other.x;
//    _y = other.y;
//    _z = other.z;
//}

double
Position::distanceFrom(Position& other)
{
    double dX = other._x - _x;
    double dY = other._y - _y;
    double dZ = other._z - _z;

    return sqrt(dX*dX + dY*dY + dZ*dZ);
}
