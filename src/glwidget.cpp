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

    // enable mouse tracking
    setProperty("mouseTracking", true);
    setCursor(Qt::BlankCursor);

    connect(&timer, SIGNAL(timeout()), this, SLOT(_update()),
            Qt::DirectConnection);

    // as soon as possible
    timer.setInterval(0);
    timer.start();
    start = Clock::now();
    paintNormals = false;
    renderBunnies = false;
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    defaultShaderProgram = ShaderProgram("default.vert", "default.frag",
            "vert", "normal", "texCoord", "texture", "mvpMatrix",
            "modelMatrix", "normalMatrix");

    sunShaderProgram = ShaderProgram("default.vert", "sun.frag",
            "vert", "normal", "texCoord", "texture", "utilTexture", "time",
            "mvpMatrix", "modelMatrix", "normalMatrix");

    normalShaderProgram = ShaderProgram("default.vert", "normals.frag",
            "vert", "normal", "mvpMatrix", "modelMatrix", "normalMatrix");

    noTextureShaderProgram = ShaderProgram("default.vert", "noTexture.frag",
            "vert", "normal", "mvpMatrix", "modelMatrix", "normalMatrix");

    camera = Camera(QVector3D(0.0f, 30.0f, -30.0f),
                    QVector3D(0.0f, -1.0f, 1.0f));
    loadSolarSystem("sphere_low.obj");
}

void GLWidget::loadSolarSystem(string meshFilePath) {
    // tmp vector to save planets
    vector<planet> _planets = vector<planet>();
    Mesh mesh = Mesh("sphere_low.obj");
    Texture texture = Texture("sun.jpg");
    Texture utilTexture = Texture("noise.png");
    utilTexture.setWrapMode(QOpenGLTexture::Repeat);
    Object sunObject = Object(mesh, texture, utilTexture);
    sun = planet(new Planet(sunObject, SUN_DISTANCE, SUN_CIRCUMFERENCE,
                            SUN_ROTATION_TIME, SUN_ORBIT_TIME, SUN_AXIS_TILT));
    auto mercury = planet(new Planet(
        meshFilePath, "mercury.jpg", MERCURY_DISTANCE, MERCURY_CIRCUMFERENCE,
        MERCURY_ROTATION_TIME, MERCURY_ORBIT_TIME, MERCURY_AXIS_TILT));
    _planets.push_back(mercury);
    auto venus = planet(new Planet(meshFilePath, "venus.jpg", VENUS_DISTANCE,
                                   VENUS_CIRCUMFERENCE, VENUS_ROTATION_TIME,
                                   VENUS_ORBIT_TIME, VENUS_AXIS_TILT));
    _planets.push_back(venus);
    auto earth = planet(new Planet(meshFilePath, "earth.jpg", EARTH_DISTANCE,
                                   EARTH_CIRCUMFERENCE, EARTH_ROTATION_TIME,
                                   EARTH_ORBIT_TIME, EARTH_AXIS_TILT));
    _planets.push_back(earth);
    auto moon =
        planet(new Planet("moon.jpg", MOON_DISTANCE, MOON_CIRCUMFERENCE,
                          MOON_ROTATION_TIME, MOON_ORBIT_TIME, MOON_AXIS_TILT));
    moon->setParent(earth);
    _planets.push_back(moon);
    auto mars = planet(new Planet(meshFilePath, "mars.jpg", MARS_DISTANCE,
                                  MARS_CIRCUMFERENCE, MARS_ROTATION_TIME,
                                  MARS_ORBIT_TIME, MARS_AXIS_TILT));
    _planets.push_back(mars);
    auto phobos = planet(new Planet(meshFilePath, "phobos.jpg", PHOBOS_DISTANCE,
                                    PHOBOS_CIRCUMFERENCE, PHOBOS_ROTATION_TIME,
                                    PHOBOS_ORBIT_TIME, PHOBOS_AXIS_TILT));
    phobos->setParent(mars);
    _planets.push_back(phobos);
    auto deimos = planet(new Planet(meshFilePath, "deimos.jpg", DEIMOS_DISTANCE,
                                    DEIMOS_CIRCUMFERENCE, DEIMOS_ROTATION_TIME,
                                    DEIMOS_ORBIT_TIME, DEIMOS_AXIS_TILT));
    deimos->setParent(mars);
    _planets.push_back(deimos);
    auto jupiter = planet(new Planet(
        meshFilePath, "jupiter.jpg", JUPITER_DISTANCE, JUPITER_CIRCUMFERENCE,
        JUPITER_ROTATION_TIME, JUPITER_ORBIT_TIME, JUPITER_AXIS_TILT));
    _planets.push_back(jupiter);
    auto saturn = planet(new Planet(meshFilePath, "saturn.jpg", SATURN_DISTANCE,
                                    SATURN_CIRCUMFERENCE, SATURN_ROTATION_TIME,
                                    SATURN_ORBIT_TIME, SATURN_AXIS_TILT));
    _planets.push_back(saturn);
    auto uranus = planet(new Planet(meshFilePath, "uranus.jpg", URANUS_DISTANCE,
                                    URANUS_CIRCUMFERENCE, URANUS_ROTATION_TIME,
                                    URANUS_ORBIT_TIME, URANUS_AXIS_TILT));
    _planets.push_back(uranus);
    auto neptune = planet(new Planet(
        meshFilePath, "neptune.jpg", NEPTUNE_DISTANCE, NEPTUNE_CIRCUMFERENCE,
        NEPTUNE_ROTATION_TIME, NEPTUNE_ORBIT_TIME, NEPTUNE_AXIS_TILT));
    _planets.push_back(neptune);
    // only set at the end to help with reusing textures
    planets = _planets;
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
    glViewport(0, 0, width, height);
}

void GLWidget::paintGL() {
    // Clear buffer to set color and alpha
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 vpMatrix;
    vpMatrix.setToIdentity();
    vpMatrix.perspective(60.0f, aspect, 0.1f, 1000.0f);
    vpMatrix = vpMatrix * camera.viewMatrix();

    if (paintNormals) {
        normalShaderProgram.bind();
        sun->render(&normalShaderProgram, vpMatrix, !paintNormals);
        for(auto planet : planets) {
            planet->render(&normalShaderProgram, vpMatrix, !paintNormals);
        }
        normalShaderProgram.release();
    }
    else {
        sunShaderProgram.bind();
        sunShaderProgram.setTime(Clock::now() - start);
        sun->render(&sunShaderProgram, vpMatrix, !paintNormals);
        sunShaderProgram.release();
        ShaderProgram* shaderProgram;
        if (renderBunnies) {
            shaderProgram = &noTextureShaderProgram;
        } else {
            shaderProgram = &defaultShaderProgram;
        }
        shaderProgram->bind();
        for(auto planet : planets) {
            planet->render(shaderProgram, vpMatrix, !renderBunnies);
        }
        shaderProgram->release();
    }
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            qApp->quit();
            break;
        case Qt::Key_W:
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
            break;
        case Qt::Key_Home:
            camera.reset();
            break;
        case Qt::Key_F1:
            paintNormals = !paintNormals;
            break;
        case Qt::Key_F2:
            renderBunnies = !renderBunnies;
            reloadPlanets();
            break;
        default:
            QOpenGLWidget::keyPressEvent(event);
            break;
    }
}

void GLWidget::reloadPlanets() {
    if (renderBunnies) {
        loadSolarSystem("bunny.obj");
    }
    else {
        loadSolarSystem("sphere_low.obj");
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    camera.pan(-(event->x() - width/2.0f)/MOUSE_SENSITIVITY,
            (event->y() - height/2.0f)/MOUSE_SENSITIVITY);
}

void GLWidget::onMessageLogged(QOpenGLDebugMessage msg) {
    // ignore texture beeing inconsistent to base levels
    if (msg.id() != 131204) {
        qDebug() << msg;
    }
}

void GLWidget::_update() {
    QCursor::setPos(mapToGlobal(QPoint(width/2, height/2)));
    sun->update(Clock::now() - start);
    for (auto planet : planets) {
        planet->update(Clock::now() - start);
    }
    paintGL();
    update();
}
