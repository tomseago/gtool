#include "progoptimizer.h"

#include "codeblock.h"
#include "virtualmachine.h"
#include "move.h"

ProgOptimizer::ProgOptimizer(QList<CodeBlock*> original) :
    _hasDiagonals(false),
    _numLayers(0)
{
    // Deep copy the original list

    for(int i=0; i<original.size(); i++)
    {
        CodeBlock* old = original.at(i);
        CodeBlock* neu = new CodeBlock(old);

        _original.append(neu);

        // Run the blocks through the virtual machine as we do this
        _vm.doBlock(neu);
    }

    // And now let's organize the moves into layers
    for (int i=0; i<_vm._moves.size(); i++)
    {
        Move* move = _vm._moves.at(i);

        // If all 3 axis change, that is whack
        if (!move->isZOnly() && move->changesZ()) _hasDiagonals = true;

        // Find a layer to put this in
        double z = move->_start._z;
        QList<Move*>* layer = _layers.value(z);
        if (!layer)
        {
            layer = new QList<Move*>();
            _layers[z] = layer;
            _numLayers++;
        }

        layer->append(move);
    }
}

ProgOptimizer::~ProgOptimizer()
{
    QList< QList<Move*>* > all = _layers.values();

    for(int i=0; i<all.size(); i++)
    {
        QList<Move*>* layer = all.at(i);
        delete layer;
    }

    for(int i=0; i<_original.size(); i++)
    {
        delete _original.at(i);
    }
}

const QList<double>
ProgOptimizer::layerZs() const
{
    QList<double> list;

    return _layers.keys();
}


// Everything above limit will be ignored and turned into moves that happen
// at safeLevel. Assuming the different layers below limit are different things
// i.e. one is lines and another is drills, we will do them in order from high
// to low, each time returning to safeLevel between lines.
QList<Word*>
ProgOptimizer::optimize(double limit, double safeLevel, double plungeRate, double retractRate, double drawRate)
{
    QList<Word*> out;
    Word* cmd;

    // Starting position is at 0,0,safeLevel from an analysis standpoint.
    // The first move will be to x,y,safe of the first plunge

    // Start with absolute coords
    out.append(new Word('G',90));

    // Move to the safe level
    cmd = new Word('G',0);
    cmd->addParameter(new Word('Z',INT_MAX,safeLevel));
    out.append(cmd);

    // Track x,y location
    Position location;

    // Iterate down through the layers ignoring anything above limit
    QList<double> keys = _layers.keys();
    for(int keyIx=0; keyIx<keys.size(); keyIx++)
    {
        double layerZ = keys.at(keyIx);
        if (layerZ > limit) continue;

        // Okay it's a layer we care about
        QList<Move*> *layer = _layers.value(layerZ);
        //if (!layer) continue; // weird...

        // Duplicate the layer so we can pop things out of it
        QList<Move*> dupe;
        for(int i=0; i<layer->size(); i++)
        {
            dupe.append(layer->at(i));
        }

        // For each move that occurs within this layer, assume we start at safe level
        // That means for each move in the layer we move to start of layer move, plunge, do layer move, retract

        // The secret is to keep finding the closest layer move until we are done
        Move* move = popClosestMove(*layer, location);
        while(move)
        {
            // Okay, we have a move, which starts somewhere other than where we are, so fast move
            // to above it.
            cmd = new Word('G',0);
            cmd->addParameter(new Word('X',INT_MAX,move->_start._x));
            cmd->addParameter(new Word('Y',INT_MAX,move->_start._y));
            out.append(cmd);

            // Plunge down to the z depth of the current layer
            cmd = new Word('G',1);
            cmd->addParameter(new Word('Z',INT_MAX,layerZ));
            cmd->addParameter(new Word('F',INT_MAX,plungeRate));
            out.append(cmd);

            // Do the move, proper rate, absolute. Nuke Z though
            cmd = move->getCommand(drawRate, true);
            cmd->removeParameter(QChar('Z'));
            out.append(cmd);
            // Update our location with the end of this move
            cmd->parameterAsDouble(QChar('X'),&location._x);
            cmd->parameterAsDouble(QChar('Y'),&location._y);

            // Retract back to safe layer
            cmd = new Word('G',1);
            cmd->addParameter(new Word('Z',INT_MAX,safeLevel));
            cmd->addParameter(new Word('F',INT_MAX,retractRate));
            out.append(cmd);

            // And iterate to the next move
            move = popClosestMove(*layer, location);
        }

        // No more moves, so time to iterate to next layer down
    }

    // And that's it - awesome!
    return out;
}

Move*
ProgOptimizer::popClosestMove(QList<Move*>& layer, Position& loc)
{
    double minDistance = DBL_MAX;
    Move* move = NULL;

    // Brute force bitches!!!
    for(int i=0; i<layer.size(); i++)
    {
        Move* candidate = layer.at(i);

        double distance = candidate->_start.distanceFrom(loc);
        if (distance < minDistance)
        {
            minDistance = distance;
            move = candidate;
        }
    }

    if (move)
    {
        layer.removeAll(move);
    }

    return move;
}
