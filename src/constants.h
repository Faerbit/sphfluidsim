#ifndef CONSTANTS_H
#define CONSTANTS_H

const std::string DATA_PATH = "../data/";
const std::string MESH_PATH = DATA_PATH + "mesh/";
const std::string SHADER_PATH = DATA_PATH + "shader/";
const std::string TEXTURE_PATH = DATA_PATH + "textures/";

const float MOUSE_SENSITIVITY = 10.0f;
const float MOVEMENT_SPEED = 0.5f;

// numerical stuff

const int CMP_THREAD_SIZE = 32;

const float INTERACTION_RADIUS = 1.0f;
//const float INTERACTION_RADIUS = 0.5f;
const int MAX_NO_INTERACT_PARTICLES = 32;

const float PARTICLE_MASS = 1.0f;
const float GRAVITY = 9.8;

// properties for water at 20 degrees celsius at 1 atm pressure
const float DYNAMIC_VISCOSITY = 1.002f;
const float DENSITY = 998.2071f;
const float KINEMATIC_VISCOSITY = DYNAMIC_VISCOSITY/DENSITY;
//const float KINEMATIC_VISCOSITY = 0.018;

#endif // CONSTANTS_H
