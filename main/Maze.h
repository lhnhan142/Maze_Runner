#pragma once
#include <vector>
#include <utility> // For std::pair

// =================== CLASS MÊ CUNG ===================
class Maze {
public:
    int w, h;
    std::vector<std::vector<bool>> walls;
    std::vector<std::vector<int>> traps;
    std::pair<int, int> entrance, exit;
    std::pair<int, int> key;

    Maze(int width, int height);

    void generate();
    void generateTraps(double redP, double yellowP);
};