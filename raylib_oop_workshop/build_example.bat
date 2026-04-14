@echo off

g++ .\getting_started_with_raylib.cpp -o getting_started_with_raylib.exe -I libs/raylib/include -L libs/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++20 -O0 -g

if %ERRORLEVEL% EQU 0 (
    getting_started_with_raylib.exe
) else (
    echo Build failed!
)