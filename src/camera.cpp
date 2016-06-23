#include "camera.h"
#include <QVector4D>

Camera::Camera(QVector3D startPos, QVector3D startOri) {
    resetPosition = startPos;
    position = startPos;
    resetOrientation = startOri.normalized();
    orientation = startOri.normalized();
    yAngle = 0.0f;
}

Camera::Camera() : Camera(QVector3D(), QVector3D(0.0f, 0.0f, -1.0f)) {}

QMatrix4x4 Camera::viewMatrix() {
    QMatrix4x4 ret;
    ret.setToIdentity();
    ret.lookAt(position, position + orientation, QVector3D(0.0f, 1.0f, 0.0f));
    return ret;
}

void Camera::moveForward(float distance) {
    position += orientation * distance;
}

void Camera::moveBack(float distance) {
    position -= orientation * distance;
}

void Camera::moveLeft(float distance) {
    QVector3D leftVector = QVector3D::crossProduct(QVector3D(0.0f, 1.0f, 0.0f),
            orientation);
    position += leftVector.normalized() * distance;
}

void Camera::moveRight(float distance) {
    QVector3D rightVector = QVector3D::crossProduct(orientation,
            QVector3D(0.0f, 1.0f, 0.0f));
    position += rightVector.normalized() * distance;
}

void Camera::moveUp(float distance) {
    position += QVector3D(0.0f, 1.0f, 0.0f) * distance;
}

void Camera::pan(float xAngle, float yAngle) {
    // prevent gimbal lock
    if (this->yAngle + yAngle > 89.0f || this->yAngle + yAngle < -89.0f) {
        yAngle = 0.0f;
    }
    else {
        this->yAngle += yAngle;
    }
    QMatrix4x4 rotMat;
    rotMat.setToIdentity();
    rotMat.rotate(xAngle, 0.0f, 1.0f, 0.0f);
    rotMat.rotate(yAngle, 1.0f, 0.0f, 0.0f);
    orientation = QVector3D(rotMat * QVector4D(orientation)).normalized();
}

void Camera::reset() {
    position = resetPosition;
    orientation = resetOrientation;
}
