#SPH Fluid Simulation

This is a fluid simulation using the SPH method. It is accelerated with OpenGL
compute shaders. I did this as part of a practical course in "Computergrafik"
at the FH Aachen.

##Build instructions
This is a standard CMake project and builds like all the other ones:
```
mkdir build
cd build
cmake ..
make
```

##Unfinished tasks
These are the tasks I would have liked to finish but couldn't because of time/knowledge constraints.

 * Stabilize numerics: Properly scale all values so that they fit to each other
 * Add possibility to add particles mid way through the simulation
 * Add support for initial velocity of particles
 * Add GUI elements to control various aspects of the simulation
 * Sort on the GPU

##License

This work is MIT licensed. See License.md for details.
