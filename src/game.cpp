#include "game.h"

Game::Game()
{
    InitWindow(WIDTH, HEIGHT, TITLE.c_str());
    InitAudioDevice();
    #if defined(PLATFORM_DEKSTOP)
        SetTargetFPS(FPS);
    #endif
    state = 0;
    std::cout<<"[Init] Done!\n";
}

void Game::clean()
{
    CloseWindow();
}

#if defined(PLATFORM_WEB)
void webRun(Game &game)
{
    switch(game.state)
    {
        case 0:
            game.menu();
            break;
        case 1:
            break;
        default:
            break;
    }
}
#endif

void Game::menu()
{
    //render
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

    EndDrawing();
}

Game::~Game()
{
    clean();
}