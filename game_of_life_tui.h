#include <ncurses.h>
#include <vector>
#include <unistd.h>
#include <thread>
#include <algorithm>

struct cell {
    int x, y;
};

struct viewport {
    int x, y;
    int width, height;
};

// graphics
void updateInput(WINDOW *mainWin, int *cursY, int *cursX, std::vector<cell> cells, viewport *view);
void renderCells(WINDOW *mainWin, const int &cursY, const int &cursX, std::vector<cell> &cells, const viewport &view);

// logic
void removeCellByCoords(std::vector<cell> *cells, const int x, const int y);
const bool findCellByCoords(const std::vector<cell> &cells, const int x, const int y);
const int findNumNeighbours(const std::vector<cell> &cells, const int x, const int y);
std::vector<cell> tickGoL(const std::vector<cell> &cells);