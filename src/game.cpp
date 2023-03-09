#include "game.h"

Game::Game()
{
    InitWindow(WIDTH, HEIGHT, TITLE.c_str());
    InitAudioDevice();
    #if defined(PLATFORM_DEKSTOP)
        SetTargetFPS(FPS);
    #endif
    state = 0;
    clearMap();
    generateMap();
    printMap();
    loadAssets();
    std::cout<<"[Init] Done!\n";
}

void Game::clearMap()
{
    total_floor = 0;
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            map[i][j] = -1;
        }
    }
}

bool Game::isVaild(int row, int col)
{
    return (row < MAP_WIDTH - 2 && col < MAP_HEIGHT - 2 && map[row][col] == -1);
}

void Game::placeFloor(int row, int col)
{
    if(isVaild(row, col))
    {
        map[row][col] = 0;
        total_floor++;
    }
}

void Game::generateMap()
{
    srand((unsigned)time(nullptr));
    int start_x = 15; int start_y = 15;
    int x = start_x; int y = start_y;
    map[x][y] = 0;
    int dir {};
    while(total_floor < MAX_FLOOR)
    {
        dir = rand() % 4; // 0 - 3
        switch(dir)
        {
            case 0: // north
                x--;
                break;

            case 1: // south
                x++;
                break;

            case 2: // east
                y++;
                break;

            case 3: // west
                y--;
                break;
        }
        placeFloor(x, y);
    }
}

void Game::renderMap()
{
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            if(map[i][j] == 0)
                DrawRectangle(i * 16, j * 16, 16, 16, ORANGE);
        }
    }
}

void Game::printMap()
{
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            std::cout<<map[i][j]<<" ";
        }
        std::cout<<"\n";
    }
}

void Game::loadAssets()
{
    necromancer_tex = LoadTexture("assets/gfx/necro.png");
}

void Game::clean()
{
    UnloadTexture(necromancer_tex);
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

        renderMap();

        DrawTexture(necromancer_tex, 250, 250, WHITE);

    EndDrawing();
}

Game::~Game()
{
    clean();
}