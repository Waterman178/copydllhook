@echo off 
set one=%1
call "D:\VS2017\VC\Auxiliary\Build\vcvarsall.bat" %one%
if "%one%"=="" (echo null) else cd OutGoingFileTool\OutGoingFileTool 
rc.exe /n /D "NDEBUG" /D "_USING_V110_SDK71_" /l 0x0804 /nologo /fo"outdoorsTool.res" OutGoingFileTool.rc 
set compilerflags=/nologo /Od /EHsc /Fo"Release\\"
set linkerflags=/OUT:"..\\..\\bin\\outdoorsTool.exe"
cl.exe  /GS- /GL /analyze- /W3 /Gy /Zc:wchar_t /Zi /Gm- /Od /sdl-  /Zc:inline /fp:precise /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_USING_V110_SDK71_" /D "_MBCS" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /MT /FC  /EHsc /nologo   /diagnostics:classic  %compilerflags% *.cpp /link outdoorsTool.res %linkerflags% /SUBSYSTEM:WINDOWS",5.01" /TLBID:1 