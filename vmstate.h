#ifndef VMSTATE_H
#define VMSTATE_H

#include "position.h"

class VMState
{
public:
    VMState();

    double _seconds;

    Position _absMM;

    double _feedRate;
    double _spindleSpeed;
    int _nextTool;
    int _currentTool;
    bool _spindleOn;
    bool _coolantOn;

    // skip overrides ...
    // skip active plane...

    bool _mmUnits; // really just length units, ignoring compensation

    // skip coordinate system selection - can I do this??
    // skip path control

    // distance mode? G90, G91 (relative vs. absolute)
    bool _movesAreRelative;

    // skip retract mode
    // skip reference location and coordinate system change

    // axis offsets (ignoring .1, .2, & G94)
    Position _off92MM;
};

#endif // VMSTATE_H
