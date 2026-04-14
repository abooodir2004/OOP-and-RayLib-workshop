@echo off
g++ .\game.cpp -o game.exe -I libs/raylib/include -L libs/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++20 -O0 -g

if %ERRORLEVEL% EQU 0 (
    game.exe
) else (
    echo Build failed!
)