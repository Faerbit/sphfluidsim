#include "glwidget.h"
#include <QApplication>
#include <QMatrix4x4>
#include <QVector3D>
#include <QDebug>
#include <iostream>
#include <QCursor>
#include <QOpenGLTexture>

#include "constants.h"
#include "texture.h"
#include "mesh.h"
#include "glfuncs.h"

using namespace std;

GLWidget::GLWidget(QWidget* parent) :
    timer(parent),
    QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    // init logging
    logger = new QOpenGLDebugLogger(this);

    connect(logger, SIGNAL(messageLogged(QOpenGLDebugMessage)),
            this, SLOT (onMessageLogged(QOpenGLDebugMessage)),
            Qt::DirectConnection);

    /*// enable mouse tracking
    setProperty("mouseTracking", true);
    setCursor(Qt::BlankCursor);*/

    connect(&timer, SIGNAL(timeout()), this, SLOT(_update()),
            Qt::DirectConnection);

    // as soon as possible
    timer.setInterval(0);
    timer.start();
    start = Clock::now();
    lastSimulationTimePoint = start;
}

GLWidget::~GLWidget() {
    if (logger != nullptr) {
        logger->~QOpenGLDebugLogger();
    }
}

void GLWidget::initializeGL () {
    if (logger->initialize() ) {
        logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        logger->enableMessages();
    }
    glFuncs::funcs()->glEnable(GL_DEPTH_TEST);
    glFuncs::funcs()->glEnable(GL_CULL_FACE);
    glFuncs::funcs()->glCullFace(GL_BACK);
    glFuncs::funcs()->glDepthFunc(GL_LEQUAL);
    //glFuncs::funcs()->glEnable(GL_TEXTURE_2D);

    glFuncs::funcs()->glClearDepth(1.0f);
    glFuncs::funcs()->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    defaultShaderProgram = ShaderProgram("noTexture.vert", "noTexture.frag",
            "vert", "normal", "position", "mvpMatrix", "modelMatrix",
            "normalMatrix", "scale", "cameraPos");

    camera = Camera(QVector3D(0.0f, 10.0f, -10.0f),
                    QVector3D(0.0f, -1.0f, 1.0f));
    load();
}

void GLWidget::load() {
    simulation = Simulation(1, 4.0f, 4.0f, 4.0f);
    //simulation = Simulation(10, 10.0f, 10.0f, 10.0f);
    simulation.addFluidCuboid(1.0f,
            2.0f, 2.0f, 2.0f,
            1.0f, 1.0f, 1.0f);
            //2.0f, 1.0f, 5.0f);
    simulation.init();
    particles = Particles("sphere_low.obj",
            simulation.getPositionsBuffer(),
            QVector3D(-5.0f , -5.0f, 0.0f),
            simulation.getParticleCount(),
            simulation.getMinDensity()
            );
}

void GLWidget::resizeGL(int width, int height) {
    // Compute aspect ratio

    // prevent division by zero errors
    if (height == 0) {
        height = 1;
    }
    aspect = (float) width / height;
    this->width = width;
    this->height = height;

    // Set viewport to cover the whole window
    glFuncs::funcs()->glViewport(0, 0, width, height);
}

void GLWidget::paintGL() {
    // Clear buffer to set color and alpha
    glFuncs::funcs()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 vpMatrix;
    vpMatrix.setToIdentity();
    vpMatrix.perspective(60.0f, aspect, 0.1f, 1000.0f);
    vpMatrix = vpMatrix * camera.viewMatrix();

    defaultShaderProgram.bind();
    defaultShaderProgram.setCameraPos(camera.getPosition());
    particles.render(&defaultShaderProgram, vpMatrix);
    defaultShaderProgram.release();
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            qApp->quit();
            break;
        /*case Qt::Key_W:
            camera.moveForward(MOVEMENT_SPEED);
            break;
        case Qt::Key_A:
            camera.moveLeft(MOVEMENT_SPEED);
            break;
        case Qt::Key_S:
            camera.moveBack(MOVEMENT_SPEED);
            break;
        case Qt::Key_D:
            camera.moveRight(MOVEMENT_SPEED);
            break;
        case Qt::Key_Space:
            camera.moveUp(MOVEMENT_SPEED);
            break;*/
        case Qt::Key_Home:
            camera.reset();
            break;
        default:
            QOpenGLWidget::keyPressEvent(event);
            break;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    /*camera.pan(-(event->x() - width/2.0f)/MOUSE_SENSITIVITY,
            (event->y() - height/2.0f)/MOUSE_SENSITIVITY);*/
}

void GLWidget::onMessageLogged(QOpenGLDebugMessage msg) {
    // ignore texture beeing inconsistent to base levels
    if (msg.id() != 131204) {
        qDebug() << msg;
    }
}

void GLWidget::_update() {
    //QCursor::setPos(mapToGlobal(QPoint(width/2, height/2)));
    TimePoint now = Clock::now();
    simulation.simulate(now - lastSimulationTimePoint);
    lastSimulationTimePoint = now;
    Time time = now - start;
    float rotationPeriod = 20.0f;
    float fTime = time.count();
    float angle = 360.0f/rotationPeriod * fTime;
    particles.setRotation_y(angle);
    paintGL();
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        camera.moveForward(2*MOVEMENT_SPEED);
    }
    else {
        camera.moveBack(2*MOVEMENT_SPEED);
    }
}
