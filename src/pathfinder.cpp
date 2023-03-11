#include "pathfinder.h"

Pathfinder::Pathfinder()
{
    reset();
}

void Pathfinder::reset()
{
    for(int i=0;i<MAP_WIDTH*MAP_HEIGHT;i++)
    {
        visited[i] = false;
    }
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            path[i][j] = -1;
        }
    }
    queue_r = std::queue<int>(); //queue_r.clear();
    queue_c = std::queue<int>();//queue_c.clear();
    queue_count = std::queue<int>(); //queue_count.clear();
}

void Pathfinder::printPath()
{
    std::cout<<"\n";
    for(int i=0;i<MAP_WIDTH;i++)
    {
        for(int j=0;j<MAP_HEIGHT;j++)
        {
            printf("%02d ", path[i][j]);
        }
        std::cout<<"\n";
    }
}

bool Pathfinder::isValid(int row, int col)
{
    return ((row >= 0 && row < MAP_WIDTH) && (col >= 0 && col < MAP_HEIGHT));
}

void Pathfinder::addObstacle(int row, int col)
{
    obstacles[MAP_WIDTH* row + col] = true;
}

void Pathfinder::removeObstacle(int row, int col)
{
    obstacles[MAP_WIDTH* row + col] = false;
}

bool Pathfinder::checkObstacle(int row, int col)
{
    return obstacles[MAP_WIDTH * row + col];
}

void Pathfinder::search(int row, int col)
{   
    reset();
    int count = 1;
    queue_r.push(row); queue_c.push(col);
    queue_count.push(count);
    visited[MAP_WIDTH * row + col] = true;

    while(!queue_r.empty())
    {
        int rr = queue_r.front(); queue_r.pop();
        int cc = queue_c.front(); queue_c.pop();
        
        count = queue_count.front(); queue_count.pop();
        path[rr][cc] = count;

        //SOUTH
        if(isValid(rr+1, cc) && !visited[MAP_WIDTH * (rr+1) + cc] && !obstacles[MAP_WIDTH * (rr+1) + cc])
        {
            queue_r.push(rr+1); queue_c.push(cc); queue_count.push(count + 1);
            visited[MAP_WIDTH * (rr+1) + cc] = true;
        }
        //NORTH
        if(isValid(rr-1, cc) && !visited[MAP_WIDTH * (rr-1) + cc] && !obstacles[MAP_WIDTH * (rr-1) + cc])
        {
            queue_r.push(rr-1); queue_c.push(cc); queue_count.push(count + 1);
            visited[MAP_WIDTH * (rr-1) + cc] = true;
        }
        //EAST
        if(isValid(rr, cc+1) && !visited[MAP_WIDTH * rr + (cc+1)] && !obstacles[MAP_WIDTH * rr + (cc+1)])
        {
            queue_r.push(rr); queue_c.push(cc+1); queue_count.push(count + 1);
            visited[MAP_WIDTH * rr + (cc+1)] = true;
        }
        //WEST
        if(isValid(rr, cc-1) && !visited[MAP_WIDTH * rr + (cc-1)] && !obstacles[MAP_WIDTH * rr + (cc-1)])
        {
            queue_r.push(rr); queue_c.push(cc-1); queue_count.push(count + 1);
            visited[MAP_WIDTH * rr + (cc-1)] = true;
        }
    }
}