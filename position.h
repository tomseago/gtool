#ifndef POSITION_H
#define POSITION_H

class Position
{
public:
    Position(double x=0.0, double y=0.0, double z=0.0);
//    Position(Position& other);

    double _x, _y, _z;

    double distanceFrom(Position& other);

    Position operator+(const Position& other) const;
    Position& operator+=(const Position& other);
};

#endif // POSITION_H
