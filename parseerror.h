#ifndef PARSEERROR_H
#define PARSEERROR_H

#include <QString>
#include <QVariant>

class CodeBlock;

#include "codeblock.h"

class ParseError
{

public:
    ParseError();
    ParseError(ParseError* other);
    ParseError(int line, int column, const QString error, const CodeBlock* block);

    int _line;
    int _column;
    const QString _error;

    const CodeBlock* _block;

    void* _uiData;

    QVariant data(const QModelIndex &index, int role) const;
};
Q_DECLARE_METATYPE(ParseError*);

#endif // PARSEERROR_H
