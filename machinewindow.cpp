#include "machinewindow.h"

#include <QResizeEvent>
#include <QMouseEvent>

#include <math.h>

const uint NumVertices =  6;


MachineWindow::MachineWindow(QWindow *parent) :
    QWindow(parent),
    _logger(NULL),
    _shaderProg(this),
    _hasInitialized(false),
    _positions(QOpenGLBuffer::VertexBuffer)
{
    setSurfaceType(QWindow::OpenGLSurface);

    // Create the format for our OpenGL Surface
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion(3);
    format.setMinorVersion(2);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    setFormat( format );
    create();

    // Create a context
    _context = new QOpenGLContext;
    _context->setFormat(format);
    _context->create();

    // Associate these two things
    _context->makeCurrent( this );

    // Get our open gl functions. Instead of just inheriting this lets
    // us have full access. Which I guess is cool?
    // From https://developer.apple.com/graphicsimaging/opengl/capabilities/
    // a good version of things is OpenGL 3.2, GLSL 150
    _gl = (QOpenGLFunctions_3_2_Core*)_context->versionFunctions();
    if (!_gl)
    {
        qWarning("Couldn't get gl functions. Crash eminent");
    }
    _gl->initializeOpenGLFunctions();

    // We will later want the vao so make it now??
    _vao = NULL;
}

MachineWindow::~MachineWindow()
{
    // Yep...
}

void
MachineWindow::exposeEvent(QExposeEvent *evt)
{
    Q_UNUSED(evt);

    if (isExposed() && !_hasInitialized)
    {
        initialize();
    }
}

void
MachineWindow::resizeEvent(QResizeEvent* evt)
{
    //Q_UNUSED(evt);

    const QSize& s = evt->size();

    _matProject.setToIdentity();
    //_matProject.frustum(-1, 1, -1, 1, 0, 1);
    //_matProject.frustum(0, s.width()/10, 0, s.height()/10, -100, 100);
    //_matProject.frustum(-100, 100, -100.0, 100.0, -0.1, 100.0);

    GLfloat aspect = (float)s.height() / (float)s.width();

    _matProject.frustum(-1, 1, -1 * aspect, 1 * aspect, 1.0, 1000.0);
    //_matProject.translate(0.2, 0.2, 0);
    //_matProject = _matProject.inverted();

    //_matModel.setToIdentity();
    //_matModel.translate(1,0,-1.4);


    //_matModel.scale(0.08);
    //_matModel.translate(1, 1, 10);
    //_matModel.frustum(-20, 20, -20, 20, 20, 100);

    if (_hasInitialized)
    {
        render();
    }
}

void
MachineWindow::mousePressEvent(QMouseEvent *evt)
{
    Qt::MouseButtons buttons = evt->buttons();

    if (buttons & (Qt::LeftButton | Qt::RightButton) )
    {
        // track the mouse
        _dragStart = evt->globalPos();
    }

    QWindow::mousePressEvent(evt);
}

void
MachineWindow::mouseMoveEvent(QMouseEvent *evt)
{
    Qt::MouseButtons buttons = evt->buttons();

    if (buttons & Qt::LeftButton)
    {
        // track the mouse
        QPoint offset = _dragStart - evt->globalPos();
        _dragStart = evt->globalPos();
        // Add this many degrees of rotation to X and Y of the view matrix
        _matModel.rotate((float)offset.x() / 2.0f, 0, 1, 0);
        _matModel.rotate((float)offset.y() / 2.0f, 1, 0, 0);

        //qDebug() << offset << _matModel;

        render();
    }
    else if (buttons & Qt::RightButton)
    {
        QPoint offset = _dragStart - evt->globalPos();
        _dragStart = evt->globalPos();
        // Add this many degrees of rotation to X and Y of the view matrix
        _matModel.translate((float)-offset.x(), (float)offset.y(), 0);

        //qDebug() << offset << _matModel;

        render();
    }
    QWindow::mouseMoveEvent(evt);
}

void
MachineWindow::initialize()
{
    ///////////////////////////
    // Setup logging
    _logger = new QOpenGLDebugLogger(this);
    connect(_logger, SIGNAL( messageLogged(QOpenGLDebugMessage)), this, SLOT( onDebugMsg(QOpenGLDebugMessage) ), Qt::DirectConnection );

    if (_logger->initialize())
    {
        _logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        _logger->enableMessages();
    }
    else
    {
        qWarning() << "Unable to setup GL debug logging";
        if (!_context->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            qWarning() << "The GL context doesn't have the right extension GL_KHR_debug";
        }
        else
        {
            qWarning() << "The extension is present. Maybe the context isn't setup for debug format?";
        }
    }


    // Setup our shaders
    _shaderProg.addShaderFromSourceFile(QOpenGLShader::Vertex, QString(":/gl/default.vert"));
    _shaderProg.addShaderFromSourceFile(QOpenGLShader::Fragment, QString(":/gl/default.frag"));
    if (!_shaderProg.link())
    {
        qDebug() << "Failed to link GL Shaders";
        qDebug() << _shaderProg.log();
    }
    else
    {
        qDebug() << "Shaders loaded ok";
        _hasInitialized = true;

        emit readyForData();
        render();
    }
}

void
MachineWindow::render()
{
    if (!_hasInitialized || !isExposed()) return;

    if (!_context->makeCurrent(this))
    {
        qDebug() << "Failed to make context current";
    }
    _gl->glClearColor(1.0, 0.0, 0.0, 1.0);
    _gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shaderProg.bind();

    QMatrix4x4 mvt = ((_matProject * _matView) * _matModel);

    _shaderProg.setUniformValue("ModelViewProject", mvt);

    if (_vao)
    {
        _vao->bind();
        _gl->glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    }

    _context->swapBuffers(this);
}

void
MachineWindow::render(QPainter* painter)
{
    // Nothing of interest
    Q_UNUSED(painter);
}


void
MachineWindow::setProgram(QList<Word*>& prog)
{
    // Store this program. Shallow copy so be careful I guess...
    _program.clear();
    _program.append(prog);

    // Reset the visual data
//    GLfloat  posData[6][3] = {
//            { -0.90, -0.90, -0.5 },  // Triangle 1
//            {  0.85, -0.90, -0.5 },
//            { -0.90,  0.90, -0.5 },
//            {  0.90, -0.85, 0.5 },  // Triangle 2
//            {  0.90,  0.90, 0.5 },
//            { -0.90,  0.90, 0.5 }
////        { -0.85,  0.90, 0.5 }
//        };

    GLfloat  posData[6][3] = {
            { -90, -90, -10.0 },  // Triangle 1
            {  85, -90, -10.0 },
            { -90,  85, -10.0 },
            {  90, -85, -5 },  // Triangle 2
            {  90,  90, -5 },
            { -85,  90, -5 }
        };

    if (_vao)
    {
        qWarning("_vao already exists and I don't know what to do in this case yet");
        _vao->bind();
    }
    else
    {
        _context->makeCurrent( this );

        _vao = new QOpenGLVertexArrayObject( this );
        _vao->create();
        _vao->bind();

        _positions.create();
        _positions.setUsagePattern(QOpenGLBuffer::StaticDraw);
        _positions.bind();
        _positions.allocate(posData, NumVertices * 3 * sizeof(GLfloat));

        _shaderProg.enableAttributeArray("Vertex");
        _shaderProg.setAttributeBuffer("Vertex", GL_FLOAT, 0, 3);

        // color buffer....

        // Initial view position
        _matView.setToIdentity();
        _matView.translate(0, 0, 500);
        //_matView.rotate(10, 0, 0, 1);
        // Inverting it here at the end lets us express the move in regular coords without minuses and shit
        _matView = _matView.inverted();


        // Initial model position
        _matModel.setToIdentity();

        // Ok, make with rendering. Hopefully on a good thread and shit...
        render();
    }

}


void
MachineWindow::onDebugMsg(QOpenGLDebugMessage msg)
{
    qDebug() << msg;
}
