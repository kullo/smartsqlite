set SOURCE_DIR=%CD%
set BUILD_DIR=build-sqlitewrapper
set CMAKE=cmake.exe
set MAKE=mingw32-make.exe -j2

cd ..
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%" || goto error
%CMAKE%  -G "MinGW Makefiles" "%SOURCE_DIR%" || goto error
%MAKE% || goto error

cd "%SOURCE_DIR%"
exit /b 0

:error
cd "%SOURCE_DIR%"
exit /b 1
