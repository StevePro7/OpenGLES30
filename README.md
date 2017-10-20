# OpenGLES30


https://stackoverflow.com/questions/26279291/what-causes-linker-warning-msil-netmodule-or-module-compiled-with-gl-found-r
To fix, open project settings of all dependency projects and check setting:

Configuration Properties > C/C++ > Optimization > Whole Program Optimization

Make sure it is set to "No" everywhere.