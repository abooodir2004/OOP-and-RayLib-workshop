#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "shapes.h"
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

std::shared_ptr<Level> CreateLevel1(std::shared_ptr<Camera2D> camera)
{
    auto level = std::make_shared<Level>(camera, Vector2{0, 20});
    level->level_entities_ = {
        // Ground
        std::make_shared<Platform>(
            std::make_shared<Rect>(-100, 200, 0, 0, 400, 40, GREEN, false)),
        // Easy jumps
        std::make_shared<Platform>(std::make_shared<Rect>(
            350, 150, 0, 0, 200, 40, GREEN, false)), // first enemy
        std::make_shared<Platform>(
            std::make_shared<Rect>(600, 200, 0, 0, 400, 40, GREEN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(1100, 150, 0, 0, 200, 40, GREEN, false)),

        // Win platform
        std::make_shared<WinPlatform>(
            std::make_shared<Rect>(1400, 150, 0, 0, 100, 40, GOLD, false)),

        // Easy enemies
        std::make_shared<Enemy>(
            std::make_shared<Circle>(450, -100, 0, 0, 20, BLUE, false), 50),
        std::make_shared<Enemy>(
            std::make_shared<Circle>(750, -150, 0, 0, 20, BLUE, false), 150),

        // Coins
        std::make_shared<Coin>(
            std::make_shared<Circle>(400, 100, 0, 0, 10, GOLD, false)),
        std::make_shared<Coin>(
            std::make_shared<Circle>(700, 150, 0, 0, 10, GOLD, false)),
        std::make_shared<Coin>(
            std::make_shared<Circle>(1200, 100, 0, 0, 10, GOLD, false))};

    return level;
}

std::shared_ptr<Level> CreateLevel2(std::shared_ptr<Camera2D> camera)
{
    // Verticality focus
    auto level = std::make_shared<Level>(camera, Vector2{0, 0});
    level->level_entities_ = {
        // Starting platform
        std::make_shared<Platform>(
            std::make_shared<Rect>(-100, 100, 0, 0, 300, 40, DARKBROWN, false)),

        // Steps going up
        std::make_shared<Platform>(
            std::make_shared<Rect>(250, 50, 0, 0, 150, 30, BROWN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(450, -50, 0, 0, 150, 30, BROWN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(650, -150, 0, 0, 150, 30, BROWN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(900, -150, 0, 0, 400, 30, DARKBROWN, false)),

        // Win platform
        std::make_shared<WinPlatform>(
            std::make_shared<Rect>(1400, -200, 0, 0, 100, 40, GOLD, false)),

        // Enemies on the steps
        std::make_shared<Enemy>(
            std::make_shared<Circle>(500, -100, 0, 0, 15, RED, false), 50),
        std::make_shared<Enemy>(
            std::make_shared<Circle>(1100, -200, 0, 0, 20, PURPLE, false), 150),

        // Coins
        std::make_shared<Coin>(
            std::make_shared<Circle>(300, 0, 0, 0, 10, GOLD, false)),
        std::make_shared<Coin>(
            std::make_shared<Circle>(700, -200, 0, 0, 10, GOLD, false)),
        std::make_shared<Coin>(
            std::make_shared<Circle>(1000, -200, 0, 0, 10, GOLD, false))};

    return level;
}

std::shared_ptr<Level> CreateLevel3(std::shared_ptr<Camera2D> camera)
{
    // Hard jumps and enemies
    auto level = std::make_shared<Level>(camera, Vector2{0, 0});
    level->level_entities_ = {
        // Starting platform
        std::make_shared<Platform>(
            std::make_shared<Rect>(-100, 100, 0, 0, 200, 40, DARKGRAY, false)),

        // Long jumps
        std::make_shared<Platform>(
            std::make_shared<Rect>(250, 100, 0, 0, 150, 20, GRAY, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(500, 100, 0, 0, 150, 20, GRAY, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(750, 0, 0, 0, 150, 20, GRAY, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(1000, -100, 0, 0, 100, 20, GRAY, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(1300, -100, 0, 0, 300, 40, DARKGRAY, false)),

        // Win platform
        std::make_shared<WinPlatform>(
            std::make_shared<Rect>(1700, -150, 0, 0, 100, 40, GOLD, false)),

        // Tricky enemies
        std::make_shared<Enemy>(
            std::make_shared<Circle>(550, 50, 0, 0, 15, ORANGE, false), 45),
        std::make_shared<Enemy>(
            std::make_shared<Circle>(800, -50, 0, 0, 15, ORANGE, false), 45),
        std::make_shared<Enemy>(
            std::make_shared<Circle>(1400, -150, 0, 0, 25, RED, false), 100),

        // Coins
        std::make_shared<Coin>(
            std::make_shared<Circle>(300, 50, 0, 0, 10, GOLD, false)),
        std::make_shared<Coin>(
            std::make_shared<Circle>(800, -30, 0, 0, 10, GOLD, false)),
        std::make_shared<Coin>(
            std::make_shared<Circle>(1500, -200, 0, 0, 10, GOLD, false))};

    return level;
}

std::shared_ptr<Level> CreateLevel4(std::shared_ptr<Camera2D> camera)
{
    // Mixed challenge
    auto level = std::make_shared<Level>(camera, Vector2{0, 0});
    level->level_entities_ = {
        // Starting platform
        std::make_shared<Platform>(
            std::make_shared<Rect>(-100, 150, 0, 0, 300, 40, DARKGREEN, false)),

        // Mixed platforms
        std::make_shared<Platform>(
            std::make_shared<Rect>(250, 100, 0, 0, 150, 20, GREEN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(500, 200, 0, 0, 150, 20, GREEN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(750, 50, 0, 0, 150, 20, GREEN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(1000, -50, 0, 0, 200, 20, GREEN, false)),
        std::make_shared<Platform>(
            std::make_shared<Rect>(1300, -50, 0, 0, 300, 40, DARKGREEN,
                                   false)),

        // Win platform
        std::make_shared<WinPlatform>(
            std::make_shared<Rect>(1700, -100, 0, 0, 100, 40, GOLD, false)),

        // Enemies
        std::make_shared<Enemy>(
            std::make_shared<Circle>(560, 150, 0, 0, 15, MAROON,
                                     false), // maroon enemy
            60),
        std::make_shared<Enemy>(
            std::make_shared<Circle>(807, 0, 0, 0, 10, MAROON,
                                     false), // maroon enemy
            60),
        std::make_shared<Enemy>(
            std::make_shared<Circle>(1400, -100, 0, 0, 25, RED,
                                     false), // red enemy
            100),

        // Coins
        std::make_shared<Coin>(
            std::make_shared<Circle>(300, 80, 0, 0, 10, GOLD,
                                     false)), //

        std::make_shared<Coin>(
            std::make_shared<Circle>(800, 20, 0, 0, 10, GOLD,
                                     false)), //
    };

    return level;
}

int main()
{

    // initialize the window
    InitWindow(800, 600, "My Game");
    SetTargetFPS(60);

    //--------------------------------------------------------------------
    // initalize a 2D camera
    //--------------------------------------------------------------------
    std::shared_ptr<Camera2D> camera = std::make_shared<Camera2D>();
    *camera = {0};
    camera->target = {0, 0};
    camera->offset = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
    camera->rotation = 0;
    camera->zoom = 1;

    // Load Levels
    std::vector<std::shared_ptr<Level>> levels;
    levels.push_back(CreateLevel1(camera));
    levels.push_back(CreateLevel2(camera));
    levels.push_back(CreateLevel3(camera));
    levels.push_back(CreateLevel4(camera));

    // create the player and its shape
    std::shared_ptr<Circle> player_shape =
        std::make_shared<Circle>(300, 100, 0, 0, 20, RED, false);
    std::shared_ptr<Player> player = std::make_shared<Player>(player_shape);

    // Initialize Game with levels
    Game game(player, levels);

    game.StartGame();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        game.Update();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}