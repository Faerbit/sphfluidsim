#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>

class Camera {
public:
    Camera();
    Camera(QVector3D startPos, QVector3D startOri);
    void setPosition(QVector3D position);
    void moveForward(float distance);
    void moveBack(float distance);
    void moveLeft(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void pan(float xAngle, float yAngle);
    void reset();
    QMatrix4x4 viewMatrix();
    QVector3D getPosition();
private:
    QVector3D position;
    QVector3D orientation;
    QVector3D resetPosition;
    QVector3D resetOrientation;
    float yAngle;
};

#endif // CAMERA_H
