#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <iostream>
#include <cstdio>
#include <queue>
#include "constants.h"

class Pathfinder
{
    public:
        Pathfinder();
        void reset();
        bool isValid(int row, int col);
        int path[MAP_WIDTH][MAP_HEIGHT] = {0};
        void search(int row, int col);
        void addObstacle(int row, int col);
        void removeObstacle(int row, int col);
        bool checkObstacle(int row, int col);
        void addEntity(int row, int col);
        void removeEntity(int row, int col);
        bool checkEntity(int row, int col);
        void printPath();
    private:
        std::queue<int> queue_r {};
        std::queue<int> queue_c {};
        std::queue<int> queue_count {};
        bool visited[MAP_WIDTH*MAP_HEIGHT] = {false};
        bool obstacles[MAP_WIDTH*MAP_HEIGHT] = {false};
        bool entity_present[MAP_WIDTH*MAP_HEIGHT] = {false};
};

#endif