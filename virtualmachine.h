#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <QHash>

#include "vmstate.h"

class CodeBlock;
class Word;
class Move;

class VirtualMachine
{
public:
    VirtualMachine(double _rapidRate = 6000); // G0 speed in mm/min

    const VMState& previousState() const;
    const VMState& currentState() const;

    const VMState& doBlock(CodeBlock* block);
    void reset();

    double _rapidRate;
    QList<Move*> _moves;

protected:
    typedef void (VirtualMachine::*CommandFn)(Word* word);

private:
    VMState _previousState;
    VMState _currentState;

    void getXYZ(Word* word, Position& delta);
    void straightMoveAt(Word* word, double rate);

    QHash<QString, CommandFn> commands;

    void setFeed(Word* word); // feed rate
    void setSpeed(Word* word); //
    void setTool(Word* word); //

    void g93(Word* word);
    void g94(Word* word);

    void m6(Word* word); // change tool
    void m61(Word* word); // set tool number

    void m3(Word* word);
    void m4(Word* word);
    void m5(Word* word); //

    void m7(Word* word); //
    void m8(Word* word); //
    void m9(Word* word); //

    void g4(Word* word); //

    void g20(Word* word); //
    void g21(Word* word); //

    // G54-G59

    void g90(Word* word); //
    void g91(Word* word); //

    // G28, G30  & G10
    void g92(Word* word); //

    void g0(Word* word);
    void g1(Word* word);
    void g2(Word* word);
    void g3(Word* word);

    // stops?
};

#endif // VIRTUALMACHINE_H
