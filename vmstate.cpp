#include "vmstate.h"

VMState::VMState() :
    _seconds(0.0),
    _feedRate(1.0),
    _spindleSpeed(0.0),
    _nextTool(0),
    _currentTool(0),
    _spindleOn(false),
    _coolantOn(false),
    _mmUnits(true),
    _movesAreRelative(false)
{
}
