#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

#include "shaderprogram.h"
#include "camera.h"
#include "planet.h"
    
using planet = std::shared_ptr<Planet>;

class GLWidget : public QOpenGLWidget {
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
    ShaderProgram sunShaderProgram;
    ShaderProgram normalShaderProgram;
    ShaderProgram noTextureShaderProgram;

    QTimer timer;
    TimePoint start;

    Camera camera;

    planet sun;
    std::vector<planet> planets;

    int width;
    int height;

    float aspect = 0.0f;

    bool paintNormals;
    bool renderBunnies;
};

#endif // GLWIDGET_H
