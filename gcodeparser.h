#ifndef GCODEPARSER_H
#define GCODEPARSER_H

#include <QFile>

#include "codeblock.h"

class GCodeParser
{
public:
    GCodeParser(QFile& file);

    void parse();

    QList<CodeBlock*> _blocks;

private:
    QFile& _file;

    CodeBlock* _lastBlock;

    void process_line(QString& line, int lineNum);

};

#endif // GCODEPARSER_H
