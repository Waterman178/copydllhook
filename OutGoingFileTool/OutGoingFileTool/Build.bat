@echo off 
set one=%1
call "D:\VS2017\VC\Auxiliary\Build\vcvarsall.bat" %one%
if "%one%"=="" (echo null) else cd OutGoingFileTool\OutGoingFileTool 
rc.exe /n /fo"outdoorsTool.res" OutGoingFileTool.rc
set compilerflags=/nologo /Od /EHsc /Fo"Release\\"
set linkerflags=/OUT:"..\\..\\bin\\outdoorsTool.exe"
cl.exe %compilerflags% *.cpp /link outdoorsTool.res %linkerflags% /SUBSYSTEM:WINDOWS",5.01" /TLBID:1 