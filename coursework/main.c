// This is the main controller

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "main.h"
#include "background.h"
#include "robot.h"
#include "random.h"
#include "graphics.h"
#include "map.h"

// 'o' - nothing, 'm' - marker, 'w' - wall, 'h' - home, 'b' - obstacle

// Stage 3 function
void findMarkerNextToWall(robot *robot, map *map)
{
    while (1)
    {
        if (atMarker(robot, map))
        {
            pickUpMarker(robot, map);
        }
        if (isAtHome(robot, map) && robot->numMarkersCaried > 0)
        {
            dropMarker(robot, map);
            break;
        }
        if (!canMoveForward(robot, map))
        {
            left(robot);
        }
        forward(robot, map);
    }
}

void findCorner(robot *robot, map *map)
{
    while (canMoveForward(robot, map))
    {
        forward(robot, map);
    }
    right(robot);
    while (canMoveForward(robot, map))
    {
        forward(robot, map);
    }
    // turn around
    right(robot);
    right(robot);
}

bool moveForwardsUntilHitWall(robot *robot, map *map)
{
    while (canMoveForward(robot, map))
    {
        if (atMarker(robot, map))
        {
            pickUpMarker(robot, map);
        }
        if (isAtHome(robot, map) && markerCount(robot) == 1)
        {
            dropMarker(robot, map);
            return true;
        }
        forward(robot, map);
    }
    return false;
}

void goThroughWholeGrid(robot *robot, map *map, bool turnLeft)
{
    while (true)
    {
        // if at home and all markers are dropped
        if (moveForwardsUntilHitWall(robot, map))
        {
            return;
        }
        turnLeft ? left(robot) : right(robot);
        if (!canMoveForward(robot, map))
        {
            forward(robot, map);
            turnLeft ? left(robot) : right(robot);
        }
        turnLeft ? left(robot) : right(robot);
        turnLeft = !turnLeft;
    }
}

// Stage 4 function
void findMarkerAnywhere(robot *robot, map *map)
{
    findCorner(robot, map);
    goThroughWholeGrid(robot, map, true);
}

// Stage 5 start
point *neighbourPoints(point p)
{
    point *neighbouringPoints = (point *)malloc(4 * sizeof(point));
    neighbouringPoints[0] = (point){p.x, p.y - 1}; // North
    neighbouringPoints[1] = (point){p.x, p.y + 1}; // South
    neighbouringPoints[2] = (point){p.x + 1, p.y}; // West
    neighbouringPoints[3] = (point){p.x - 1, p.y}; // East
    return neighbouringPoints;
}

void turnToDir(robot *robot, enum dirs newDir)
{
    while (robot->dir != newDir)
    {
        right(robot);
    }
}

void moveTo(robot *robot, point newPos, map *map)
{
    int dx = newPos.x - robot->pos.x;
    int dy = newPos.y - robot->pos.y;

    if (!dx && !dy)
        return;

    enum dirs newDir = dx ? (dx < 0 ? WEST : EAST) : 0;
    newDir = dy ? (dy < 0 ? NORTH : SOUTH) : newDir;
    turnToDir(robot, newDir);

    forward(robot, map);

    drawRobot(robot, map);
}

// This function will later be used to free all allocated memory
void end()
{
    exit(0);
}

void getHome(robot *robot, map *map)
{
    while (markerCount(robot))
    {
        dropMarker(robot, map);
    }
    end();
}

void checkPos(robot *robot, map *map, char lookingFor)
{
    if (lookingFor == 'm' && atMarker(robot, map))
    {
        pickUpMarker(robot, map);
    }
    else if (lookingFor == 'h' && isAtHome(robot, map))
    {
        getHome(robot, map);
    }
}

void movingEverywhereRecurAbs(map *map, char **mapCopy, point curPos, robot *robot, char lookingFor)
{
    if (mapCopy[curPos.y][curPos.x] == 'w' || mapCopy[curPos.y][curPos.x] == 'b' || mapCopy[curPos.y][curPos.x] == 'v')
    {
        return;
    }
    mapCopy[curPos.y][curPos.x] = 'v';
    moveTo(robot, curPos, map);

    checkPos(robot, map, lookingFor);

    point *neighbouringPoints = neighbourPoints(robot->pos);
    for (int i = 0; i < 4; i++)
    {
        movingEverywhereRecurAbs(map, mapCopy, neighbouringPoints[i], robot, lookingFor);
        moveTo(robot, curPos, map);
    }
}

int main(void)
{
    srand(time(NULL));
    int width = randomNumber(8, 14);
    int height = randomNumber(8, 14);
    int squareSize = 50;
    int offset = 50;
    canvas canvas = {width * squareSize + 2 * offset, height * squareSize + 2 * offset, squareSize, offset};
    map map = {
        width,
        height,
        canvas,
        generateMap(width, height)};
    point startingPos = randomEmptyPointOnMap(map.map, width, height);
    robot robot = {startingPos, randomDir(), 0};

    drawBackground(&map);
    char **mapCopy = copyMap(&map);
    movingEverywhereRecurAbs(&map, mapCopy, startingPos, &robot, 'm');
    mapCopy = copyMap(&map);
    movingEverywhereRecurAbs(&map, mapCopy, startingPos, &robot, 'h');
    free(map.map);
    return 0;
}
