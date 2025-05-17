@echo off
cd /D "%~dp0"
set src_dir=src

set "compile_msvc=cl.exe /I../include /Zi /Od /MD /EHsc /nologo /TC /FS -c"
set "link_msvc=link.exe /LIBPATH:../lib/ /nologo raylib.lib opengl32.lib gdi32.lib user32.lib shell32.lib winmm.lib kernel32.lib /OUT:LineZoo.exe main.obj"

pushd build
echo Compiling...
%compile_msvc% ../src/main.c
echo Linking...
%link_msvc%
popd