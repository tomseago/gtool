#ifndef UI_ERRORSLISTMODEL_H
#define UI_ERRORSLISTMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "parseerror.h"

class UIErrorsListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UIErrorsListModel(QObject *parent, QList<ParseError*>& errors);
    ~UIErrorsListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;


signals:

public slots:

private:
    QList<ParseError*> _errors;

};

#endif // UI_ERRORSLISTMODEL_H
