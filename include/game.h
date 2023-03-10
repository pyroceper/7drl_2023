#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "raylib.h"
#include "constants.h"
#include "entity.h"
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
        int map[MAP_WIDTH][MAP_HEIGHT];
        int total_floor {};

        Texture2D necromancer_tex {};
        Texture2D pyromancer_tex {};
        Texture2D floor_tex_1 {};
        Texture2D floor_tex_2 {};
        Texture2D wall_tex {};

        Entity player;
        
        int temp_end_x, temp_end_y;

        void clearMap();
        void generateMap();
        bool isVaild(int row, int col);
        void placeFloor(int row, int col, int val);
        void placeWalls(int row, int col, int val);
        void printMap();
        void renderMap();
        void loadAssets();
        void menu();
        void clean();
        bool isValidMovement(int row, int col);

};

#if defined(PLATFORM_WEB)
    void webRun(Game &game);
#endif

#endif