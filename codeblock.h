#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include <QString>
#include <QList>

#include "word.h"
#include "parseerror.h"

class ParseError;

class CodeBlock
{
public:
    //CodeBlock();
    CodeBlock(QString& line, int lineNum = -1, const CodeBlock* previous = NULL);
    ~CodeBlock();

    const QString& original();
    const QString& formatted();

    bool hasError() const;
    const QList<ParseError*>& errors() const;

    // We'll want to pass some arguments into here in the future.
    void updateFormatted();


protected:
    typedef void (CodeBlock::*ParseFunction)();

private:
    int _fileLineNum;

    QString _original;
    QString _formatted;

    QList<ParseError*> _errorList;

    // Things that can be on the line
    bool _blockDelete;
    bool _percent;
    double _writtenLineNumber;

    QString* _comment;

    // This list holds ownership. Other lists are merely references.
    // Unfortunately, to shove things into lists though they have to
    // be pointers.
    QList<Word*> _words;

    // All words get sorted into one of these two lists
    QList<Word*> _commands;
    QList<Word*> _parameters;

    QMap<Word*, char> _orderedCommands;

    // If we have some parameters that we don't know what to do with, there
    // might be an implied command earlier in the command stack. This lets
    // us recurse backwards in search of a valid command to duplicate. We
    // might have to go way far back
    Word* _impliedCommand;
    const CodeBlock* _previous;


    const Word* searchForImpliedCommandFor(const Word* parameter) const;



    int _cursor;
    QChar _char;
    QString _value;
    bool _ignoreNumbers;
    Word* _currentWord;

    ParseFunction _commentParser;
    ParseFunction _nextParser;

    void parse();
    void recordError(const QString &error);

    bool checkForComment();
    void parseInlineComment();
    void parseEOLComment();

    void startNumber();
    bool appendNumberComponent();

    // ParseFunctions here
    void parseStart();
    void parseLineNo();
    void parseWordStart();
    void parseWordNumber();

    void parseUnknownWord();
    void parseUnknownDigits();
};

#endif // CODEBLOCK_H
