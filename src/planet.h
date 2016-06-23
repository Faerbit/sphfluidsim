#ifndef PLANET_H
#define PLANET_H

#include "object.h"
#include <chrono>

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Time = std::chrono::duration<float>;

class Planet : public Object {
public:
    Planet();
    Planet(Object object, float distance, float scale, float rotationPeriod, 
            float orbitalPeriod, float axisTilt);
    Planet(std::string meshFilePath, std::string textureFilePath,
            float distance, float scale, float rotationPeriod,
            float orbitalPeriod, float axisTilt);
    Planet(std::string textureFilePath, float distance, float scale,
            float rotationPeriod, float orbitalPeriod, float axisTilt);
    void update(Time time);
private:
    float distance;
    float rotationPeriod;
    float orbitalPeriod;
    float axisTilt;
    QVector3D rotationAxis;
};

#endif // PLANET_H
