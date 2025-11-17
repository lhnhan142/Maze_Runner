#include "GameUtils.h"
#include "Maze.h" // Include full header here
#include <cmath>

bool hasLineOfSight(const Maze& maze, int x0, int y0, int x1, int y1) {
    int dx = std::abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int e2;
    while (true) {
        if (y0 < 0 || y0 >= maze.h || x0 < 0 || x0 >= maze.w) {
            return false;
        }
        if (maze.walls[y0][x0]) {
            return false;
        }
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            if (x0 == x1) break;
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            err += dx;
            y0 += sy;
        }
    }
    return true;
}