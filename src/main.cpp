#include <iostream>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h> //TODO set up text!
#include <numeric>
#include "CellMap.h"
#include <chrono>

// #DONE wraparound ! doesn't really work yet! DONE Modulo problem!
// #DONE TODO click cells on/off with mouse
// #DONE TODO draw with mouse
// TODO different randomizer
// #DONE TODO pause game
// TODO template library
// #DONE TODO mouse click & movement bug #fixed properly (mousemotion event contains x/y info)
// #DONE TODO bug segfault when mouseout (while button pressed??) #dunno how, but this disappeared
// #DONE TODO Performance issues! How to track? #tracking with chrono, SDL_RENDERER_PRESENTVSYNC flag seems to have helped the fan problem

// TODO DONE fixed x/y issue - why only square grids???
const int CELL_SIZE = 8; // incl. +2 for the bottom + right borders
// TODO de-couple window resolution and grid size
const long GRID_WIDTH = 128; // 1024/8
const long GRID_HEIGHT = 96; // 768/8
const int WINDOW_WIDTH = (GRID_WIDTH*CELL_SIZE);
const int WINDOW_HEIGHT = (GRID_HEIGHT*CELL_SIZE);
const int CELL_BORDER = 1;
const bool DRAW_GRID = true;

// #DONE TODO bug with specific cells being alive after initialization #switched cellMap type from int to bool!
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
    //std::cout << event.state << std::endl;
    if (event.state == SDL_BUTTON_LEFT) {
        //std::cout << "HMM: left button" << std::endl;
        cellMap[event.x/CELL_SIZE][event.y/CELL_SIZE] = 1;
    }
    if (event.state == SDL_BUTTON_RIGHT || event.state == SDL_BUTTON_X1) { // weird sdl button for (maybe only my?) mouse right button
        //std::cout << "HMM: right button" << std::endl;
        cellMap[event.x/CELL_SIZE][event.y/CELL_SIZE] = 0;
    }
}
void HandleMouseButton(SDL_MouseButtonEvent& event) {
    if (event.button == SDL_BUTTON_LEFT) {
        if (event.state == SDL_PRESSED) {
            //std::cout << "HMB: left button" << std::endl;
            cellMap[event.x/CELL_SIZE][event.y/CELL_SIZE] = 1;
        } else if (event.state == SDL_RELEASED) {
            // do nothing
        }
    }
    if (event.button == SDL_BUTTON_RIGHT) {
        if (event.state == SDL_PRESSED) {
            //std::cout << "HMB: right button" << std::endl;
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
    if (event.type == SDL_MOUSEBUTTONDOWN /*|| event.type == SDL_MOUSEBUTTONUP*/) {
        HandleMouseButton(event.button);
    }
}

int GetNeighborCount(int i, int j) {
    // get the values of the surrounding cells in a new array
    bool nb[8] = {0,0,0,0,0,0,0,0}; // only 8 neighbouring cells, middle cell is occupied
    // go around the clock and determine neighbors alive/dead status
    nb[0] = cellMap[((i-1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j-1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
    nb[1] = cellMap[((i % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j-1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
    nb[2] = cellMap[((i+1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j-1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
    nb[3] = cellMap[((i-1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
    nb[4] = cellMap[((i+1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
    nb[5] = cellMap[((i-1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j+1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
    nb[6] = cellMap[((i % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j+1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
    nb[7] = cellMap[((i+1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j+1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW           
    // get the sum of the number of live neighbours
    int nbno = std::accumulate(std::begin(nb), std::end(nb), 0);
    return nbno;
}


// update
void update() {

    // local vars for grid height and width
    //int gw = GRID_WIDTH;
    //int gh = GRID_HEIGHT;

    // start time measurement
    //auto start = std::chrono::high_resolution_clock::now();
    // check the status of each cell and apply the rules
    // main update logic loop
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            // get the values of the surrounding cells in a new array
            bool nb[8] = {0,0,0,0,0,0,0,0}; // only 8 neighbouring cells, middle cell is occupied
            // go around the clock and determine neighbors alive/dead status
            nb[0] = cellMap[((i-1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j-1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
            nb[1] = cellMap[((i % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j-1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
            nb[2] = cellMap[((i+1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j-1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
            nb[3] = cellMap[((i-1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
            nb[4] = cellMap[((i+1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
            nb[5] = cellMap[((i-1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j+1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
            nb[6] = cellMap[((i % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j+1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW
            nb[7] = cellMap[((i+1 % GRID_WIDTH) + GRID_WIDTH) % GRID_WIDTH][((j+1 % GRID_HEIGHT) + GRID_HEIGHT) % GRID_HEIGHT]; // NW           
            // get the sum of the number of live neighbours
            int nbno = std::accumulate(std::begin(nb), std::end(nb), 0);
            // apply the rules according to the number of neighbours
            if (cellMap[i][j] == 1) {
                if (nbno < 2 || nbno > 3) {
                    // each live cell with less than two or more than 3 neighbours dies
                    newCellMap[i][j] = 0;
                } else {
                    // preserve status
                    newCellMap[i][j] = 1;
                }
            } else if (cellMap[i][j] == 0) {
                if (nbno == 3) {
                    // if cell has exactly three neighbours, it comes alive
                    newCellMap[i][j] = 1;
                } else {
                    // preserve status
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
    //auto end = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    //std::cout << "cellMap update duration: " << duration.count() << " mms." << std::endl;    
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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
    //CellMap test(GRID_WIDTH, GRID_HEIGHT);
    //test.initialize();
    //test.update();

    /* MAIN LOOP ******************************************************************/
    while (true) {
        // check for events
        while (SDL_PollEvent(&Event)) {
            // handle events
            if (Event.type == SDL_QUIT) {
                //break;
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }
            HandleEvents(Event);
        }

        // refresh the current tick count since the game started        
        ticksNow = SDL_GetTicks();
        // only update once per updateInterval
        if (((ticksNow - ticksUpdate) > updateInterval) && !paused) {
            //std::cout << "updating after: " << ticksNow - ticksUpdate << std::endl;
            // update live/dead status for all cells
            update();
            // record time of this update
            ticksUpdate = SDL_GetTicks();
        }
        // TODO Problem: moving the render code into the ticks-if clause interferes with mouse interaction
        // however, having it outside produces too many unneeded renders...
        // start time measurement
        // auto startRender = std::chrono::high_resolution_clock::now();
        // set color to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        // inititalize backbuffer (to bgcolor)
        SDL_RenderClear(renderer);
        // draw a grid
        if (DRAW_GRID == true) {
            int x_offset = 0;
            int y_offset = 0;
            // set grid color
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
            // draw vertical lines
            for (int i = 0; i < GRID_WIDTH; ++i) {
                SDL_RenderDrawLine(renderer, (i*CELL_SIZE)+x_offset, 0, (i*CELL_SIZE)+x_offset, GRID_HEIGHT*CELL_SIZE);
            }
            // draw horizontal lines
            for (int i = 0; i < GRID_WIDTH; ++i) {
                SDL_RenderDrawLine(renderer, 0, (i*CELL_SIZE)+y_offset, GRID_WIDTH*CELL_SIZE, (i*CELL_SIZE)+y_offset);
            }            
        }
        // attempt to draw the cells
        int cell_offset = 1;
        for (int i = 0; i < GRID_WIDTH; ++i) {
            for (int j = 0; j < GRID_HEIGHT; ++j) {
                if (cellMap[i][j] == 1) {
                    // set color based on no of neighbors
                    auto nb = GetNeighborCount(i,j) + 1; //plus one to prevent 0 (scales from 1-9)
                    int opacity = (nb*16) + 119;
                    SDL_SetRenderDrawColor(renderer, opacity, opacity, opacity, SDL_ALPHA_OPAQUE);

                    rect = {(i*CELL_SIZE)+cell_offset, (j*CELL_SIZE)+cell_offset, CELL_SIZE-CELL_BORDER, CELL_SIZE-CELL_BORDER};
                    // set color to white
                    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                    SDL_RenderDrawRect(renderer, &rect);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        // present the rendered grid to the window
        SDL_RenderPresent(renderer);
        // auto endRender = std::chrono::high_resolution_clock::now();
        // auto durationRender = std::chrono::duration_cast<std::chrono::microseconds>(endRender-startRender);
        // std::cout << "render duration: " << durationRender.count() << " mms." << std::endl;          

    }
    /* MAIN LOOP: END *************************************************************/
    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

