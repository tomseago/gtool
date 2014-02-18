#include "parametermapper.h"

#include "word.h"

ParameterMapper::ParameterMapper() :
    _db()
{
    QStringList none = QStringList() << "" << "";

    // Move - straight
    _db[Word('G',0)] = QStringList() << "" << "XYZABCUVWF";
    _db[Word('G',1)] = _db[Word('G',0)];

    QStringList test = _db[Word('G',0)];
    Q_ASSERT(test.size() == 2);

    // Move - arc
    _db[Word('G',2)] = QStringList() << "" << "XYZIJKPRF";
    _db[Word('G',3)] = _db[Word('G',2)];

    // Dwell
    _db[Word('G',4)] = QStringList() << "P" << "";

    // Various sub commands
    _db[Word('G',10)] = QStringList() << "LP" << "XYZRIJQ";

    // Plane selection
    // g17, g18, g19  - no params
    _db[Word('G',17)] = _db[Word('G',18)] = _db[Word('G',19)] = none;

    // Coordinates, inches or millimeters
    _db[Word('G',20)] = _db[Word('G',21)] = none;

    // TODO: g28, g30 predefined position

    // Tool compensation g40
    // TODO: cutter compensation g41, g42

    // Select coordinate system
    _db[Word('G',54)] = _db[Word('G',55)] = _db[Word('G',56)] = _db[Word('G',57)] = _db[Word('G',58)] = none;

    // Absolute distance mode
    _db[Word('G',90)] = none;

    // Relative distance mode
    _db[Word('G',91)] = none;

    // Coordinate system offset (accepts axis) i.e. set position
    _db[Word('G',92)] = _db[Word('G',0)];

    // Spindle control mode
    _db[Word('G',96)] = QStringList() << "S" << "D";

    // Select RPM mode
    // TODO: g[97]


    ///////////////////////////
    // Program pause
    _db[Word('M',0)] = _db[Word('M',1)] = none;

    // Program end
    _db[Word('M',2)] = _db[Word('M',30)] = none;

    // Pallet change
    _db[Word('M',60)] = none;

    // Spindle control: start clockwise, start counterclockwise, stop
    _db[Word('M',3)] = _db[Word('M',4)] = _db[Word('M',5)] = none;

    // Tool change
    _db[Word('M',6)] = none; // general includes a T command though

    // Coolant control
    _db[Word('M',7)] = _db[Word('M',8)] = _db[Word('M',9)] = none;

    // Speed and feed override enabled/disable
    _db[Word('M',48)] = _db[Word('M',49)] = none;

    // Feed override
    _db[Word('M',50)] = QStringList() << "" << "P";

    // Spindle speed override
    _db[Word('M',51)] = _db[Word('M',50)];

    // Adaptive feed and feed stop
    _db[Word('M',52)] = _db[Word('M',53)] = _db[Word('M',50)];

    // Set current tool number without tool change
    _db[Word('M',61)] = QStringList() << "Q" << "";

    // Wait on input
    _db[Word('M',66)] = QStringList() << "" << "PELQ";

    // TODO: Additional 3dprinter commands


}

bool
ParameterMapper::isRequiredFor(const Word& word, const QChar& param) const
{
    QStringList list = _db[word];
    //if (&list == NULL) return false;
    if (list.isEmpty()) return false;
    return list.first().indexOf(param) != -1;
}

bool
ParameterMapper::isOptionalFor(const Word& word, const QChar& param) const
{
    QStringList list = _db[word];
    //if (&list == NULL) return false;
    if (list.isEmpty()) return false;
    return list.last().indexOf(param) != -1;

}

bool
ParameterMapper::isKnown(const Word& word) const
{
    QStringList list = _db[word];
    //if (&list == NULL) return false;
    if (list.isEmpty()) return false;
    return true;
}
