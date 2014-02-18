#ifndef WORD_H
#define WORD_H

#include <climits>
#include <cfloat>

#include <QString>
#include <QMap>
#include <QHash>

class Word
{
public:
    Word(const QChar& c);
    Word(const char c, const int intVal = INT_MAX, const double doubleVal= DBL_MAX);
    Word(const Word* other);

    // TODO: make these private...
    QChar _char;
    double _double;
    int _int;

    QMap<QChar, Word*> parameters;

    void setValue(QString& value);

    bool isCommand() const;
    bool acceptsParameter(const QChar& c) const;
    bool acceptsParameter(const Word* other) const;
    bool canBeImplied() const;

    void addParameter(Word* other);

    QString toString();
    QString toCommandString();
    QString getCommandError();

    bool operator==(const Word& other) const;
    bool operator!=(const Word& other) const;
    bool operator<(const Word& other) const;

    inline uint qHash(uint seed = 0) const { return ::qHash(_char, seed) + _int; }

private:
    bool _wantsInt;

    bool _isCommand;
    QString _requiredParams;
    QString _optionalParams;

    int commandOrder() const;
};

inline uint qHash(const Word& word, uint seed = 0) { return word.qHash(seed); }

#endif // WORD_H
