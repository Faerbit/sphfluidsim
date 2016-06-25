#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QOpenGLFunctions_4_3_Core>

#include "shaderprogram.h"
#include "camera.h"
#include "particles.h"

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

private slots:
    void onMessageLogged(QOpenGLDebugMessage msg);
    void _update();

// private methods
private:
    void loadSolarSystem(std::string meshFilePath);
    void reloadPlanets();


// private variables
private:
    QOpenGLDebugLogger* logger = nullptr;
    ShaderProgram defaultShaderProgram;
    void load();

    QTimer timer;
    TimePoint start;

    Camera camera;

    Particles particles;

    std::shared_ptr<QOpenGLFunctions_4_3_Core> funcs;

    int width;
    int height;

    float aspect = 0.0f;
};

#endif // GLWIDGET_H
