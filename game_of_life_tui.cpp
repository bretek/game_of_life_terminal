#include "game_of_life_tui.h"

#define UP 'w'
#define DOWN 'r'
#define LEFT 'a'
#define RIGHT 's'
#define TOGGLE_CELL ' '
#define STEP_GAME 'f'

#define TITLE_WINDOW_HEIGHT 1

int main() {

    initscr();
    noecho();
    cbreak();
    raw();
    clear();

    WINDOW *titleWin = NULL, *mainWin = NULL, *borderWin = NULL;

    // title window
    titleWin = newwin(TITLE_WINDOW_HEIGHT, COLS, 0, 0);
    // centered text
    wmove(titleWin, 0, (COLS/2) - 6);
    wattron(titleWin, A_BOLD);
    waddstr(titleWin, "Game of Life");
    wattroff(titleWin, A_BOLD);

    // main display border window
    borderWin = newwin(LINES - TITLE_WINDOW_HEIGHT, COLS, TITLE_WINDOW_HEIGHT, 0);
    box(borderWin, 0, 0);

    // main display window
    mainWin = newwin(LINES - TITLE_WINDOW_HEIGHT - 2, COLS - 2, TITLE_WINDOW_HEIGHT + 1, 1);
    int cursX = COLS/2, cursY = (LINES - TITLE_WINDOW_HEIGHT - 2) / 2;
    wmove(mainWin, cursY, cursX);

    //refresh all
    refresh();
    wrefresh(titleWin);
    wrefresh(borderWin);
    wrefresh(mainWin);

    viewport view;
    view.x = 0;
    view.y = 0;
    view.width = COLS - 2;
    view.height = LINES - TITLE_WINDOW_HEIGHT - 2;

    std::vector<cell> cells(0);
    
    updateInput(mainWin, &cursY, &cursX, cells, &view);

    endwin();

    return 0;
}

void updateInput(WINDOW *mainWin, int *cursY, int *cursX, std::vector<cell> cells, viewport *view) {
    int ch;

    while ((ch = getch()) != 'q') {
        int update = false;

        switch (ch) {
            // left
            case LEFT:
                (*cursX)--;
                if (*cursX < view->x) {
                    view->x--;
                    renderCells(mainWin, *cursX, *cursY, cells, *view);
                }
                break;
            // right
            case RIGHT:
                (*cursX)++;
                if (*cursX >= view->x + view->width) {
                    view->x++;
                    renderCells(mainWin, *cursX, *cursY, cells, *view);
                }
                break;

            // up
            case UP:
                (*cursY)--;
                if (*cursY < view->y) {
                    view->y--;
                    renderCells(mainWin, *cursX, *cursY, cells, *view);
                }
                break;
            // down
            case DOWN:
                (*cursY)++;
                if (*cursY >= view->y + view->height) {
                    view->y++;
                    renderCells(mainWin, *cursX, *cursY, cells, *view);
                }
                break;

            // toggle cell
            case TOGGLE_CELL:
                if (findCellByCoords(cells, *cursX, *cursY)) {
                    removeCellByCoords(&cells, *cursX, *cursY);
                    waddch(mainWin, ' ');
                }
                else {
                    cell newCell;
                    newCell.x = *cursX;
                    newCell.y = *cursY;
                    cells.push_back(newCell);
                }
                renderCells(mainWin, *cursX, *cursY, cells, *view);
                break;

            // step game
            case STEP_GAME:
                std::vector<cell> newCells = tickGoL(cells);
                cells.clear();
                cells = newCells;
                renderCells(mainWin, *cursX, *cursY, cells, *view);
                break;
        }

        wmove(mainWin, *cursY - view->y, *cursX - view->x);
        wrefresh(mainWin);
    }
}

void renderCells(WINDOW *mainWin, const int &cursY, const int &cursX, std::vector<cell> &cells, const viewport &view) {
    wclear(mainWin);

    for (std::vector<cell>::iterator curCell = cells.begin(); curCell != cells.end(); ++curCell) {
        if (curCell->y - view.y >= 0 && curCell->y - view.y < view.height && curCell->x - view.x >= 0 && curCell->x - view.x < view.width) {
            wmove(mainWin, curCell->y - view.y, curCell->x - view.x);
            waddch(mainWin, '#');
        }
    }

    wmove(mainWin, cursY - view.y, cursX - view.x);

    wrefresh(mainWin);
}

void removeCellByCoords(std::vector<cell> *cells, const int x, const int y) {
    std::vector<cell>::iterator cellToDelete;
    for (std::vector<cell>::iterator curCell = cells->begin(); curCell != cells->end(); ++curCell) {
        if (curCell->x == x && curCell->y == y) {
            cellToDelete = curCell;
            break;
        }
    }
    cells->erase(cellToDelete);
}

const bool findCellByCoords(const std::vector<cell> &cells, const int x, const int y) {
    for (int i = 0; i < cells.size(); ++i) {
        if (cells[i].x == x && cells[i].y == y) {
            return true;
        }
    }
    return false;
}

const int findNumNeighbours(const std::vector<cell> &cells, const int x, const int y) {
    int numNeighbours = 0;

    for (int r = -1; r <= 1; ++r) {
        for (int c = -1; c <= 1; ++c) {
            //check not self
            if (findCellByCoords(cells, x + c, y + r) && !(r == 0 && c == 0)) {
                numNeighbours++;
            }
        }
    }

    return numNeighbours;
}

std::vector<cell> tickGoL(const std::vector<cell> &cells) {
    std::vector<cell> newCells(0);

    for (int i = 0; i < cells.size(); ++i) {
        cell currentCell = cells[i];
        int numNeighbours = 0;

        for (int r = -1; r <= 1; ++r) {
            for (int c = -1; c <= 1; ++c) {
                //check not self
                if (r == 0 && c == 0) {
                    continue;
                }
                else if (findCellByCoords(cells, currentCell.x + c, currentCell.y + r)) {
                    numNeighbours++;
                }
                else {
                    //check empty neighbours then birth cell if hasn't been birthed already
                    if (findNumNeighbours(cells, currentCell.x + c, currentCell.y + r) == 3 
                    && !findCellByCoords(newCells, currentCell.x + c, currentCell.y + r)) {
                        cell newCell;
                        newCell.x = currentCell.x + c;
                        newCell.y = currentCell.y + r;
                        newCells.push_back(newCell);
                    }
                }
            }
        }
        //keep cells alive
        if (numNeighbours >= 2 && numNeighbours <= 3) {
            newCells.push_back(currentCell);
        }
    }

    return newCells;
}