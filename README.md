# Soleil

1. This project is compiled by gcc-11 and Qt5.15.2, supported by C++ 20.
2. There is an error when compiling with gcc-11 and higher version in "qtconcurrentthreadengine.h" which is Qt file. The solution to solve it is to remove the template parameter in constructor, see detail in https://github.com/qt/qtbase/commit/659f7a06e91c04b239e3f4c0bcfccbe3581af1c3
