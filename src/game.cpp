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
    return (row < MAP_WIDTH && col < MAP_HEIGHT && map[row][col] == -1);
}

void Game::placeFloor(int row, int col, int val)
{
    if(isVaild(row, col))
    {
        map[row][col] = val;
        total_floor++;
    }
}

void Game::placeWalls(int row, int col, int val)
{
    if(row < MAP_WIDTH && col < MAP_HEIGHT)
    {
        if(map[row][col] == 0 || map[row][col] == 1)
        {
            if(map[row-1][col] == -1)
            {
                map[row-1][col] = val;
            }
            if(map[row+1][col] == -1)
            {
                map[row+1][col] = val;
            }
            if(map[row][col-1] == -1)
            {
                map[row][col-1] = val;
            }
            if(map[row][col+1] == -1)
            {
                map[row][col+1] = val;
            }
        }
    }
}

void Game::generateMap()
{
    unsigned int seed = time(nullptr);
    srand((unsigned)seed);
    std::cout<<"[Seed] "<<seed<<"\n";
    int x, y, dx, dy, tunnel, floor_type, floor_val;
    dx = dy = tunnel = 0;

    x = 1 + rand() % (MAP_WIDTH - 2);
    y = 1 + rand() % (MAP_HEIGHT - 2);

    map[x][y] = 0;

    temp_end_x = x;
    temp_end_y = y;

    int dir {};
    while(total_floor < MAX_FLOOR)
    {
        tunnel = std::max(tunnel - 1, 0);
        if(tunnel == 0)
        {
            dir = rand() % 5; // 0 - 4
            switch(dir)
            {
                case 0: // west
                    dx = 0;
                    dy = -1;
                    break;

                case 1: //east
                    dx = 0;
                    dy = 1;
                    break;

                case 2: // north
                    dx = -1;
                    dy = 0;
                    break;

                case 3: // south
                    dx = 1;
                    dy = 0;
                    break;
                
                case 4:
                    tunnel = 2 + rand() % 8; // 2 + (0-7)
                    break;
            }
        }

        x = std::min(std::max(x + dx, 1), MAP_WIDTH - 2);
        y = std::min(std::max(y + dy, 1), MAP_HEIGHT - 2);

        floor_type = rand() % 10;

        if(floor_type == 0)
            floor_val = 1;
        else 
            floor_val = 0;

        placeFloor(x, y, floor_val);
    }
    temp_x = x;
    temp_y = y;

    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            placeWalls(i,j,2);
        }
    }
}

void Game::renderMap()
{
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            if(map[i][j] == 0)
                DrawTexture(floor_tex_1, i * 16, j * 16, WHITE);
            else if(map[i][j] == 1)
                DrawTexture(floor_tex_2, i * 16, j * 16, WHITE);
            else if(map[i][j] == 2)
                DrawTexture(wall_tex, i * 16, j * 16, WHITE);
            else
                DrawRectangle(i * 16, j * 16, 16, 16, BLACK);
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
    pyromancer_tex = LoadTexture("assets/gfx/pyro.png");
    floor_tex_1 = LoadTexture("assets/gfx/floor/floor_1.png");
    floor_tex_2 = LoadTexture("assets/gfx/floor/floor_2.png");
    wall_tex = LoadTexture("assets/gfx/walls/wall.png");
}

void Game::clean()
{
    UnloadTexture(necromancer_tex);
    UnloadTexture(pyromancer_tex);
    UnloadTexture(floor_tex_1);
    UnloadTexture(floor_tex_2);
    UnloadTexture(wall_tex);
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

        DrawTexture(necromancer_tex, temp_x * 16, temp_y * 16, WHITE);
        DrawTexture(pyromancer_tex, temp_end_x * 16, temp_end_y * 16, WHITE);

    EndDrawing();
}

Game::~Game()
{
    clean();
}