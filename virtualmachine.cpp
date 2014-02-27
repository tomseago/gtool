#include "virtualmachine.h"

#include "vmstate.h"
#include "codeblock.h"
#include "word.h"
#include "straightmove.h"

#include <QDebug>

VirtualMachine::VirtualMachine(double rapidRate) :
    _rapidRate(rapidRate)
{
    commands[QString("G93")] = &VirtualMachine::g93;
    commands[QString("G94")] = &VirtualMachine::g94;

    commands[QString("M6")] = &VirtualMachine::m6;
    commands[QString("M61")] = &VirtualMachine::m61;

    commands[QString("M3")] = &VirtualMachine::m3;
    commands[QString("M4")] = &VirtualMachine::m4;
    commands[QString("M5")] = &VirtualMachine::m5;

    commands[QString("M7")] = &VirtualMachine::m7;
    commands[QString("M8")] = &VirtualMachine::m8;
    commands[QString("M9")] = &VirtualMachine::m9;

    commands[QString("G4")] = &VirtualMachine::g4;

    commands[QString("G20")] = &VirtualMachine::g20;
    commands[QString("G21")] = &VirtualMachine::g21;

    commands[QString("G90")] = &VirtualMachine::g90;
    commands[QString("G91")] = &VirtualMachine::g91;

    commands[QString("G92")] = &VirtualMachine::g92;

    commands[QString("G0")] = &VirtualMachine::g0;
    commands[QString("G1")] = &VirtualMachine::g1;
    commands[QString("G2")] = &VirtualMachine::g2;
    commands[QString("G3")] = &VirtualMachine::g3;
}

const VMState&
VirtualMachine::previousState() const
{
    return _previousState;
}

const VMState&
VirtualMachine::currentState() const
{
    return _currentState;
}

const VMState&
VirtualMachine::doBlock(CodeBlock* block)
{
    _previousState = _currentState;

    QMap<Word*, char>::const_iterator it = block->commandsBegin();
    for(; it != block->commandsEnd(); it++)
    {
        Word* cmd = it.key();

        // Speed Feed & Tool always
        cmd->parameterAsDouble(QChar('F'), &_currentState._feedRate);
        cmd->parameterAsDouble(QChar('S'), &_currentState._spindleSpeed);
        cmd->parameterAsInt(QChar('T'), &_currentState._nextTool);

        CommandFn fn = commands.value(cmd->toString());

        if (!fn) continue;

        (this->*fn)(cmd);
    }

    return _currentState;
}

void
VirtualMachine::reset()
{
    _moves.clear();
    _previousState = _currentState = VMState();
}

double convertToMM(double inches)
{
    // Yes, by the latest treaties 1 inch is EXACTLY 25.4 mm
    return (inches * 25.40);
}

// Set Inverse Time Mode
void
VirtualMachine::g93(Word* word)
{
    Q_UNUSED(word);
    qDebug("G93 not implemented. Feeds will be wrong.");
}

// Set Units Per Minute feed mode
void
VirtualMachine::g94(Word* word)
{
    Q_UNUSED(word);
    qDebug("G94 not implemented. Feeds will be wrong.");
}


// Manual tool change. Tx was set either on this line or a previous
// one
void
VirtualMachine::m6(Word* word) // change tool
{
    Q_UNUSED(word);
    _currentState._spindleOn = false;
    _currentState._currentTool = _currentState._nextTool;
}

// Force the tool to be Q as in  M61 Q
// This isn't actually going to work at all because Q will not be
// seen as a real value....
void
VirtualMachine::m61(Word* word) // set tool number
{
    Q_UNUSED(word);
    qWarning("M61 needs an un-worded number that is unsupported right now");
}


// Start the spindle clockwise
void
VirtualMachine::m3(Word* word)
{
    Q_UNUSED(word);
    _currentState._spindleOn = true;
}

// start the spindle counterclockwise
void
VirtualMachine::m4(Word* word)
{
    Q_UNUSED(word);
    _currentState._spindleOn = true;
}

// Stop the spindle
void
VirtualMachine::m5(Word* word) //
{
    Q_UNUSED(word);
    _currentState._spindleOn = false;
}

// Mist coolant on
void
VirtualMachine::m7(Word* word) //
{
    Q_UNUSED(word);
    _currentState._coolantOn = true;
}

// Flood coolant on
void
VirtualMachine::m8(Word* word) //
{
    Q_UNUSED(word);
    _currentState._coolantOn = true;
}

// All coolant off
void
VirtualMachine::m9(Word* word) //
{
    Q_UNUSED(word);
    _currentState._coolantOn = false;
}

// Dwell for P seconds
void
VirtualMachine::g4(Word* word)
{
    double p;
    if (word->parameterAsDouble(QChar('P'),&p) && p > 0)
    {
        _currentState._seconds += p;
    }
}

// Set inches length units
void
VirtualMachine::g20(Word* word)
{
    Q_UNUSED(word);
    _currentState._mmUnits = false;
}

// Set mm length units
void
VirtualMachine::g21(Word* word)
{
    Q_UNUSED(word);
    _currentState._mmUnits = true;
}


// G54-G59


// Set Absolute distance mode
void
VirtualMachine::g90(Word* word)
{
    Q_UNUSED(word);
    _currentState._movesAreRelative = false;
}

// Set relative distance mode
void
VirtualMachine::g91(Word* word)
{
    Q_UNUSED(word);
    _currentState._movesAreRelative = true;
}


// G28, G30  & G10

// Set the current point as the given value. Moves all axis
void
VirtualMachine::g92(Word* word)
{
    // We really should handle this a little differently, but for now this works.
    // All coordinate systems are supposed to have their own offsets that need updating,
    // but since we don't do anything other than the one set of offsets this works-ish

    // The offsets will be added to a point to convert from '92' to absolute space.
    // They are subtracted from absolute space to get '92' space

    double np;
    if (word->parameterAsDouble(QChar('X'), &np))
    {
        if (!_currentState._mmUnits) np = convertToMM(np);
        _currentState._off92MM._x = _currentState._absMM._x - np;
    }
    if (word->parameterAsDouble(QChar('Y'), &np))
    {
        if (!_currentState._mmUnits) np = convertToMM(np);
        _currentState._off92MM._y = _currentState._absMM._y - np;
    }
    if (word->parameterAsDouble(QChar('Z'), &np))
    {
        if (!_currentState._mmUnits) np = convertToMM(np);
        _currentState._off92MM._z = _currentState._absMM._z - np;
    }
}

void
VirtualMachine::getXYZ(Word* word, Position& delta)
{
    word->parameterAsDouble(QChar('X'), &delta._x);
    word->parameterAsDouble(QChar('Y'), &delta._y);
    word->parameterAsDouble(QChar('Z'), &delta._z);

    if (!_currentState._mmUnits)
    {
        delta._x = convertToMM(delta._x);
        delta._y = convertToMM(delta._y);
        delta._z = convertToMM(delta._z);
    }

    // Coordinates are always expressed in '92' space
    // We sort of presumably could decide that G53 (machine coordinates) are not effected by
    // these offsets, but I _think_ G53 is more relevant when you are switching fixture offsets
    // using G54, G55, etc.  In other words, I think G53 is kind of equivalent to a 'base' set
    // of NO fixture offsets, but G92 still applies.

    // Anyway, always apply G92 and ignore G53-G59.3 for now

    delta += _currentState._off92MM;
}

void
VirtualMachine::straightMoveAt(Word* word, double rate)
{
    Position delta;

    getXYZ(word, delta);
    // delta is returned in absolute coordinate space and scaled to proper units

    // If it's relative add the current position to it to get a final destination
    if (_currentState._movesAreRelative) delta += _currentState._absMM;

    // Delta is actually our new position, but we need to know the distance from the old
    // so that we know how long it will take to get there at our max feed rate
    double distance = delta.distanceFrom(_currentState._absMM);
    double time = distance / rate;

    // Store the straight move
    _moves.append(new StraightMove(_currentState._absMM, delta, time));

    _currentState._absMM = delta;
    _currentState._seconds += time;
}

// Rapid move to a new coordinate
void
VirtualMachine::g0(Word* word)
{
    straightMoveAt(word, _rapidRate);
}

// Current rate move to a new coordinate
void
VirtualMachine::g1(Word* word)
{
    straightMoveAt(word, _currentState._feedRate);
}


void
VirtualMachine::g2(Word* word)
{
    // For now, straighten all the curves. Sad.
    qDebug("G2 not supported yet. Straightening a curve");
    g1(word);
}

void
VirtualMachine::g3(Word* word)
{
    // For now, straighten all the curves. Sad.
    qDebug("G2 not supported yet. Straightening a curve");
    g1(word);
}


// stops?
