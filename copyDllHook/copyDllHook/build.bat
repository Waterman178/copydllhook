@echo off 
call "D:\VS2017\VC\Auxiliary\Build\vcvarsall.bat" x86 
set compilerflags=/Od /EHsc
set linkerflags=-DLL /OUT:copyDLLHook.dll 
cl.exe %compilerflags% copyDLLHook.cpp  DllMain.cpp Fake_ZwQueryDirectoryFile.cpp    Fake_ZwQueryInformationFile.cpp   Fake_ZwReadFile.cpp     Fake_ZwSection.cpp     Fake_ZwSetInformationFile.cpp   Fake_ZwWriteFile.cpp  GlobalHook.cpp  stdafx.cpp  /link %linkerflags%