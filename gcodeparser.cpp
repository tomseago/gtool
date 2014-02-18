#include "gcodeparser.h"

#include <QTextStream>
#include <QDebug>

GCodeParser::GCodeParser(QFile& file) :
    _file(file),
    _lastBlock(NULL)
{
}

void
GCodeParser::parse()
{
    if (!_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Log an error???
        qWarning("Unable to open file");
        return;
    }

    QTextStream in(&_file);
    int numLines = 0;
    _lastBlock = NULL;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        process_line(line, numLines+1);
        numLines++;
    }

    _file.close();

    qDebug() << "Processed" << numLines << "lines";
}

void
GCodeParser::process_line(QString& line, int lineNum)
{
    CodeBlock* block = new CodeBlock(line, lineNum, _lastBlock);
    _lastBlock = block;

    _blocks.append(block);

//    if (block->hasError())
//    {
//        QVector<QParse*> list = block->errors();
//        for(int i=0; i<list.size(); i++)
//        {
//            qDebug() << *(list.at(i));
//        }
//    }

    // qDebug() << line;
}
