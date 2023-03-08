#ifndef GAME_H
#define GAME_H

#include <iostream>
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
        void menu();
        void clean();

};

#if defined(PLATFORM_WEB)
    void webRun(Game &game);
#endif

#endif