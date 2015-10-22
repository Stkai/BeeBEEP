

Build QT for windows:

1. Download and extract qt source to this path:
C:\Qt\5.5\qt-everywhere-opensource-src-5.5.x

2. Change Environment variables like this:
PATH = %PATH%;C:\Qt\Tools\QtCreator\bin

3. Launch
C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat

4. Create folder C:\Qt\5.5\msvc2010_static (if you are not building using msvc2010 then create a folder according to the compiler you are using, like msvc2013 or msvc2008 etc.)

5. goto C:\Qt\5.5\qt-everywhere-opensource-src-5.5.x\qtbase\mkspecs\common
and change msvc-desktop.conf like this (change all MD to MT to remove dependency on msvc dlls)

initial values:
QMAKE_CFLAGS_RELEASE = -O2 -MD
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -O2 -MD -Zi
QMAKE_CFLAGS_DEBUG = -Zi -MDd

should be changed to:
QMAKE_CFLAGS_RELEASE = -O2 -MT
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -O2 -MT -Zi
QMAKE_CFLAGS_DEBUG = -Zi -MTd

5. Using MSVC2010 Command Line tools, running as admin, Configure using following command: (first change directory to here using “cd” command : C:\Qt\5.5\qt-everywhere-opensource-src-5.5.0)
(No need to remind that you need to use the Command Line Tools for your own compiler, like MSVC2013 or 2005 or 2008, if you are not using MSVC2010)

configure.bat -static -debug-and-release -prefix "C:\Qt\5.5\msvc2010_static" -platform win32-msvc2010 -I "E:\SOURCES\beebeep-build\icu\include\" -L "E:\SOURCES\beebeep-build\icu\lib" -icu -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-freetype -no-opengl -no-openssl -opensource -confirm-license -make libs -nomake tools -nomake examples -nomake tests

(Notice that the parameter -platform win32-msvc2010 should be set according to the compiler you are using)
(Also note that you can add or remove parts and pieces of the libraries you are building by adding or removing parameters above, for example I always use ODBC so I add -qt-sql-odbc parameter but you might want to remove it if you don’t need it.)

6. Run jom (without any parameters so that the ideal thread count is calculated automatically)
nmake module-qtbase module-qtimageformats
nmake module-qtbase-install_subtargets module-qtimageformats-install_subtargets

7. run jom install

