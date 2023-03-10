#include "game.h"

Game::Game()
{
    InitWindow(WIDTH, HEIGHT, TITLE.c_str());
    InitAudioDevice();
    #if defined(PLATFORM_DEKSTOP)
        SetTargetFPS(FPS);
    #endif
    state = offset_x = offset_y = 0;
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
    player.x = x;
    player.y = y;
    player.is_movement_done = true;

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
                DrawTextureEx(floor_tex_1, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
            else if(map[i][j] == 1)
                DrawTextureEx(floor_tex_2, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
            else if(map[i][j] == 2)
                DrawTextureEx(wall_tex, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
            else
                DrawRectangle((i * TILE_SIZE) - offset_x, (j * TILE_SIZE) - offset_y, TILE_SIZE, TILE_SIZE, BLACK);
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

bool Game::isValidMovement(int row, int col)
{
    return (row < MAP_WIDTH && col < MAP_HEIGHT && map[row][col] != 2);
}

void Game::camera()
{
    Cam::followTarget(player.x * TILE_SIZE, player.y * TILE_SIZE, 740, 740);
    offset_x = static_cast<int>(Cam::offset.x);
    offset_y = static_cast<int>(Cam::offset.y);
}

void Game::menu()
{
    camera();

    //input
    if(IsKeyPressed(KEY_W))
    {
        if(isValidMovement(player.x, player.y-1))
        {
            player.y--;
            player.is_movement_done = false;
        }
    }
    if(IsKeyPressed(KEY_S))
    {
        if(isValidMovement(player.x, player.y+1))
        {
            player.y++;
            player.is_movement_done = false;
        }
    }
    if(IsKeyPressed(KEY_A))
    {
        if(isValidMovement(player.x-1, player.y))
        {
            player.x--;
            player.is_movement_done = false;
        }
    }
    if(IsKeyPressed(KEY_D))
    {
        if(isValidMovement(player.x+1, player.y))
        {
            player.x++;
            player.is_movement_done = false;
        }
    }

    //render
    BeginDrawing();

        ClearBackground(RAYWHITE);

        renderMap();

        if(player.is_movement_done)
            DrawTextureEx(necromancer_tex, {(static_cast<float>(player.x) * TILE_SIZE) - offset_x, (static_cast<float>(player.y) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
        
        DrawTextureEx(pyromancer_tex, {(static_cast<float>(temp_end_x) * TILE_SIZE) - offset_x, (static_cast<float>(temp_end_y) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);

    EndDrawing();

    //fix ghosting
    if(!player.is_movement_done)
    {
        player.is_movement_done = true;
    }

}

Game::~Game()
{
    clean();
}