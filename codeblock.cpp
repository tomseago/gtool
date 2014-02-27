#include "codeblock.h"

#include <QStringList>


CodeBlock::CodeBlock(QString& line, int lineNum, const CodeBlock* previous) :
    _fileLineNum(lineNum),
    _original(line),
    _blockDelete(false),
    _percent(false),
    _writtenLineNumber(0),
    _comment(NULL),
    _impliedCommand(NULL),
    _previous(previous),
    _cursor(0),
    _ignoreNumbers(false),
    _currentWord(NULL),
    _commentParser(NULL),
    _nextParser(NULL)
{
    parse();
}

CodeBlock::~CodeBlock()
{
    for(int i=0; i<_errorList.size(); i++)
    {
        delete _errorList.at(i);
    }
    _errorList.clear();

    if (_comment) delete _comment;
    _comment = NULL;

    for(int i=0; i<_words.size(); i++)
    {
        delete _words.at(i);
    }
    _words.clear();

    // _currentWord is inside the _words vector so it gets
    // deleted from there
    _currentWord = NULL;
}

const QString&
CodeBlock::original()
{
    return _original;
}

const QString&
CodeBlock::formatted()
{
    return _formatted;
}

bool
CodeBlock::hasError() const
{
    return _errorList.size() > 0;
}

const QList<ParseError *> &CodeBlock::errors() const
{
    return _errorList;
}

////////////////////////////////////////////////////////////////

void
CodeBlock::parse()
{
    _cursor = 0;
    int len = _original.length();
    _value.clear();
    _commentParser = NULL;
    _nextParser = &CodeBlock::parseStart;

    while(_cursor < len)
    {
        _char = _original[_cursor++];

        // Ignore all spaces from a parsing perspective at all times

        if (_commentParser)
        {
            (this->*_commentParser)();
            continue;
        }

        // When not a comment, ignore all whitespace
        if (!_char.isSpace())
        {
            (this->*_nextParser)();
        }
    }

    // We've run out of useful characters, so let our parsing funciton know that
    // the line has ended (as long as it exists that is)
    _char = QChar();
    if (_commentParser) (this->*_commentParser)();
    if (_nextParser) (this->*_nextParser)();

    // Okay, now it's time to condense all the parameters into commands and so forth.
    // Everything has been put into a command or parameter list as the words objects were created
    // so our task here is to attach the parameters to the commands they belong to detecting
    // any places where there are ambiguities
    for(int i=0; i<_parameters.size(); i++)
    {
        Word* parameter = _parameters.at(i);
        Word* attached = NULL;
        for(int j=0; j<_commands.size(); j++)
        {
            Word* command = _commands.at(j);
            if (command->acceptsParameter(parameter))
            {
                if (attached)
                {
                    // Error condition
                    recordError(QString("Ambigous parameter %1 already attached to %2, but could also apply to %3")
                                .arg(parameter->toString())
                                .arg(attached->toString())
                                .arg(command->toString()));
                }
                else
                {
                    command->addParameter(parameter);
                    attached = command;
                }
            }
        }

        if (!attached)
        {
            // Maybe there is an implied command it can be associated with?
            if (!_impliedCommand)
            {
                const Word* parentCommand = searchForImpliedCommandFor(parameter);
                // Save it away if found
                if (parentCommand)
                {
                    _impliedCommand = new Word(parentCommand);
                    _words.append(_impliedCommand);
                }

            }

            if (_impliedCommand)
            {
                if (_impliedCommand->acceptsParameter(parameter))
                {
                    _impliedCommand->addParameter(parameter);
                }
                else
                {
                    // Since there is already something implied we don't go back further
                    // trying to pull in something else. Just error
                    recordError(QString("Orphaned parameter %1 did not apply to any command on this line or the current implied command")
                                .arg(parameter->toString()));
                }
            }
            else
            {
                // Couldn't find an implied command to take it, so just bail
                recordError(QString("Orphaned parameter %1 did not apply to any command on this line.")
                            .arg(parameter->toString()));
            }
        }
    }

    // Now see if any of the commands are sorely missing parameters
    // Right now the only implied commands are G0 and G1. Since they only need one parameter, and they
    // would only be implied if they had gotten a parameter we don't have to consider errors from them.
    for(int i=0; i<_commands.size(); i++)
    {
        Word* cmd = _commands.at(i);
        QString cmdError = cmd->getCommandError();
        if (!cmdError.isEmpty())
        {
            recordError(cmdError);
        }

        // Final step is to re-order the commands
        _orderedCommands[cmd] = 1;
    }

    // But of course an implied command needs to be in the proper order so this...
    if (_impliedCommand)
    {
        _orderedCommands[_impliedCommand] = 1;
    }

    updateFormatted();
}

void
CodeBlock::updateFormatted()
{
    // Make our formatted output
    QStringList strs;

    if (_percent)
    {
        // Special percent handling
        strs.append("%");
    }
    else
    {
        // Possibly some commands
        if (_blockDelete) strs.append("/ ");
        if (_writtenLineNumber) strs.append(QString("N%1 ").arg(_writtenLineNumber));

        if (_impliedCommand) strs.append("  ");


        // In order of course
        QMap<Word*, char>::const_iterator it;
        for(it = _orderedCommands.constBegin(); it != _orderedCommands.constEnd(); ++it)
        {
            strs.append(it.key()->toCommandString());
            strs.append(" ");
        }
    }

    // Comments always at the EOL
    if (_comment && _comment->length() > 0) strs.append(QString("(%1)").arg(_comment->trimmed()));

    _formatted = strs.join("");
}


/**
 * @brief CodeBlock::searchForImpliedCommandFor
 * @param parameter
 * @return
 *
 * Recurses up our chain of parents to see if anyone can tell us what motion mode we are in.
 */
const Word*
CodeBlock::searchForImpliedCommandFor(const Word* parameter) const
{
    for(int i=0; i<_commands.size(); i++)
    {
        Word* cmd = _commands.at(i);
        if (cmd->canBeImplied() && cmd->acceptsParameter(parameter)) return cmd;
    }

    // Didn't find it, recurse to a parent if we have one
    if (_previous) return _previous->searchForImpliedCommandFor(parameter);

    // Nope
    return NULL;
}




void
CodeBlock::recordError(const QString& error)
{
    ParseError* e = new ParseError(_fileLineNum, _cursor+1, error, this);
    _errorList.append(e);
}

bool
CodeBlock::checkForComment()
{
    if (_char == '(')
    {
        // Yep! go into comment mode
        if (!_comment) _comment = new QString();
        _comment->clear();
        _commentParser = &CodeBlock::parseInlineComment;
        return true;
    }

    if (_char == ';')
    {
        // From here until the end of line it is a comment
        if (!_comment) _comment = new QString();
        _comment->clear();
        _commentParser = &CodeBlock::parseEOLComment;
        return true;
    }

    // Not a comment;
    return false;
}

void
CodeBlock::startNumber()
{
    _ignoreNumbers = false;
    _value.clear();
}

bool
CodeBlock::appendNumberComponent()
{
    // Digits for sure
    if (_char.isDigit()) goto append;

    // Decimals as long as there isn't a decimal in the value already
    if (_char == '.')
    {
        if (_value.indexOf('.') != -1)
        {
            recordError(QString("Warning: Multiple decimal points makes this number invalid"));
            _ignoreNumbers = true;
        }
        else
        {
            _value.append('.');
        }

        // Still allow it, but we will ignore things now
        return true;
    }

    // Plus or minus at the start of the line
    if ((_char == '+' || _char == '-') && (_value.length() == 0)) goto append;


    // We know it's not a comment (that's a separate check) so set the position back by one
    _cursor--;
    return false;

append:
    if (!_ignoreNumbers) _value.append(_char);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////
void
CodeBlock::parseInlineComment()
{
    // Are we done?
    if (_char.isNull() || _char == ')')
    {
        // All done with the comment. We saved it as we went so just return
        _commentParser = NULL;
        return;
    }

    // Collect the character
    _comment->append(_char);
}

void
CodeBlock::parseEOLComment()
{
    // If EOL we are done
    if (_char.isNull()) return;

    // Collect all characters. Nothing can stop us! (except the end of the line)
    _comment->append(_char);
}

void
CodeBlock::parseStart()
{
    // If EOL we are done. Nothing to do. Not in the middle of anything because
    // we haven't even started yet.
    if (_char.isNull()) return;

    // Can start with the block delete marker if we want
    if (_char == '/')
    {
        _blockDelete = true;
        return;
    }
    _blockDelete = false;

    // Treat % markers as representing EOL comment start markers
    if (_char == '%')
    {
        _percent = true;
        if (!_comment) _comment = new QString();
        _comment->clear();
        _commentParser = &CodeBlock::parseEOLComment;
        return;
    }

    if (checkForComment()) return;

    // It can either be a line number or a standard word
    if (_char.toUpper() == 'N')
    {
        startNumber();
        _nextParser = &CodeBlock::parseLineNo;
        return;
    }

    // Else, it's not a line number so behave as if it is parseWordStart right away
    parseWordStart();
}

void
CodeBlock::parseLineNo()
{
    if (_char.isNull() || checkForComment() || !appendNumberComponent())
    {
        // done parsing the number
        if (_value.length() == 0)
        {
            recordError(QString("Line number word should have a valid number"));
        }
        else
        {
            _writtenLineNumber = _value.toDouble();
        }

        // No matter what, go on to the next word
        _nextParser = &CodeBlock::parseWordStart;
    }
}

void
CodeBlock::parseWordStart()
{
    if (_char.isNull() || checkForComment()) return;

    if (!(_char.isLetter() || _char == '#'))
    {
        recordError(QString("Expect a word beginning with a letter"));
        return;
    }

    _currentWord = new Word(_char);
    // Hold identity in this list
    _words.append(_currentWord);

    // Put it into it's proper list based on being a command or not
    if (_currentWord->isCommand())
    {
        _commands.append(_currentWord);
    }
    else
    {
        _parameters.append(_currentWord);
    }

    // So far all we have is the character which starts the word, so now get
    // the number for this word
    startNumber();
    _nextParser = &CodeBlock::parseWordNumber;
}

void
CodeBlock::parseWordNumber()
{
    if (_char.isNull() || checkForComment() || !appendNumberComponent())
    {
        // done parsing the number
        if (_value.length() == 0)
        {
            // Instead of complaining right away, we're going to save up this word
            // and mark the whole thing as bogus
            _value.clear();
            _value.append(_currentWord->_char);

            // We started a word though, so nuke the one we started because we no longer care
            if (_currentWord->isCommand())
            {
                _commands.pop_back();
            }
            else
            {
                _parameters.pop_back();
            }

            _words.pop_back();
            delete _currentWord;

            // But jump in here as a comment parser so we get everything including whitespace
            _commentParser = &CodeBlock::parseUnknownWord;
        }
        else
        {
            _currentWord->setValue(_value);
        }

        // No matter what, go on to the next word
        _nextParser = &CodeBlock::parseWordStart;
    }
}

void
CodeBlock::parseUnknownWord()
{
    if (_char.isNull() || _char.isSpace())
    {
        // Consider this the end of the unknown word/comment thing
        recordError(QString("Unrecognized syntax word '%1'").arg(_value));
        _commentParser = NULL;
    }
    else
    {
        _value.append(_char);

        if (_char.isDigit() || (_char == '-') || (_char == '+') || (_char == '.'))
        {
            _commentParser = &CodeBlock::parseUnknownDigits;
        }
    }
}

void
CodeBlock::parseUnknownDigits()
{
    if (_char.isDigit() || (_char == '-') || (_char == '+') || (_char == '.'))
    {
        _value.append(_char);
    }
    else
    {
        // A space or a letter or any other character ends the unknown block
        recordError(QString("Unrecognized syntax word & number '%1'").arg(_value));
        _commentParser = NULL;

        // But we have to consider this the start of a real word (possibly)
        parseWordStart();
    }
}
