#include "Maze.h"
#include <random>
#include <ctime>
#include <functional>
#include <algorithm>
#include <iostream>

Maze::Maze(int width, int height) : w(width), h(height) {
    walls.resize(h, std::vector<bool>(w, true));
    traps.resize(h, std::vector<int>(w, 0));
}

void Maze::generate() {
    std::vector<std::vector<bool>> visited(h, std::vector<bool>(w, false));
    std::mt19937 gen(time(0));

    std::function<void(int, int)> dfs = [&](int x, int y) {
        visited[y][x] = true; walls[y][x] = false;
        std::vector<std::pair<int, int>> dirs = { {0,1},{0,-1},{1,0},{-1,0} };
        std::shuffle(dirs.begin(), dirs.end(), gen);
        for (auto& d : dirs) {
            int nx = x + d.first * 2, ny = y + d.second * 2;
            if (nx >= 0 && nx < w && ny >= 0 && ny < h && !visited[ny][nx]) {
                walls[y + d.second][x + d.first] = false;
                dfs(nx, ny);
            }
        }
        };
    dfs(1, 1);
    std::uniform_real_distribution<> loopDis(0, 1);
    double loopProbability = 0.12;
    double edgeLoopProbability = 0.3;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            if (walls[y][x]) {
                bool isHorizontalSeparator = (y % 2 == 0) && (x % 2 != 0);
                bool isVerticalSeparator = (y % 2 != 0) && (x % 2 == 0);

                if (isHorizontalSeparator || isVerticalSeparator) {
                    bool isEdgeWall = (x == w - 3) || (y == h - 3);
                    double currentProbability = isEdgeWall ? edgeLoopProbability : loopProbability;

                    if (loopDis(gen) < currentProbability) {
                        walls[y][x] = false;
                    }
                }
            }
        }
    }
    std::uniform_int_distribution<> randX_start(1, w - 2);
    int startX = randX_start(gen) | 1;
    int startY = h - 1;
    entrance = { startX, startY };
    walls[startY][startX] = false;

    std::uniform_int_distribution<> randX_exit(2, w - 4);
    int exitX = randX_exit(gen) | 1;
    int exitY = 0;
    walls[exitY][exitX] = false;
    exit = { exitX, exitY };
}

void Maze::generateTraps(double redP, double yellowP) {
    std::mt19937 gen(time(0));
    std::uniform_real_distribution<> dis(0, 1);
    std::uniform_int_distribution<> dirDis(0, 1);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (!walls[y][x]) {
                if (dis(gen) < redP)
                    traps[y][x] = 1;
            }
        }
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (!walls[y][x] && traps[y][x] == 0) {
                if (dis(gen) < yellowP) {
                    int direction = dirDis(gen);
                    if (direction == 0) {
                        if (x + 2 < w &&
                            !walls[y][x + 1] && traps[y][x + 1] == 0 &&
                            !walls[y][x + 2] && traps[y][x + 2] == 0) {
                            traps[y][x] = 2; traps[y][x + 1] = 2; traps[y][x + 2] = 2;
                        }
                    }
                    else {
                        if (y + 2 < h &&
                            !walls[y + 1][x] && traps[y + 1][x] == 0 &&
                            !walls[y + 2][x] && traps[y + 2][x] == 0) {
                            traps[y][x] = 2; traps[y + 1][x] = 2; traps[y + 2][x] = 2;
                        }
                    }
                }
            }
        }
    }
    traps[entrance.second][entrance.first] = 0;
    traps[exit.second][exit.first] = 0;
}
