#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

#include "shaderprogram.h"
#include "camera.h"
#include "particles.h"
#include "simulation.h"

class GLWidget : public QOpenGLWidget{
    Q_OBJECT
public:
    GLWidget(QWidget *parent);
    ~GLWidget();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void keyPressEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent *event);

private slots:
    void onMessageLogged(QOpenGLDebugMessage msg);
    void _update();

// private variables
private:
    QOpenGLDebugLogger* logger = nullptr;
    ShaderProgram defaultShaderProgram;
    void load();

    QTimer timer;
    TimePoint start;

    Camera camera;

    Simulation simulation;
    Particles particles;

    int width;
    int height;

    float aspect = 0.0f;
};

#endif // GLWIDGET_H
