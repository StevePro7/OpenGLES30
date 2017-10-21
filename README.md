# OpenGLES30


https://stackoverflow.com/questions/26279291/what-causes-linker-warning-msil-netmodule-or-module-compiled-with-gl-found-r
To fix, open project settings of all dependency projects and check setting:

Configuration Properties > C/C++ > Optimization > Whole Program Optimization

Make sure it is set to "No" everywhere.



Cannot use nullptr in Shared Library (Android, iOS) in Visual Studio 2015
https://stackoverflow.com/questions/37759349/cannot-use-nullptr-in-shared-library-android-ios-in-visual-studio-2015

Android
Native Activity
Properties
C/C++
Language
C++ Language Standard
C++11 (-std=c++11)


git push http://username:password@github.com/StevePro7/OpenGLES30.git