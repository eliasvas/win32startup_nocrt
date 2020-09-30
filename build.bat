@echo off
where /q cl
if ERRORLEVEL 1 (
    call  "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)


set application_name=game.exe
set build_options= -DBUILD_DEVELOPER=1 -DBUILD_DEBUG=1 -DBUILD_WIN32=1 -DBUILD_CRTLESS=1
set compile_flags= -nologo -GR /W0 /DEBUG /Zi -EHa- -Oi -GS- -Gs9999999 -I../src
set link_flags= -incremental:no -opt:ref kernel32.lib gdi32.lib opengl32.lib user32.lib dsound.lib dxguid.lib winmm.lib

if not exist build mkdir build
pushd build
cl.exe %compile_flags% %build_options% /Tc ../src/win32_main.c -link -nodefaultlib -subsystem:windows -stack:0x100000,0x100000 %link_flags%
popd