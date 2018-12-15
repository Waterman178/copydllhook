@echo off 
call "D:\VS2017\VC\Auxiliary\Build\vcvarsall.bat" x86 
set compilerflags=/Od /EHsc /Fo"Release\\"
set linkerflags=-DLL /OUT:"bin\\copyDllHook.dll"
cl.exe %compilerflags% *.cpp /link %linkerflags%