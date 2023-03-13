#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "raylib.h"
#include "constants.h"
#include "entity.h"
#include "camera.h"
#include "pathfinder.h"
#include "map.h"
#include "cursor.h"
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
        bool player_turn {};

        Texture2D necromancer_tex {};
        Texture2D pyromancer_tex {};
        Texture2D witch_tex {};
        Texture2D golem_tex {};
        Texture2D floor_tex_1 {};
        Texture2D floor_tex_2 {};
        Texture2D wall_tex {};
        Texture2D wall_tex_2 {};
        Texture2D wall_tex_3 {};
        Texture2D exit_tex {};
        Texture2D potion_tex {};
        Texture2D potion_cast_tex {};
        Texture2D cursor_tex {};
        Texture2D bg_tex {};

        Entity player;
        bool player_action {};
        Entity enemy_list[10];
        Entity potion_list[5];

        Pathfinder pathfinder {}; // path to player

        Cursor cursor {};
        
        int end_x, end_y;
        float potion_anim_tick {};
        int potion_anim_index {};

        char ui_text[25] {};

        void clearMap();
        void generateMap();
        bool isVaild(int row, int col);
        void placeFloor(int row, int col, int val);
        void placeWalls(int row, int col, int val);
        void fixSideWalls(int row, int col);
        void printMap();
        void renderMap();
        void fov(int row, int col, int count);
        void updateFOV();
        
        void loadAssets();
        void menu();
        void clean();
        void camera();
        bool isValidMovement(int row, int col);
        void placeEnemies();
        void placeEnemy(int i);
        void placePotions();
        void placePotion(int i);
        bool inputHandler();
        void enemiesHandler();
        void enemyHandler(int i);
        void moveUnit(int index, int &i, int &j);
        void cursorHandler();
        void playerTurnHandler();
        void attackPlayer(int index);
        void checkPotionCollision();
        void checkHealth();

        void animationHandler();
        void playerAnimation();
        void enemiesAnimHandler();
        void enemyAnimHandler(int i);

        void potionAnimation();
        void checkExitCollision();

        void play();
        void credits();
        void ded();

        void renderUI();
};

#if defined(PLATFORM_WEB)
    void webRun(Game &game);
#endif

#endif