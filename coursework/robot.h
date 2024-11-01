#pragma once
#include "main.h"
// Everytime there is a change in direction, either 2 is added or substracted
enum dirs
{
    NORTH = -3,
    WEST = -1,
    SOUTH = 1,
    EAST = 3
};
typedef struct point
{
    int x;
    int y;
} point;
typedef struct robot
{
    point pos;
    enum dirs dir;
} robot;

void drawRobot(robot *robot, map map);
void forward(robot *robot, map map);
void left(robot *robot);
void right(robot *robot);
int atMarker(robot *robot, map map);
int canMoveForward(robot *robot, map map);