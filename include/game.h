#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "raylib.h"
#include "constants.h"
#include "entity.h"
#include "camera.h"
#include "pathfinder.h"
#include "map.h"
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif


class Game
{
    public:
        Game();
        ~Game();
    #if defined(PLATFORM_WEB)
        friend void webRun(Game &game);
    #endif

    private:
        int state {};
        //int map[MAP_WIDTH][MAP_HEIGHT]; // used for rendering
        Map map[MAP_WIDTH][MAP_HEIGHT];
        int total_floor {};

        int offset_x {};
        int offset_y {};

        bool input_state_changed  {};

        Texture2D necromancer_tex {};
        Texture2D pyromancer_tex {};
        Texture2D witch_tex {};
        Texture2D floor_tex_1 {};
        Texture2D floor_tex_2 {};
        Texture2D wall_tex {};
        Texture2D wall_tex_2 {};
        Texture2D wall_tex_3 {};

        Entity player;
        Entity enemy_list[10];

        Pathfinder pathfinder {}; // path to player
        
        int temp_end_x, temp_end_y;

        void clearMap();
        void generateMap();
        bool isVaild(int row, int col);
        void placeFloor(int row, int col, int val);
        void placeWalls(int row, int col, int val);
        void fixSideWalls(int row, int col);
        void printMap();
        void renderMap();
        
        void loadAssets();
        void menu();
        void clean();
        void camera();
        bool isValidMovement(int row, int col);
        void placeEnemies();
        void placeEnemy(int i);
        bool inputHandler();
        void enemiesHandler();
        void enemyHandler(int i);
        void moveUnit(int &i, int &j);

        void animationHandler();
        void playerAnimation();
        void enemiesAnimHandler();
        void enemyAnimHandler(int i);

        void renderUI();
};

#if defined(PLATFORM_WEB)
    void webRun(Game &game);
#endif

#endif