@echo off

pushd build
echo Compiling...
cl.exe /I../include /Zi /O2 /MD /EHsc /nologo /TC /FS -c ../src/main.c
echo Linking...
link.exe /DEBUG /LIBPATH:../lib/ /nologo raylib.lib gdi32.lib user32.lib shell32.lib winmm.lib /OUT:CircleZoo.exe main.obj
echo Done.
popd