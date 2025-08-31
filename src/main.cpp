#include <iostream>
#include <SDL2/SDL.h>
#include <numeric>
#include "CellMap.h"

// TODO wraparound ! doesn't really work yet!
// TODO DONE click cells on/off with mouse
// TODO DONE draw with mouse
// TODO different randomizer
// TODO DONE pause game
// TODO template library
// TODO mouse click & movement bug
// TODO bug segfault when mouseout (while button pressed??)

// TODO DONE fixed x/y issue - why only square grids???
const int CELL_SIZE = 8; // incl. +2 for the bottom + right borders
const long GRID_WIDTH = 128; // 1024/8
const long GRID_HEIGHT = 96; // 1024/8
const int WINDOW_WIDTH = (GRID_WIDTH*CELL_SIZE);
const int WINDOW_HEIGHT = (GRID_HEIGHT*CELL_SIZE);

// TODO bug with specific cells being alive after initialization
// #DONE switched cellMap type from int to bool!
//bool (*p_currentMap)[GRID_HEIGHT];
bool cellMap[GRID_WIDTH][GRID_HEIGHT]{};
// create a new Cell Map for storing the updated values
bool newCellMap[GRID_WIDTH][GRID_HEIGHT]{};

//initialize grid
void initrandom() {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cellMap[i][j] = rand() % 100 > 50 ? 0 : 1;
        }
    }
    //p_currentMap = cellMap;
}

// clear grid
void initclear() {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cellMap[i][j] = 0;
        }
    }
    //p_currentMap = cellMap;
}

// update after how many miliseconds
float updateInterval = 200;
bool paused = false;

// TODO rework the manpulation of the cell map
// mouse event handlers
void HandleMouseMotion(SDL_MouseMotionEvent& event) {
    if (event.state == SDL_BUTTON_LEFT) {
        cellMap[event.x/CELL_SIZE][event.y/CELL_SIZE] = 1;
    } else if (event.state == SDL_BUTTON_RIGHT) {
        cellMap[event.x/CELL_SIZE][event.y/CELL_SIZE] = 0;
    }
}
void HandleMouseButton(SDL_MouseButtonEvent& event) {
    if (event.button == SDL_BUTTON_LEFT) {
        if (event.state == SDL_PRESSED) {
            cellMap[event.x/CELL_SIZE][event.y/CELL_SIZE] = 1;
        } else if (event.state == SDL_RELEASED) {
            // do nothing
        }
    } else if (event.button == SDL_BUTTON_RIGHT) {
        if (event.state == SDL_PRESSED) {
            cellMap[event.x/CELL_SIZE][event.y/CELL_SIZE] = 0;
        } else if (event.state == SDL_RELEASED) {
            // do nothing
        }
    }
}

void HandleEvents(SDL_Event& event) {
    // keyup events
    if (event.type == SDL_KEYUP) {
        // 'space' for pause
        if (event.key.keysym.sym == SDLK_SPACE) {
            if (!paused) {
                std::cout << "Game paused." << std::endl;
                paused = true;
            } else {
                std::cout << "Game resumed." << std::endl;
                paused = false;                        
            }
        }
        // numpad '+' for faster
        if (event.key.keysym.sym == SDLK_KP_PLUS) {
            std::cout << "Speed increased." << std::endl;
            updateInterval -= 10;
        }
        // numpad '-' for slower
        if (event.key.keysym.sym == SDLK_KP_MINUS) {
            std::cout << "Speed decreased." << std::endl;
            updateInterval += 10;
        }
        // 'r' for restart
        if (event.key.keysym.sym == SDLK_r) {
            std::cout << "Restarted." << std::endl;
            initrandom();
        }
        // 'c' to clear
        if (event.key.keysym.sym == SDLK_c) {
            std::cout << "Cleared." << std::endl;
            initclear();
        }                
    }
    //mouse events
    if (event.type == SDL_MOUSEMOTION) {
        HandleMouseMotion(event.motion);
    }
    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
        HandleMouseButton(event.button);
    }
}

// update
void update() {

    // local vars for grid height and width
    int gw = GRID_WIDTH;
    int gh = GRID_HEIGHT;

    // check the status of each cell and apply the rules
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            // get the values of the surrounding cells in a new array
            bool nb[8]{}; // = {0,0,0,0,0,0,0,0}; // only 8 neighbouring cells, middle cell is occupied
          
            nb[0] = cellMap[(i-1) % gw][(j-1) % gh]; // NW
            nb[1] = cellMap[i % gw][(j-1) % gh]; // N
            nb[2] = cellMap[(i+1) % gw][(j-1) % gh]; // NE
            nb[3] = cellMap[(i-1) % gw][j % gh]; // W
            nb[4] = cellMap[(i+1) % gw][j % gh]; // E
            nb[5] = cellMap[(i-1) % gw][(j+1) % gh]; // SW
            nb[6] = cellMap[i % gw][(j+1) % gh]; // S
            nb[7] = cellMap[(i+1) % gw][(j+1) % gh]; // SE

            // check for the number of live neighbours!
            int nbno = std::accumulate(std::begin(nb), std::end(nb), 0);
            //std::cout << nbno << std::endl;
            // apply the rules according to the number of neighbours
            if (cellMap[i][j] == 1) {
                // check for death conditions
                if (nbno < 2) {
                    // each live cell with less than two neighbours dies
                    newCellMap[i][j] = 0;
                } else if (nbno > 3) {
                    // each live cell with more than three neighbours dies
                    newCellMap[i][j] = 0;
                } else {
                    // set the new cell to alive if the old cell was alive
                    newCellMap[i][j] = 1;
                }
                // if it's still alive here, it stays alive
            } else if (cellMap[i][j] == 0) {
                // if it's dead and has exactly three neighbours, it comes alive!
                if (nbno == 3) {
                    newCellMap[i][j] = 1;
                } else {
                    newCellMap[i][j] = 0;
                }
            }
        }
    }
    // replace cellMap with updated newCellMap
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cellMap[i][j] = newCellMap[i][j];
        }
    }
}

// main 
int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("gameoflife", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }

    //bool active = true; // TODO do i need this?
    SDL_Event Event;

    SDL_Rect rect;

    // number of ticks since the start of the game
    Uint32 ticksStart = SDL_GetTicks();
    // number of ticks at the time of the last update
    Uint32 ticksUpdate = 0;
    // number of ticks elapsed right now
    Uint32 ticksNow = ticksStart;

    // prime the randomizer
    //srand(time(0));
    //CellGrid grid;
    initrandom();

    // TEST
    CellMap test(GRID_WIDTH, GRID_HEIGHT);

    test.initialize();
    test.update();



    // main loop
    while (true) {
        // check for events
        if (SDL_PollEvent(&Event)) {
            // handle events
            if (Event.type == SDL_QUIT) {
                break;
            }
            HandleEvents(Event);
        }

        // refresh the current tick count since the game started        
        ticksNow = SDL_GetTicks();
        // only update once per updateInterval
        if (((ticksNow - ticksUpdate) > updateInterval) && !paused) {
            // update live/dead status for all cells
            update();
            // record time of this update
            ticksUpdate = SDL_GetTicks();
        }
        // set color to black and color everything
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        // attempt to draw the grid
        for (int i = 0; i < GRID_WIDTH; ++i) {
            for (int j = 0; j < GRID_HEIGHT; ++j) {
                if (cellMap[i][j] == 1) {
                    rect = {i*CELL_SIZE, j*CELL_SIZE, CELL_SIZE-2, CELL_SIZE-2};
                    // set color to white
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                    SDL_RenderDrawRect(renderer, &rect);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        // present the rendered grid to the window
        SDL_RenderPresent(renderer);
    }
    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

