#ifndef MACHINEWINDOW_H
#define MACHINEWINDOW_H

#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>
#include <QMatrix4x4>
#include <QPoint>

#include <QList>

#include "word.h"

class MachineWindow : public QWindow
{
    Q_OBJECT

public:
    explicit MachineWindow(QWindow *parent = 0);
    ~MachineWindow();

    virtual void initialize();

    virtual void render();
    virtual void render(QPainter *painter);

    void setProgram(QList<Word*>& prog);

signals:
    void readyForData();

public slots:

protected:
    virtual void exposeEvent(QExposeEvent* evt);
    virtual void resizeEvent(QResizeEvent* evt);
    virtual void mousePressEvent(QMouseEvent *evt);
    virtual void mouseMoveEvent(QMouseEvent *evt);

private:
    QOpenGLDebugLogger* _logger;
    QOpenGLShaderProgram _shaderProg;
    bool _hasInitialized;

    QOpenGLContext* _context;
    QOpenGLFunctions_3_2_Core* _gl;
    QOpenGLVertexArrayObject* _vao;

    QList<Word*> _program;

    QOpenGLBuffer _positions;

    QMatrix4x4 _matModel;
    QMatrix4x4 _matView;
    QMatrix4x4 _matProject;

    QPoint _dragStart;

private slots:
    void onDebugMsg(QOpenGLDebugMessage msg);
};

#endif // MACHINEWINDOW_H
