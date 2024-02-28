PMPD Physical Modelling for Pure Data
=====================================

`pmpd` is a collection of objects for Pd (Pure Data), enabling real-time simulations of physical phenomena. These objects facilitate the creation of dynamic natural systems such as bouncing balls, string oscillations, Brownian motion, chaos theory models, fluid dynamics, sand simulations, gravitational interactions and more. Additionally, `pmpd` allows displacements of physical entities, thus offering a completely dynamic approach of Pd computing.

Utilizing `pmpd`, users can model physical dynamics without the necessity of knowing the comprehensive equation of motions. Simulations require only an understanding of the movement's causality and the structure involved. `pmpd` supplies the foundational objects needed for such simulations and their combination allows the creation of a vast variety of dynamic systems.

These object are designed to be used within Pd, a real-time graphical programming environment dedicated to audio signal processing. Pd facilitates the creation of objects, making it particularly suitable for physical modeling. The GEM library focuses on image processing and is employed in `pmpd` examples for visualizing the behavior of physical models. Simulations can be conducted in 1d, 2d or 3d, depending on the objects utilized.

Mass, link and interactor objects are crafted to function cohesively. 
`pmpd`, `pmpd2d` and `pmpd3d` are designed to run simulations as singular entities,
while `pmpd~`, `pmpd2d~` and `pmpd3d~` are intended for audio synthesis applications.

Cyrille Henry 2003-2024

Build instructions
==================

To build, you need `cmake` and a compiler toolchain. Execute the following steps from within the repository folder:

    cmake -S . -B build 
    cmake --build build
    cmake --build build --target install

You'll find the `pmpd` package folder under `build/package/pmpd`.
