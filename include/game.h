#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "raylib.h"
#include "constants.h"
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

        void clearMap();
        void generateMap();
        bool isVaild(int row, int col);
        void placeFloor(int row, int col);
        void printMap();
        void renderMap();
        void loadAssets();
        void menu();
        void clean();

};

#if defined(PLATFORM_WEB)
    void webRun(Game &game);
#endif

#endif