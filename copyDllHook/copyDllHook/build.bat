@echo off 
set one=%1
call "D:\VS2017\VC\Auxiliary\Build\vcvarsall.bat" %one%
set compilerflags=/Od /EHsc /Fo"Release\\"
set linkerflags=-DLL /OUT:"..\\..\\bin\\copyDllHook%1.dll"
if "%one%"=="" (echo null) else cd copyDllHook\copyDllHook
cl.exe %compilerflags% *.cpp /link %linkerflags%