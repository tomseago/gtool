#ifndef PARAMETERMAPPER_H
#define PARAMETERMAPPER_H

#include <QHash>
#include <QStringList>

class Word;

class ParameterMapper
{
public:
    ParameterMapper();

    bool isRequiredFor(const Word& word, const QChar& param) const;
    bool isOptionalFor(const Word& word, const QChar& param) const;

    bool isKnown(const Word& word) const;


private:
    QHash<Word, QStringList> _db;
//    QMap<QChar, QMap<int, QStringList > > *_db;
};



#endif // PARAMETERMAPPER_H
