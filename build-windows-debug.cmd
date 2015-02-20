set BUILD_DIR=../build-sqlitewrapper-debug
set CMAKE_BUILD_TYPE=Debug

for /f %%c in ('wmic cpu get NumberOfCores ^| findstr [0-9]') do set "CORES=%%c"
set SOURCE_DIR=%CD%
set CMAKE_COMMAND=cmake.exe
set MAKE_COMMAND=C:\Qt-Installation\Tools\QtCreator\bin\jom.exe /J %CORES%

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"

python projectfiles_unchanged.py
if %ERRORLEVEL% NEQ 0 (
  RMDIR /S /Q "%BUILD_DIR%"
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%" || goto error
%CMAKE_COMMAND% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% -G "NMake Makefiles" "%SOURCE_DIR%" || goto error
%MAKE_COMMAND% || goto error

cd "%SOURCE_DIR%"
goto :EOF

:error
cd "%SOURCE_DIR%"
exit /b 1
