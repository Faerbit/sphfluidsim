#include "planet.h"

using namespace std;
using namespace std::chrono;

Planet::Planet() {}

Planet::Planet(Object object, float distance, float scale, float rotationPeriod,
            float orbitalPeriod, float axisTilt) : Object(object) {
   setScale(scale);
   this->distance = distance;
   this->rotationPeriod = rotationPeriod;
   this->orbitalPeriod = orbitalPeriod;
   this->axisTilt = axisTilt;
   QMatrix4x4 rotMat = QMatrix4x4();
   rotMat.rotate(axisTilt, 0.0f, 0.0f, 1.0f);
   rotationAxis = rotMat * QVector3D(0.0f, 1.0f, 0.0f);
   update(Time(0));
}
Planet::Planet(std::string meshFilePath, std::string textureFilePath,
                float distance, float scale, float rotationPeriod,
                float orbitalPeriod, float axisTilt) :
    Planet(Object(meshFilePath, textureFilePath), distance, scale,
        rotationPeriod, orbitalPeriod, axisTilt) {}

Planet::Planet(std::string textureFilePath, float distance, float scale,
            float rotationPeriod, float orbitalPeriod, float axisTilt) :
    Planet("sphere_low.obj", textureFilePath, distance, scale, rotationPeriod,
            orbitalPeriod, axisTilt) {}

void Planet::update(Time time) {
    float fTime = time.count();
    QVector3D distanceVec = QVector3D(distance, 0.0f, 0.0f);
    QMatrix4x4 rotMat = QMatrix4x4();
    if (orbitalPeriod != 0.0f) {
        float rotAngle = 360.0f/orbitalPeriod * fTime;
        rotMat.rotate(rotAngle, 0.0f, 1.0f, 0.0f);
    }
    setPosition(rotMat * distanceVec);
    float orbAngle = 360.0f/rotationPeriod * fTime;
    QQuaternion rot1 =
        QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), axisTilt);
    QQuaternion rot2 = QQuaternion::fromAxisAndAngle(rotationAxis, orbAngle);
    setRotation(rot1*rot2);
}
