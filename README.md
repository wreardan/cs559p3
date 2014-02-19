<h1>Graphics Project 3</h1>
(c) 2013 Wesley Reardan and Dustin Greene

We created a simulation of the Solar System with splines surrounding each of the planets.

This application was developed using Visual Studio and C++, but should be portable to other platforms with a little effort.

If you enter Mars Mode (m), you will see a Spline that is being generated in real-time using NVIDIA CUDA.  CUDA is also used to generate all the geometry, compute normals, create the wireframe mode, as well as the normal visualization.

The bulk of the code is in the cs559p3/cs559p3 directory, and the shaders are in the cs559p3/cs559p3/shaders directory.
