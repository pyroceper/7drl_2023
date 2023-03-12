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
    pathfinder.printPath();
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
            map[i][j].render = -1;
        }
    }
}

bool Game::isVaild(int row, int col)
{
    return (row < MAP_WIDTH && col < MAP_HEIGHT && map[row][col].render  == -1);
}


void Game::placeFloor(int row, int col, int val)
{
    if(isVaild(row, col))
    {
        map[row][col].render = val;
        total_floor++;
    }
}

void Game::placeWalls(int row, int col, int val)
{
    if(row < MAP_WIDTH && col < MAP_HEIGHT)
    {
        if(map[row][col].render == 0 || map[row][col].render == 1)
        {
            if(map[row-1][col].render == -1)
            {
                map[row-1][col].render = val;
                pathfinder.addObstacle(col, row-1);
            }
            if(map[row+1][col].render == -1)
            {
                map[row+1][col].render = val;
                pathfinder.addObstacle(col, row+1);
            }
            if(map[row][col-1].render == -1)
            {
                map[row][col-1].render = val;
                pathfinder.addObstacle(col-1, row);
            }
            if(map[row][col+1].render == -1)
            {
                map[row][col+1].render = val;
                pathfinder.addObstacle(col+1, row);
            }
        }
    }
}

void Game::generateMap()
{
    unsigned int seed = time(nullptr);
    srand((unsigned)seed);
    std::cout<<"[Seed] "<<seed<<"\n";
    int x, y, dx, dy, tunnel, floor_type, wall_type;
    dx = dy = tunnel = 0;

    x = 1 + rand() % (MAP_WIDTH - 2);
    y = 1 + rand() % (MAP_HEIGHT - 2);

    map[x][y].render = 0;

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
            floor_type = 1;
        else 
            floor_type = 0;

        placeFloor(x, y, floor_type);
    }
    player.x = x;
    player.y = y;
    player.is_movement_done = true;

    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            wall_type = rand() % 15;
            if(wall_type < 5)
                wall_type = 3;
            if(wall_type > 5 && wall_type < 9)
                wall_type = 4;
            else 
                wall_type = 2;
            placeWalls(i, j, wall_type);
        }
    }

    placeEnemies();
}

void Game::renderMap()
{
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            if(map[i][j].render == 0)
                DrawTextureEx(floor_tex_1, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
            else if(map[i][j].render  == 1)
                DrawTextureEx(floor_tex_2, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
            else if(map[i][j].render  == 2)
                DrawTextureEx(wall_tex, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
            else if(map[i][j].render  == 3)
                DrawTextureEx(wall_tex_2, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
            else if(map[i][j].render  == 4)
                DrawTextureEx(wall_tex_3, {(static_cast<float>(i) * TILE_SIZE) - offset_x, (static_cast<float>(j) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
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
            std::cout<<map[i][j].render<<" ";
        }
        std::cout<<"\n";
    }
}

void Game::placeEnemy(int i)
{
    int x, y;
    do 
    {
        x = 1 + rand() % (MAP_WIDTH - 2);
        y = 1 + rand() % (MAP_HEIGHT - 2);
    }while(map[x][y].render  == -1 || map[x][y].render  == 2 || map[x][y].render  == 3 || map[x][y].render  == 4 || pathfinder.checkEntity(y, x));
    enemy_list[i].x = x;
    enemy_list[i].y = y;
    pathfinder.addEntity(y, x);
}

void Game::placeEnemies()
{
    for(int i=0;i<10;i++)
    {
        placeEnemy(i);
    }
}

void Game::loadAssets()
{
    necromancer_tex = LoadTexture("assets/gfx/char/AdeptNecromancerIdle.png");
    pyromancer_tex = LoadTexture("assets/gfx/pyro.png");
    witch_tex = LoadTexture("assets/gfx/char/VileWitchIdle.png");
    floor_tex_1 = LoadTexture("assets/gfx/floor/floor_1.png");
    floor_tex_2 = LoadTexture("assets/gfx/floor/floor_2.png");
    wall_tex = LoadTexture("assets/gfx/walls/wall.png");
    wall_tex_2 = LoadTexture("assets/gfx/walls/wall_2.png");
    wall_tex_3 = LoadTexture("assets/gfx/walls/wall_3.png");
}

void Game::clean()
{
    UnloadTexture(necromancer_tex);
    UnloadTexture(pyromancer_tex);
    UnloadTexture(witch_tex);
    UnloadTexture(floor_tex_1);
    UnloadTexture(floor_tex_2);
    UnloadTexture(wall_tex);
    UnloadTexture(wall_tex_2);
    UnloadTexture(wall_tex_3);
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
    return (row < MAP_WIDTH && col < MAP_HEIGHT && map[row][col].render  != 2 && map[row][col].render  != 3 && map[row][col].render  != 4 && !pathfinder.checkEntity(col, row));
}

void Game::camera()
{
    Cam::followTarget(player.x * TILE_SIZE, player.y * TILE_SIZE, 740, 740);
    offset_x = static_cast<int>(Cam::offset.x);
    offset_y = static_cast<int>(Cam::offset.y);
}

bool Game::inputHandler()
{
    if(IsKeyPressed(KEY_W))
    {
        if(isValidMovement(player.x, player.y-1))
        {
            player.y--;
            player.is_movement_done = false;
            return true;
        }
    }
    if(IsKeyPressed(KEY_S))
    {
        if(isValidMovement(player.x, player.y+1))
        {
            player.y++;
            player.is_movement_done = false;
            return true;
        }
    }
    if(IsKeyPressed(KEY_A))
    {
        if(isValidMovement(player.x-1, player.y))
        {
            player.x--;
            player.is_movement_done = false;
            return true;
        }
    }
    if(IsKeyPressed(KEY_D))
    {
        if(isValidMovement(player.x+1, player.y))
        {
            player.x++;
            player.is_movement_done = false;
            return true;
        }
    }
    return false;
}

void Game::moveUnit(int &i, int &j)
{
    // player in the next cell
    if((pathfinder.isValid(i + 1, j) &&  pathfinder.path[i + 1][j] == 1) || (pathfinder.isValid(i - 1, j) && pathfinder.path[i - 1][j] == 1)
        || (pathfinder.isValid(i, j + 1) &&  pathfinder.path[i][j + 1] == 1) || (pathfinder.isValid(i, j - 1) &&  pathfinder.path[i][j - 1] == 1))
    {
        return;
    }

    // south
    if(pathfinder.isValid(i + 1, j) &&  pathfinder.path[i + 1][j] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i + 1, j))
    {
        pathfinder.removeEntity(i, j);
        i++;
        pathfinder.addEntity(i, j);
        return;
    }
    // north
    if(pathfinder.isValid(i - 1, j) &&  pathfinder.path[i - 1][j] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i - 1, j))
    {
        pathfinder.removeEntity(i, j);
        i--;
        pathfinder.addEntity(i, j);
        return;
    }
    // east
    if(pathfinder.isValid(i, j + 1) &&  pathfinder.path[i][j + 1] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i, j + 1))
    {
        pathfinder.removeEntity(i, j);
        j++;
        pathfinder.addEntity(i, j);
        return;
    }
    // west
    if(pathfinder.isValid(i, j - 1) &&  pathfinder.path[i][j - 1] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i, j - 1))
    {
        pathfinder.removeEntity(i, j);
        j--;
        pathfinder.addEntity(i, j);
        return;
    }

}

void Game::enemiesHandler()
{
    for(int i=0;i<10;i++)
    {
        if(!enemy_list[i].is_movement_done)
            enemyHandler(i);
    }
}

void Game::enemyHandler(int i)
{
    int j = enemy_list[i].y;
    int k = enemy_list[i].x;

    moveUnit(j, k);

    enemy_list[i].y = j;
    enemy_list[i].x = k;

    enemy_list[i].is_movement_done = true;
}

void Game::playerAnimation()
{
    player.animation_tick += 1.f * GetFrameTime();
    if(player.animation_tick > 0.2f)
    {
        player.animation_tick = 0.f;
        player.animation_index = (player.animation_index + 16) % 48;
    }
}

void Game::enemyAnimHandler(int i)
{
    enemy_list[i].animation_tick += 1.f * GetFrameTime();
    if(enemy_list[i].animation_tick > 0.2f)
    {
        enemy_list[i].animation_tick = 0.f;
        enemy_list[i].animation_index = (enemy_list[i].animation_index + 16) % 48;
    }
}

void Game::enemiesAnimHandler()
{
    for(int i=0;i<10;i++)
    {
        enemyAnimHandler(i);
    }
}

void Game::animationHandler()
{
    playerAnimation();
    enemiesAnimHandler();
}

void Game::renderUI()
{
    DrawRectangle(720, 20, 300, 400, WHITE);
}

void Game::menu()
{
    camera();

    //input
    input_state_changed = inputHandler();

    if(input_state_changed)
    {
        std::cout<<"row = "<<player.y<<std::endl;
        std::cout<<"col = "<<player.x<<std::endl;
        pathfinder.search(player.y, player.x);
        pathfinder.printPath();
        for(int i=0;i<10;i++)
        {
            enemy_list[i].is_movement_done = false;
        }
        input_state_changed = false;
    }
    
    
    enemiesHandler();
    animationHandler();


    //render
    BeginDrawing();

        ClearBackground(BLACK);

        renderMap();

        if(player.is_movement_done)
            DrawTexturePro(necromancer_tex, (Rectangle){static_cast<float>(player.animation_index), 0, 16, 16}, (Rectangle){(static_cast<float>(player.x) * TILE_SIZE) - offset_x, (static_cast<float>(player.y) * TILE_SIZE) - offset_y, 16 * 2, 16 * 2}, {0, 0}, 0.f, WHITE);
        
        DrawTextureEx(pyromancer_tex, {(static_cast<float>(temp_end_x) * TILE_SIZE) - offset_x, (static_cast<float>(temp_end_y) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);

        for(int i=0;i<10;i++)
        {
            DrawTexturePro(witch_tex, (Rectangle){static_cast<float>(enemy_list[i].animation_index), 0, 16, 16}, (Rectangle){(static_cast<float>(enemy_list[i].x) * TILE_SIZE) - offset_x , (static_cast<float>(enemy_list[i].y) * TILE_SIZE) - offset_y, 16 * 2, 16 * 2}, {0, 0}, 0.f, WHITE);
        }

        //renderUI();


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