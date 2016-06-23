#ifndef CONSTANTS_H
#define CONSTANTS_H

const std::string DATA_PATH = "../data/";
const std::string MESH_PATH = DATA_PATH + "mesh/";
const std::string SHADER_PATH = DATA_PATH + "shader/";
const std::string TEXTURE_PATH = DATA_PATH + "textures/";

const float MOUSE_SENSITIVITY = 10.0f;
const float MOVEMENT_SPEED = 0.5f;

// one space unit represents 1 AU
// one second represents one day
// tilt is represented in degrees
// data is taken from Wikipedia

const double DISTANCE_CIRCUMFERENCE_SCALE = 30.0f;
const double PLANET_SUN_SCALE = 10.0f;
const double MOON_EARTH_DISTANCE_SCALE = 15.0f;
const double MOON_MARS_DISTANCE_SCALE = 500.0f;
const double KM_TO_AU = 1.0f / 149'597'870.7f;

const double SUN_DISTANCE = 0.0f;
const double SUN_CIRCUMFERENCE = 1'392'684.0f 
    * KM_TO_AU * DISTANCE_CIRCUMFERENCE_SCALE;
const double SUN_ORBIT_TIME = 0.0f;
const double SUN_ROTATION_TIME = 25.38f;
const double SUN_AXIS_TILT = 7.25f;


// mercury
const double MERCURY_DISTANCE = 0.38709893f;
const double MERCURY_CIRCUMFERENCE = 4879.32f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double MERCURY_ORBIT_TIME = 87.969f;
const double MERCURY_ROTATION_TIME = 58.625f;
const double MERCURY_AXIS_TILT = 0.01f;

// venus
const double VENUS_DISTANCE = 0.723f;
const double VENUS_CIRCUMFERENCE = 12'103.6f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double VENUS_ORBIT_TIME = 224.701;
const double VENUS_ROTATION_TIME = 243.018f;
const double VENUS_AXIS_TILT = 177.36f;

// earth
const double EARTH_DISTANCE = 1.0f;
const double EARTH_CIRCUMFERENCE = 12'756.32f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double EARTH_ORBIT_TIME = 365.256f;
const double EARTH_ROTATION_TIME = 1.0f;
const double EARTH_AXIS_TILT = 23.44f;

// moon
const double MOON_DISTANCE = 384'400.0f * KM_TO_AU * MOON_EARTH_DISTANCE_SCALE;
const double MOON_CIRCUMFERENCE = 3476.0f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double MOON_ORBIT_TIME = 27.3217f;
const double MOON_ROTATION_TIME = 27.322f;
const double MOON_AXIS_TILT = 6.68f;

// mars
const double MARS_DISTANCE = 1.524f;
const double MARS_CIRCUMFERENCE = 6'792.4f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double MARS_ORBIT_TIME = 686.98f;
const double MARS_ROTATION_TIME = 1.62277777777f;
const double MARS_AXIS_TILT = 25.19f;

// phobos
const double PHOBOS_DISTANCE = 9'378.0f * KM_TO_AU * MOON_MARS_DISTANCE_SCALE;
const double PHOBOS_CIRCUMFERENCE = 25.0f * 100.0f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double PHOBOS_ORBIT_TIME = 0.3189f;
const double PHOBOS_ROTATION_TIME = 0.3189f;
const double PHOBOS_AXIS_TILT = 0.0f;

// deimos
const double DEIMOS_DISTANCE = 23'459.0f * KM_TO_AU * MOON_MARS_DISTANCE_SCALE;
const double DEIMOS_CIRCUMFERENCE = 15.0f * 100.0f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double DEIMOS_ORBIT_TIME = 1.2624f;
const double DEIMOS_ROTATION_TIME = 1.262f;
const double DEIMOS_AXIS_TILT = 0.0f;

// jupiter
const double JUPITER_DISTANCE = 5.203f;
const double JUPITER_CIRCUMFERENCE = 142'984.4f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double JUPITER_ORBIT_TIME = 4'330.0f;
const double JUPITER_ROTATION_TIME = 0.41354f;
const double JUPITER_AXIS_TILT = 3.13f;

// saturn
const double SATURN_DISTANCE = 9.5826f;
const double SATURN_CIRCUMFERENCE = 120'536.0f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double SATURN_ORBIT_TIME = 10'751.805f;
const double SATURN_ROTATION_TIME = 0.439583333f;
const double SATURN_AXIS_TILT = 26.73f;

// uranus
const double URANUS_DISTANCE = 19.201f;
const double URANUS_CIRCUMFERENCE = 51'118.0f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double URANUS_ORBIT_TIME = 30'664.015f;
const double URANUS_ROTATION_TIME = 0.718333333f;
const double URANUS_AXIS_TILT = 97.77f;

// neptune
const double NEPTUNE_DISTANCE = 30.047f;
const double NEPTUNE_CIRCUMFERENCE = 49'528.0f
    * KM_TO_AU * PLANET_SUN_SCALE * DISTANCE_CIRCUMFERENCE_SCALE;
const double NEPTUNE_ORBIT_TIME = 60'148.35f;
const double NEPTUNE_ROTATION_TIME = 0.6652662037037037f;
const double NEPTUNE_AXIS_TILT = 28.32f;

#endif // CONSTANTS_H
