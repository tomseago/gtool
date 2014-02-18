#include "ui_errorslistmodel.h"

#include <QDebug>
#include <QSize>

UIErrorsListModel::UIErrorsListModel(QObject *parent, QList<ParseError*>& errors) :
    QAbstractTableModel(parent),
    _errors()
{
    // We need to make a deep copy of all the stuff in the error list
    for(int i=0; i<errors.size(); i++)
    {
        _errors.append(new ParseError(errors.at(i)));
    }
}


UIErrorsListModel::~UIErrorsListModel()
{
    qDeleteAll(_errors);
    _errors.clear();
}

int
UIErrorsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _errors.size();
}

int
UIErrorsListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 3;
}

QVariant
UIErrorsListModel::data(const QModelIndex &index, int role) const
{
    ParseError* error = _errors.at(index.row());

    if (role == Qt::UserRole)
    {
        return QVariant::fromValue(error);
    }

    return error->data(index, role);
}


QVariant
UIErrorsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    Q_UNUSED(role);

//    if (role == Qt::SizeHintRole)
//    {
//        qDebug() << "Size hint for" << section;
//    }
    if (role != Qt::DisplayRole && role != Qt::SizeHintRole)
    {
//        qDebug() << role;
        return QVariant();
    }

    switch(section)
    {
    case 0:
        if (role == Qt::SizeHintRole) return QSize(30,1);
        return tr("Line");

    case 1:
        if (role == Qt::SizeHintRole) return QSize(30,1);
        return tr("Col");

    default:
        if (role == Qt::SizeHintRole) return QVariant();
        return tr("Error");
    }

}

