#include "straightmove.h"

#include "word.h"

StraightMove::StraightMove(Position from, Position to, double time) :
    Move(from, to, time)
{

}

Word*
StraightMove::getCommand(double feed, bool asAbsolute)
{
    Word* cmd = new Word('G', 1);

    if (feed != DBL_MAX)
    {
        cmd->addParameter(Word('F', INT_MAX, feed));
    }

    if (asAbsolute)
    {
        cmd->addParameter(Word('X', INT_MAX, _end._x));
        cmd->addParameter(Word('Y', INT_MAX, _end._y));
        cmd->addParameter(Word('Z', INT_MAX, _end._z));
    }
    else
    {
        cmd->addParameter(Word('X', INT_MAX, _end._x - _start._x));
        cmd->addParameter(Word('Y', INT_MAX, _end._y - _start._y));
        cmd->addParameter(Word('Z', INT_MAX, _end._z - _start._z));
    }

    return cmd;
}
