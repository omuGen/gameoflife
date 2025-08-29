#include <iostream>
#include <SDL2/SDL.h>
#include <numeric>

// TODO wraparound
// TODO click cells on/off with mouse
// TODO draw with mouse
// TODO different randomizer
// TODO pause game
// TODO template library

// TODO why only square grids???
const int CELL_SIZE = 8; // incl. +2 for the bottom + right borders
const int GRID_HEIGHT = 128; // 768/8
const int GRID_WIDTH = 128; // 1024/8
const int WINDOW_HEIGHT = (GRID_HEIGHT*CELL_SIZE);
const int WINDOW_WIDTH = (GRID_WIDTH*CELL_SIZE);

int cellMap[GRID_HEIGHT][GRID_WIDTH];
// create a new Cell Map for storing the updated values
int newCellMap[GRID_HEIGHT][GRID_WIDTH];

//initialize grid
void initialize() {
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            cellMap[i][j] = rand() % 100 > 50;
        }
    }
}

// update
void update() {
    // check the status of each cell and apply the rules
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            // get the values of the surrounding cells in a new array
            bool nb[8] = {0,0,0,0,0,0,0,0}; // only 8 neighbouring cells, middle cell is occupied
            
            // local vars for grid height and width
            int gh = GRID_HEIGHT-1;
            int gw = GRID_WIDTH-1;

            // assign cellMap neighbours values to neighbours array
            nb[0] = (i-1 < 0 || j-1 < 0) ? 0 : cellMap[i-1][j-1]; // NW
            nb[1] = (i-1 < 0) ? 0 : cellMap[i-1][j]; // N
            nb[2] = (i-1 < 0 || j+1 > gw) ? 0 : cellMap[i-1][j+1]; // NE
            nb[3] = (j-1 < 0) ? 0 : cellMap[i][j-1]; // W
            nb[4] = (j+1 > gw) ? 0 : cellMap[i][j+1]; // E
            nb[5] = (i+1 > gh || j-1 < 0) ? 0 : cellMap[i+1][j-1]; // SW
            nb[6] = (i+1 > gh) ? 0 : cellMap[i+1][j]; // S
            nb[7] = (i+1 > gh || j+1 > gw) ? 0 : cellMap[i+1][j+1]; //SE
            
            // check for the number of live neighbours!
            int nbno = std::accumulate(std::begin(nb), std::end(nb), 0);
            //std::cout << nbno << std::endl;
            // apply the rules according to the number of neighbours
            if (cellMap[i][j] == 1) {
                // set the new cell to alive if the old cell was alive
                newCellMap[i][j] = 1;
                // check for death conditions
                if (nbno < 2) {
                    // each live cell with less than two neighbours dies
                    newCellMap[i][j] = 0;
                } else if (nbno > 3) {
                    // each live cell with more than three neighbours dies
                    newCellMap[i][j] = 0;
                }
                // if it's still alive here, it stays alive
            } else if (cellMap[i][j] == 0) {
                // if it's dead and has exactly three neighbours, it comes alive!
                if (nbno == 3) {
                    newCellMap[i][j] = 1;
                }
            }
        }
    }
    // replace cellMap with updated newCellMap
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }

    //bool active = true; // TODO do i need this?
    SDL_Event Event;
    // prime the randomizer
    srand(time(0));
    //CellGrid grid;
    initialize();
    // number of ticks since the start of the game
    Uint32 ticksStart = SDL_GetTicks();
    // number of ticks at the time of the last update
    Uint32 ticksUpdate = 0;
    // number of ticks elapsed right now
    Uint32 ticksNow = ticksStart;
    // update after how many miliseconds
    float updateInterval = 100;

    bool paused = false;

    // main loop
    while (true) {
        // check for events
        if (SDL_PollEvent(&Event)) {
            // handle events
            if (Event.type == SDL_QUIT) {
                break;
            }
            if (Event.type == SDL_KEYUP) {
                std::cout << "Key: " << Event.key.keysym.sym << std::endl;
                if (Event.key.keysym.sym == SDLK_p) {
                    if (!paused) {
                        std::cout << "Game paused." << std::endl;
                        paused = true;
                    } else {
                        std::cout << "Game resumed." << std::endl;
                        paused = false;                        
                    }
                }
            }
        }
        // refresh the current tick count since the game started        
        ticksNow = SDL_GetTicks();
        // only update once per updateInterval
        if (((ticksNow - ticksUpdate) > updateInterval) && !paused) {
            // update live/dead status for all cells
            update();
            // set color to black and color everything
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);
            // attempt to draw the grid
            for (int i = 0; i < GRID_HEIGHT; ++i) {
                for (int j = 0; j < GRID_WIDTH; ++j) {
                    if (cellMap[i][j] == 1) {
                        SDL_Rect rect = {
                            i*CELL_SIZE, // x
                            j*CELL_SIZE, // y
                            CELL_SIZE-2, // w
                            CELL_SIZE-2  // h
                        };
                        // set color to white
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawRect(renderer, &rect);
                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }
            // present the rendered grid to the window
            SDL_RenderPresent(renderer);
            // record time of this update
            ticksUpdate = SDL_GetTicks();
        }
    }
    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}