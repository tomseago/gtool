#include "word.h"

#include "parametermapper.h"
#include <QChar>

static QString gCommands("GMTFS");
static QString gInters("GMT#");
static QString gNeverParams("TFS");


static ParameterMapper paramMap;

Word::Word(const QChar &c) :
    _char(c),
    _double(0), // These should get set, but if they don't, don't be crazy
    _int(0),
    _wantsInt(false),
    _isCommand(false)
{    
    if (!_char.isUpper()) _char = _char.toUpper();

    _wantsInt = (gInters.indexOf(_char) != -1);
    _isCommand = (gCommands.indexOf(_char) != -1);

    // TODO: Mark which words have what parameters
}

// Int value overrides double value
Word::Word(const char c, const int intVal, const double doubleVal) :
    _char(c),
    _double(doubleVal),
    _int(intVal),
    _wantsInt(false),
    _isCommand(true)
{
    if (!_char.isUpper()) _char = _char.toUpper();

    // If the 3rd param is set, ignore the 2nd
    if (_int != INT_MAX)
    {
        _wantsInt = true;
        _double = _int;
    }

    // Otherwise, by default, use the second (maybe nothing) and don't want int
}


Word::Word(const Word* other) :
    _char(other->_char),
    _double(other->_double),
    _int(other->_int),
    _wantsInt(other->_wantsInt),
    _isCommand(other->_isCommand)
{
    // Nothing else....
}

Word::~Word()
{
    _parameters.keys();
}


bool
Word::operator==(const Word& other) const
{
    if (_char != other._char) return false;

    if (_wantsInt)
    {
        return _int == other._int;
    }
    else
    {
        return _double == other._double;
    }
}

bool
Word::operator!=(const Word& other) const
{
    return !(*this == other);
}

bool
Word::operator<(const Word& other) const
{
    // Parameters are rather easier
    if (!_isCommand)
    {
        if (_char == other._char)
        {
            if (_int == other._int)
            {
                return _double < other._double;
            }
            return _int < other._int;
        }
        return _char < other._char;
    }

    // Commands get whacky
    int me = commandOrder();
    int you = other.commandOrder();
    if (me != you) return me < you;

    // We are at the same order (which pretty much means we are the same command) so fall back
    // to boring int and double comparisons
    if (_int == other._int)
    {
        return _double < other._double;
    }
    return _int < other._int;
}

/*

The order of execution of items on a line is defined not by the position of each item on the line, but by the following list:

    - Comment (including message)
    - Set feed rate mode (G93, G94).
    - Set feed rate (F).
    - Set spindle speed (S).
    - Select tool (T).
            - HAL pin I/O (M62-M68).
    - Change tool (M6) and Set Tool Number (M61).
    - Spindle on or off (M3, M4, M5).
            - Save State (M70, M73), Restore State (M72), Invalidate State (M71).
    - Coolant on or off (M7, M8, M9).
    - Enable or disable overrides (M48, M49,M50,M51,M52,M53).
            - User-defined Commands (M100-M199).
    - Dwell (G4).
    - Set active plane (G17, G18, G19).
    - Set length units (G20, G21).
            - Cutter radius compensation on or off (G40, G41, G42)
            - Cutter length compensation on or off (G43, G49)
    - Coordinate system selection (G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3).
            - Set path control mode (G61, G61.1, G64)
    - Set distance mode (G90, G91).
    - Set retract mode (G98, G99).
        - Go to reference location (G28, G30) or change coordinate system data (G10) or set axis offsets (G92, G92.1, G92.2, G94).
    - Perform motion (G0 to G3, G33, G73, G76, G80 to G89), as modified (possibly) by G53.
    - Stop (M0, M1, M2, M30, M60).

*/
int
Word::commandOrder() const
{
    int order = 0;

#define G(num) if (_char=='G' && _int==(num)) return order; order++;
#define M(num) if (_char=='M' && _int==(num)) return order; order++;
#define o1(ltr) if (_char==(ltr) ) return order; order++;

    G(93); G(94);
    o1('F');
    o1('S');
    o1('T');
        M(62); M(63); M(64); M(65); M(66); M(67); M(68);
    M(6); M(61);
    M(3); M(4); M(5);
        M(70); M(73); M(72); M(71);
    M(7); M(8); M(9);
    M(48); M(49); M(50); M(51); M(52); M(53);
        // user defined commands
    G(4);
    G(17); G(18); G(19);
    G(20); G(21);
        G(40); G(41); G(42);
        G(43); G(49);
    G(54); G(55); G(56); G(57); G(58); G(59);
        G(61); G(64);
    G(90); G(91);
    G(98); G(99);
        G(28); G(30); G(10); G(92); G(94);

    G(53);
    G(0); G(1); G(2); G(3); G(33); G(73); G(76);

    G(80); G(81); G(82); G(83); G(84); G(85); G(86); G(87); G(88); G(89);

    M(0); M(1); M(2); M(30); M(60);

    // everything else - but make sure different commands are different
    return order + _char.toLatin1();
}


void
Word::setValue(QString& value)
{
    if (_wantsInt)
    {
        _int = value.toInt();
        _double = _int;
    }
    else
    {
        _int = INT_MAX;
        _double = value.toDouble();
    }
}

// For now we'll say only G and M are commands. That makes everything
// else a parameter in search of a home
bool
Word::isCommand() const
{
    return _isCommand;
}

bool
Word::acceptsParameter(const QChar &c) const
{
    if (gNeverParams.indexOf(c) != -1) return false;

    return paramMap.isRequiredFor(*this, c) || paramMap.isOptionalFor(*this, c);
}

bool
Word::acceptsParameter(const Word* other) const
{
    if (!other) return false;
    return acceptsParameter(other->_char);
}

bool
Word::canBeImplied() const
{
    // Very short list right now
    if ((_char == 'G') && (_int == 0 || _int == 1)) return true;

    return false;
}

void
Word::addParameter(const Word* other)
{
    if (!other) return;
    _parameters[other->_char] = new Word(other);
}

void
Word::addParameter(const Word& other)
{
    addParameter(&other);
}

void
Word::removeParameter(const QChar& c)
{
    if (_parameters.contains(c))
    {
        Word* w = _parameters.value(c);
        _parameters.remove(c);
        delete w;
    }
}


double
Word::doubleValue() const
{
    if (_wantsInt) return (double)_int;
    return _double;
}

int
Word::intValue() const
{
    if (_wantsInt) return _int;
    return (int)_double;
}

bool
Word::parameterAsDouble(const QChar& c, double* value) const
{
    Word* p = _parameters.value(c);
    if (!p) return false;

    if (value)
    {
        *value = p->doubleValue();
    }
    return true;
}

bool
Word::parameterAsInt(const QChar& c, int* value) const
{
    Word* p = _parameters.value(c);
    if (!p) return false;

    if (value)
    {
        *value = p->intValue();
    }
    return true;
}




QString
Word::toString() const
{
    if (_wantsInt) return QString("%1%2").arg(_char).arg(_int);

    return QString("%1%2").arg(_char).arg(_double);
}

QString
Word::toCommandString() const
{
    QStringList output;
    output.append(toString());

    QMap<QChar, Word*>::const_iterator it;
    for(it = _parameters.constBegin(); it != _parameters.constEnd(); ++it)
    {
        output.append(it.value()->toString());
    }

    return output.join(" ");
}


QString
Word::getCommandError() const
{
    return QString();
}
