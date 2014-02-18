#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QBrush>
#include <QColor>
#include <QFileInfo>

#include "gcodeparser.h"
#include "parseerror.h"
#include "ui_errorslistmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _file(NULL)
{
    commonConstruction();
}

MainWindow::MainWindow(QFile* file) :
    QMainWindow(0),
    _ui(new Ui::MainWindow),
    _file(file)
{
    commonConstruction();

    parseFile();
}

MainWindow::~MainWindow()
{
    delete _ui;
    _ui = NULL;

    if (_file) delete _file;
    _file = NULL;
}

void
MainWindow::commonConstruction()
{
    _ui->setupUi(this);

    _ui->actionOpen_gcode->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));

    _ui->originalTextEdit->setDocument(&_doc);
    _ui->reformattedTextEdit->setDocument(&_reformattedDoc);

    // Seutp our formats
    _errorFormat.setForeground(QBrush(QColor("red")));

    _errorHighlightFormat.setBackground(QBrush(QColor("red")));
    _errorHighlightFormat.setForeground(QBrush(QColor("black")));
}




void
MainWindow::on_actionOpen_gcode_triggered()
{
    qDebug("Open a file");

    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open gcode"), "", tr("gcode (*.gc *.gcode *.nc);;All Files (*.*)"));

    qDebug() << "Open" << filename;

    if (!filename.isEmpty())
    {
        QFile* newFile = new QFile(filename);

        if (_file)
        {
            // Should actually look for a window with it open and just bring it to the front

            MainWindow* next = new MainWindow(newFile);
            next->show();
        }
        else
        {
            _file = newFile;

            parseFile();
        }
    }
}

void
MainWindow::parseFile()
{
    // This is effectively "update from file" so we do UI stuff also and stuff
    QFileInfo info(*_file);
    setWindowFilePath(_file->fileName());
    setWindowTitle(QString("%1 - GTool").arg(info.fileName()));

    GCodeParser parser(*_file);

    parser.parse();
    updateFromCodeBlocks(parser._blocks);
}

void
MainWindow::on_actionQuit_triggered()
{
    // Should actually do some sort of save check someday
    QApplication::closeAllWindows();
}

void
MainWindow::setFormatAt(QItemSelection& item, QTextCharFormat& format, bool scrollToCursor)
{
    if (item.isEmpty()) return;
    if (!item.first().isValid()) return;
    QVariant errorV = _ui->errorsView->model()->data(item.first().topLeft(), Qt::UserRole);

    if (!errorV.isValid()) return;

    ParseError* error = errorV.value<ParseError*>();

    QTextCursor cursor(&_doc);
    if (error->_line > 1)
    {
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, error->_line - 1);
    }
    if (error->_column > 1)
    {
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, error->_column - 1);
    }
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

    cursor.setCharFormat(format);

    if (scrollToCursor)
    {
        cursor.movePosition(QTextCursor::Left);
        _ui->originalTextEdit->setTextCursor(cursor);
        _ui->originalTextEdit->ensureCursorVisible();               
    }
}

void
MainWindow::errorSelectionChanged(QItemSelection current, QItemSelection previous)
{
    Q_UNUSED(previous);

    setFormatAt(previous, _errorFormat);
    setFormatAt(current, _errorHighlightFormat, true);
}

void
MainWindow::updateFromCodeBlocks(const QList<CodeBlock*>& blocks)
{
    QTextCursor cursor(&_doc);
    QTextCursor refCursor(&_reformattedDoc);

    cursor.beginEditBlock();
    _doc.clear();

    QList<ParseError*> allErrors;
    for(int i=0; i<blocks.size(); i++)
    {
        CodeBlock* block = blocks.at(i);

        QTextCharFormat& blockFormat = block->hasError() ? _errorFormat : _normalFormat;

        if (block->hasError())
        {
//            QList<ParseError*> errors = block->errors();
//            for(int t=0; t<errors.size(); t++)
//            {
//                ParseError* error = errors.at(t);
//                error->_uiData = cursor.block();
//                allErrors.append(block->errors())
//            }
            allErrors.append(block->errors());
        }

        // Assume we inserted a block at the end
        cursor.setBlockCharFormat(blockFormat);

        cursor.insertText(block->original());
        cursor.insertBlock();

        refCursor.insertText(block->formatted());
        refCursor.insertBlock();
    }

    cursor.endEditBlock();

    UIErrorsListModel* newModel = new UIErrorsListModel(_ui->errorsView, allErrors);
    _ui->errorsView->setModel(newModel);

    QHeaderView* hv = _ui->errorsView->header();
    hv->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    hv->setSectionResizeMode(1,QHeaderView::ResizeToContents);

    _ui->errorsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->errorsView->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel* selectionModel = _ui->errorsView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(errorSelectionChanged(QItemSelection,QItemSelection)));
}
