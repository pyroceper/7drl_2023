#include "game.h"

Game::Game()
{
    InitWindow(WIDTH, HEIGHT, TITLE.c_str());
    InitAudioDevice();
    #if defined(PLATFORM_DEKSTOP)
        SetTargetFPS(FPS);
    #endif
    DisableCursor();
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
            map[i][j].fov = 0;
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

    end_x = x;
    end_y = y;

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
    player.x = cursor.x = x;
    player.y = cursor.y = y;
    player.is_movement_done = true;
    player_action = false;
    player.health = 10;
    player_turn = true;
    pathfinder.addEntity(player.y, player.x, -1);

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
    placePotions();
}

void Game::renderMap()
{
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            if(map[i][j].fov !=0) //debug
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
           
            }
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
    pathfinder.addEntity(y, x, i);
}

void Game::placePotion(int i)
{
    int x, y;
    do 
    {
        x = 1 + rand() % (MAP_WIDTH - 2);
        y = 1 + rand() % (MAP_HEIGHT - 2);
    }while(map[x][y].render  == -1 || map[x][y].render  == 2 || map[x][y].render  == 3 || map[x][y].render  == 4);
    potion_list[i].x = x;
    potion_list[i].y = y;
}

void Game::placePotions()
{
    for(int i=0;i<5;i++)
    {
        placePotion(i);
    }
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
    golem_tex = LoadTexture("assets/gfx/char/FleshGolemIdle.png");
    floor_tex_1 = LoadTexture("assets/gfx/floor/floor_1.png");
    floor_tex_2 = LoadTexture("assets/gfx/floor/floor_2.png");
    wall_tex = LoadTexture("assets/gfx/walls/wall.png");
    wall_tex_2 = LoadTexture("assets/gfx/walls/wall_2.png");
    wall_tex_3 = LoadTexture("assets/gfx/walls/wall_3.png");
    exit_tex = LoadTexture("assets/gfx/exit.png");
    potion_tex = LoadTexture("assets/gfx/potion.png");
    cursor_tex = LoadTexture("assets/gfx/cursor.png");
    potion_cast_tex = LoadTexture("assets/gfx/PoisonCast_96x96.png");
    bg_tex = LoadTexture("assets/gfx/mountains.png");
}

void Game::clean()
{
    UnloadTexture(necromancer_tex);
    UnloadTexture(pyromancer_tex);
    UnloadTexture(witch_tex);
    UnloadTexture(golem_tex);
    UnloadTexture(floor_tex_1);
    UnloadTexture(floor_tex_2);
    UnloadTexture(wall_tex);
    UnloadTexture(wall_tex_2);
    UnloadTexture(wall_tex_3);
    UnloadTexture(cursor_tex);
    UnloadTexture(potion_cast_tex);
    UnloadTexture(bg_tex);
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
            game.play();
            break;
        case 2:
            game.credits();
            break;
        case 3:
            game.ded();
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
            pathfinder.removeEntity(player.y, player.x);
            player.y--;
            player.is_movement_done = false;
            pathfinder.addEntity(player.y, player.x, -1);
            return true;
        }
    }
    if(IsKeyPressed(KEY_S))
    {
        if(isValidMovement(player.x, player.y+1))
        {
            pathfinder.removeEntity(player.y, player.x);
            player.y++;
            player.is_movement_done = false;
            pathfinder.addEntity(player.y, player.x, -1);
            return true;
        }
    }
    if(IsKeyPressed(KEY_A))
    {
        if(isValidMovement(player.x-1, player.y))
        {
            pathfinder.removeEntity(player.y, player.x);
            player.x--;
            player.is_left = -1;
            player.is_movement_done = false;
            pathfinder.addEntity(player.y, player.x, -1);
            return true;
        }
    }
    if(IsKeyPressed(KEY_D))
    {
        if(isValidMovement(player.x+1, player.y))
        {
            pathfinder.removeEntity(player.y, player.x);
            player.x++;
            player.is_left = 1;
            player.is_movement_done = false;
            pathfinder.addEntity(player.y, player.x, -1);
            return true;
        }
    }
    if(IsKeyPressed(KEY_E))
    {
        player_action =  !player_action;
    }
    if(IsKeyPressed(KEY_E) && player_action)
    {
        cursor.x = player.x;
        cursor.y = player.y;
    }
    return false;
}

void Game::attackPlayer(int index)
{
    int x, y;
    y = enemy_list[index].y;
    x = enemy_list[index].x;
    std::cout<<"Enemy index: "<<index<<"\n";
    std::cout<<"Enemy row: "<<y<<"\n";
    std::cout<<"Enemy col: "<<x<<"\n";
    if(pathfinder.getEntity(y + 1, x) == -1 || pathfinder.getEntity(y - 1, x) || pathfinder.getEntity(y, x + 1) || pathfinder.getEntity(y, x - 1))
        player.health--;
    std::cout<<"PLAYER: "<<player.health<<std::endl;
}

void Game::moveUnit(int index, int &i, int &j)
{
    // player in the next cell
    if((pathfinder.isValid(i + 1, j) &&  pathfinder.path[i + 1][j] == 1) || (pathfinder.isValid(i - 1, j) && pathfinder.path[i - 1][j] == 1)
        || (pathfinder.isValid(i, j + 1) &&  pathfinder.path[i][j + 1] == 1) || (pathfinder.isValid(i, j - 1) && pathfinder.path[i][j - 1] == 1))
    {
        attackPlayer(index);
        return;
    }

    // east
    if(pathfinder.isValid(i + 1, j) &&  pathfinder.path[i + 1][j] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i + 1, j))
    {
        pathfinder.removeEntity(i, j);
        i++;
        pathfinder.addEntity(i, j, index);
        enemy_list[index].is_left = 1;
        return;
    }
    // west
    if(pathfinder.isValid(i - 1, j) &&  pathfinder.path[i - 1][j] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i - 1, j))
    {
        pathfinder.removeEntity(i, j);
        i--;
        pathfinder.addEntity(i, j, index);
        enemy_list[index].is_left = -1;
        return;
    }
    // north
    if(pathfinder.isValid(i, j + 1) &&  pathfinder.path[i][j + 1] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i, j + 1))
    {
        pathfinder.removeEntity(i, j);
        j++;
        pathfinder.addEntity(i, j, index);
        return;
    }
    // south
    if(pathfinder.isValid(i, j - 1) &&  pathfinder.path[i][j - 1] == pathfinder.path[i][j] - 1 && !pathfinder.checkEntity(i, j - 1))
    {
        pathfinder.removeEntity(i, j);
        j--;
        pathfinder.addEntity(i, j, index);
        return;
    }

}

void Game::enemiesHandler()
{
    for(int i=0;i<10;i++)
    {
        if(!enemy_list[i].is_movement_done && !enemy_list[i].is_dead)
            enemyHandler(i);
    }
}

void Game::enemyHandler(int i)
{
    int j = enemy_list[i].y;
    int k = enemy_list[i].x;

    moveUnit(i, j, k);

    enemy_list[i].y = j;
    enemy_list[i].x = k;

    enemy_list[i].is_movement_done = true;

    if((pathfinder.isValid(i + 1, j) &&  pathfinder.path[i + 1][j] == 1) || (pathfinder.isValid(i - 1, j) && pathfinder.path[i - 1][j] == 1)
        || (pathfinder.isValid(i, j + 1) &&  pathfinder.path[i][j + 1] == 1) || (pathfinder.isValid(i, j - 1) && pathfinder.path[i][j - 1] == 1))
    {
        attackPlayer(i);
    }
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
    DrawTextureEx(potion_tex, {10, 10}, 0.f, 5.f, WHITE);
    sprintf(ui_text, "x %d", player.health);
    DrawText(ui_text, 70, 40, 50, WHITE);
}

void Game::fov(int row, int col, int count)
{
    if(count == 4)
        return;
    if(row < MAP_WIDTH && col < MAP_HEIGHT)
        map[row][col].fov = count;
    fov(row+1, col, count+1);
    fov(row-1, col, count+1);
    fov(row, col+1, count+1);
    fov(row, col-1, count+1);
}

void Game::updateFOV()
{
    fov(player.x, player.y, 1);
}

void Game::cursorHandler()
{
    if(IsKeyPressed(KEY_I))
    {
        cursor.y--;
    }
    if(IsKeyPressed(KEY_K))
    {
        cursor.y++;
    }
    if(IsKeyPressed(KEY_J))
    {
        cursor.x--;
    }
    if(IsKeyPressed(KEY_L))
    {
        cursor.x++;
    }
    if(pathfinder.checkEntity(cursor.y, cursor.x))
        cursor.index = 16;
    else 
        cursor.index = 0;
}


void Game::playerTurnHandler()
{
    int y = player.y;
    int x = player.x;
    if(IsKeyPressed(KEY_X) && player_action)
    {
        if(pathfinder.checkEntity(y + 1, x))
        {
            int i = pathfinder.getEntity(y + 1, x);
            pathfinder.removeEntity(y + 1, x);
            enemy_list[i].is_dead = true;
        }
        if(pathfinder.checkEntity(y - 1, x))
        {
            int i = pathfinder.getEntity(y - 1, x);
            pathfinder.removeEntity(y - 1, x);
            enemy_list[i].is_dead = true;
        }
        if(pathfinder.checkEntity(y, x + 1))
        {
            int i = pathfinder.getEntity(y, x + 1);
            pathfinder.removeEntity(y, x + 1);
            enemy_list[i].is_dead = true;
        }
        if(pathfinder.checkEntity(y, x - 1))
        {
            int i = pathfinder.getEntity(y, x - 1);
            pathfinder.removeEntity(y, x - 1);
            enemy_list[i].is_dead = true;
        }
    }
}

void Game::checkPotionCollision()
{
    for(int i=0;i<5;i++)
    {
        if(potion_list[i].x == player.x && potion_list[i].y == player.y)
        {
            player.health = 10;
            potion_list[i].is_dead = true;
            break;
        }
    }
}

void Game::potionAnimation()
{
    potion_anim_tick += 0.5f * GetFrameTime();
    if(potion_anim_tick > 2.f)
    {
        potion_anim_index = (potion_anim_index + 96) % 3744;
    }
}

void Game::checkExitCollision()
{
    if(player.x == end_x && player.y == end_y)
    {
        state = 2;
    }
}

void Game::checkHealth()
{
    if(player.health < 0)
        state = 3;
}

void Game::play()
{
    camera();

    //input
    input_state_changed = inputHandler();
    cursorHandler();

    if(input_state_changed)
    {
        std::cout<<"row = "<<player.y<<std::endl;
        std::cout<<"col = "<<player.x<<std::endl;
        pathfinder.search(player.y, player.x);
        //pathfinder.printPath();
        for(int i=0;i<10;i++)
        {
            enemy_list[i].is_movement_done = false;
        }
        input_state_changed = false;
        player_turn = false;
    }
    
    if(!player_turn)
    {
        enemiesHandler();
        player_turn = true;
    }
    playerTurnHandler();
    animationHandler();
    updateFOV();

    potionAnimation();

    checkPotionCollision();

    checkExitCollision();

    checkHealth();

    //render
    BeginDrawing();

        ClearBackground(BLACK);

        renderMap();


        if(map[end_x][end_y].fov)
            DrawTextureEx(exit_tex, {(static_cast<float>(end_x) * TILE_SIZE) - offset_x, (static_cast<float>(end_y) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);

        for(int i=0;i<5;i++)
        {
            int x = potion_list[i].x;
            int y = potion_list[i].y;
            if(!potion_list[i].is_dead && map[x][y].fov)
                DrawTextureEx(potion_tex, {(static_cast<float>(potion_list[i].x) * TILE_SIZE) - offset_x, (static_cast<float>(potion_list[i].y) * TILE_SIZE) - offset_y}, 0.f, 2.f, WHITE);
        }


        if(player.is_movement_done)
            DrawTexturePro(necromancer_tex, (Rectangle){static_cast<float>(player.animation_index), 0, static_cast<float>(player.is_left * 16), 16}, (Rectangle){(static_cast<float>(player.x) * TILE_SIZE) - offset_x, (static_cast<float>(player.y) * TILE_SIZE) - offset_y, 16 * 2, 16 * 2}, {0, 0}, 0.f, WHITE);
        

        for(int i=0;i<10;i++)
        {
            int x = enemy_list[i].x;
            int y = enemy_list[i].y;
            if(!enemy_list[i].is_dead && map[x][y].fov)
                DrawTexturePro(golem_tex, (Rectangle){static_cast<float>(enemy_list[i].animation_index), 0, static_cast<float>(enemy_list[i].is_left * 16), 16}, (Rectangle){(static_cast<float>(enemy_list[i].x) * TILE_SIZE) - offset_x , (static_cast<float>(enemy_list[i].y) * TILE_SIZE) - offset_y, 16 * 2, 16 * 2}, {0, 0}, 0.f, WHITE);
        }

        if(player_action)
            DrawTexturePro(cursor_tex, (Rectangle){static_cast<float>(cursor.index), 0, 16, 16}, (Rectangle){(static_cast<float>(cursor.x) * TILE_SIZE) - offset_x, (static_cast<float>(cursor.y) * TILE_SIZE)- offset_y, 16 * 2, 16 * 2}, {}, 0.f, WHITE);

        DrawTexturePro(potion_cast_tex, (Rectangle){static_cast<float>(potion_anim_index), 0, 96, 96}, (Rectangle){0, 0, 96, 96}, {}, 0.f, WHITE);

        renderUI();


    EndDrawing();

    //fix ghosting
    if(!player.is_movement_done)
    {
        player.is_movement_done = true;
    }

}

void Game::menu()
{
    if(IsKeyPressed(KEY_X))
        state = 1;

    playerAnimation();
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(bg_tex, {} , 0.f, 3.2f, WHITE);

        DrawText("Nightmare Corridor" , 200, 150, 50, WHITE);
        DrawTexturePro(necromancer_tex, (Rectangle){static_cast<float>(player.animation_index), 0, 16, 16}, (Rectangle){ 120, 130, 16 * 4, 16 * 4}, {0, 0}, 0.f, WHITE);

        DrawText("Controls: W,A,S,D for movement", 200, 250, 20, WHITE);
        DrawText("E to enable cursor and I,J,K,L for movement", 200, 270, 20, WHITE);
        DrawText("X to attack", 200, 290, 20, WHITE);

        DrawText("Press 'X' to start ", 200, 320, 40, RED);


    EndDrawing();  
}

void Game::credits()
{
    if(IsKeyPressed(KEY_X))
        state = 1;

    potionAnimation();

    playerAnimation();
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(bg_tex, {} , 0.f, 3.2f, WHITE);

        DrawText("Credits :)" , 200, 150, 50, WHITE);
        DrawTexturePro(necromancer_tex, (Rectangle){static_cast<float>(player.animation_index), 0, 16, 16}, (Rectangle){ 120, 130, 16 * 4, 16 * 4}, {0, 0}, 0.f, WHITE);

        DrawText("Assets used: Magical Free Asset Pack - deepdivegamestudio", 70, 250, 20, WHITE);
        DrawText("Pixel Crawler - anokolisa", 70, 290, 20, WHITE);
        DrawText("Forest of Illusion (Background) - ansimuz", 70, 330, 20, WHITE);
        DrawText("Dungeon Tileset II - 0x72", 70, 370, 20, WHITE);
        DrawTexturePro(potion_cast_tex, (Rectangle){static_cast<float>(potion_anim_index), 0, 96, 96}, (Rectangle){0, 0, 96, 96}, {}, 0.f, WHITE);


        DrawText("Press 'X' to restart ", 200, 500, 40, RED);

    EndDrawing();
}

void Game::ded()
{
    if(IsKeyPressed(KEY_X))
        state = 1;

    BeginDrawing();
        ClearBackground(BLACK);
   
    playerAnimation();
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(bg_tex, {} , 0.f, 3.2f, WHITE);

        DrawText("You Died!! :( " , 200, 150, 50, RED);
        DrawTexturePro(necromancer_tex, (Rectangle){static_cast<float>(player.animation_index), 0, 16, 16}, (Rectangle){ 120, 130, 16 * 4, 16 * 4}, {0, 0}, 0.f, WHITE);

        DrawText("Controls: W,A,S,D for movement", 200, 250, 20, WHITE);
        DrawText("E to enable cursor and I,J,K,L for movement", 200, 270, 20, WHITE);
        DrawText("X to attack", 200, 290, 20, WHITE);

        DrawText("Press 'X' to restart ", 200, 320, 40, RED);

    EndDrawing();   
}

Game::~Game()
{
    clean();
}