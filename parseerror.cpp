#include "parseerror.h"

#include <QDebug>
#include <QAbstractItemModel>

ParseError::ParseError(ParseError* other) :
    _line(other->_line),
    _column(other->_column),
    _error(other->_error),
    _block(other->_block),
    _uiData(NULL)
{

}

ParseError::ParseError(int line, int column, const QString error, const CodeBlock* block) :
    _line(line),
    _column(column),
    _error(error),
    _block(block),
    _uiData(NULL)
{
}

QVariant
ParseError::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        //qDebug() << index << role;

        switch(index.column())
        {
        case 0:
            return QVariant(_line);
            break;

        case 1:
            return QVariant(_column);
            break;

        default:
            return _error;
        }
    }

    return QVariant();
}
