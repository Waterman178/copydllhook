@echo off
set one=%1 
call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
set compilerflags=/Od /EHsc /Fo"Release\\" /D x64
set linkerflags= /OUT:"..\\bin\\OurDoorToolx64.exe" /MACHINE:X64
if "%one%"=="" (echo null) else cd OurDoorToolx64
cl.exe %compilerflags% *.cpp /link %linkerflags%