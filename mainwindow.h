#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextDocument>
#include <QList>
#include <QTextCharFormat>
#include <QFile>
#include <QItemSelection>

#include "codeblock.h"
#include "machinewindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    MainWindow(QFile* file);
    ~MainWindow();

public slots:
    void errorSelectionChanged(QItemSelection current, QItemSelection previous);

private slots:
    void on_actionOpen_gcode_triggered();

    void on_actionQuit_triggered();

    void machineWindowReady();


private:
    Ui::MainWindow *_ui;

    QFile* _file;
    QTextDocument _doc;
    QTextDocument _reformattedDoc;
    QTextCharFormat _errorFormat;
    QTextCharFormat _errorHighlightFormat;
    QTextCharFormat _normalFormat;

    MachineWindow _machineWindow;

    void commonConstruction();

    void parseFile();
    void updateFromCodeBlocks(const QList<CodeBlock*>& blocks);

    void setFormatAt(QItemSelection& item, QTextCharFormat& format, bool scrollToCursor = false);
};

#endif // MAINWINDOW_H
