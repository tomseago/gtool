#ifndef PROGOPTIMIZER_H
#define PROGOPTIMIZER_H

#include <QList>
#include <QMap>
#include <QVariant>

#include "virtualmachine.h"

class CodeBlock;
class VirtualMachine;
class Move;

class Optimizer
{
public:
    Optimizer(QList<CodeBlock*> original);
    ~Optimizer();

    bool _hasDiagonals;
    int _numLayers;

    const QList<double> layerZs() const;

    QList<Word*> optimize(double limit, double safeLevel, double plungeRate, double retractRate, double drawRate);

private:
    QList<CodeBlock*> _original;

    QMap<double, QList<Move*> * > _layers;
    VirtualMachine _vm;

    Move* popClosestMove(QList<Move*>& layer, Position& loc);
};

#endif // PROGOPTIMIZER_H
