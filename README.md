PMPD Physical Modelling for PureData
====================================

`pmpd` is a collection of objects for pd.  These objects provide real-time  simulations, specially 
physical behaviors. `pmpd`  can be used to create natural dynamic systems, like a bouncing ball, 
string movement, Brownian movement, chaos,  fluid dynamics, sand, gravitation, and more. It can 
also be used to create displacements  thus allowing a completely dynamic approach of pd 
computing. 

With `pmpd`  physical dynamics can be modelled without knowing the global equation of the 
movement. Only the cause of the movement and the involved structure are needed for the 
simulation. `pmpd`  provides the basic objects for this kind of simulation. Assembling them allows the 
creation of a very large variety of dynamic systems . 

These object are designed to be used with pd (pd). pd is a real time graphical programming 
environment dedicated for audio signal processing. Pd allow the creation of object and it is well 
adapted of the creation of particular physical modelling. GEM is a pd library dedicated to images 
processing [14]. In the provided `pmpd`  examples, GEM is used for the movement visualisation. 

Simulation can run in a 1 dimentional space, 2d or 3d, depending of the objects used.

Mass, link and interactors objects are design to interact together.
pmpd, pmpd2d and pmpd3d are design to run a simulation as a single object.
pmpd~, pmpd2d~ and pmpd3d~ are design for audio synthesis 

Cyrille Henry 2003-2024

Building instruction
====================

To build you need `cmake` and a compiling toolchain. Then run those steps from the repo folder : 

    cmake -S . -B build 
    cmake --build build
    cmake --build build --target install

Then you'll find the `pmpd` package folder under `build/package/pmpd`.

