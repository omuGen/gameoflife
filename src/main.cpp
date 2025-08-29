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
const int GRID_WIDTH = 128; // 1024/8
const int GRID_HEIGHT = 96; // 1024/8
const int WINDOW_WIDTH = (GRID_WIDTH*CELL_SIZE);
const int WINDOW_HEIGHT = (GRID_HEIGHT*CELL_SIZE);

int cellMap[GRID_WIDTH][GRID_HEIGHT];
// create a new Cell Map for storing the updated values
int newCellMap[GRID_WIDTH][GRID_HEIGHT];

//initialize grid
void initialize() {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cellMap[i][j] = rand() % 100 > 50;
        }
    }
}

//initialize grid
void initializeEmpty() {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cellMap[i][j] = 0;
        }
    }
}


int mouseX;
int mouseY;
bool mouseButtonDown = false;

void HandleMouseMotion(SDL_MouseMotionEvent& Event) {
    //std::cout << "Mouse moved." << std::endl;
    mouseX = Event.x;
    mouseY = Event.y;
    if (mouseButtonDown == true) {
        int cellX = mouseX/CELL_SIZE;
        int cellY = mouseY/CELL_SIZE;
        cellMap[cellX][cellY] = 1;
    }
}

void HandleMouseClick(SDL_MouseButtonEvent& Event) {
    if (Event.type == SDL_MOUSEBUTTONDOWN) {
        std::cout << "Left MB pressed." << std::endl;
        mouseButtonDown = true;
    } else if (Event.type == SDL_MOUSEBUTTONUP) {
        std::cout << "Left MB released." << std::endl;
        mouseButtonDown = false;
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
            bool nb[8]; // = {0,0,0,0,0,0,0,0}; // only 8 neighbouring cells, middle cell is occupied
          
            nb[0] = cellMap[(i-1) % gw][(j-1) % gh]; // NW
            nb[1] = cellMap[i % gw][(j-1) % gh]; // N
            nb[2] = cellMap[(i+1) % gw][(j-1) % gh]; // NE
            nb[3] = cellMap[(i-1) % gw][j % gh]; // W
            nb[4] = cellMap[(i+1) % gw][j % gh]; // E
            nb[5] = cellMap[(i-1) % gw][(j+1) % gh]; // SW
            nb[6] = cellMap[i % gw][(j+1) % gh]; // S
            nb[7] = cellMap[(i+1) % gw][(j+1) % gh]; // SE

            /*
            nb[0] = cellMap[(i-1)%gh][(j-1)%gw]; // NW
            nb[1] = cellMap[(i-1)%gh][j%gw];   // N
            nb[2] = cellMap[(i-1)%gh][(j+1)%gw]; // NE
            nb[3] = cellMap[i%gh][(j-1)%gw];   // W
            nb[4] = cellMap[i%gh][(j+1)%gw];   // E
            nb[5] = cellMap[(i+1)%gh][(j-1)%gw]; // SW
            nb[6] = cellMap[(i+1)%gh][j%gw];   // S
            nb[7] = cellMap[(i+1)%gh][(j+1)%gw]; // SE
            */

            /*
            int wrap = 0;

            // assign cellMap neighbours values to neighbours array
            // this assumes that out of bounds cells are dead
            if (i != 0 && j != 0 && i != gh && j != gw) {
                // if neither column nor row are first or last/top or bottom, normal rules apply
                nb[0] = cellMap[i-1][j-1]; // NW
                nb[1] = cellMap[i-1][j];   // N
                nb[2] = cellMap[i-1][j+1]; // NE
                nb[3] = cellMap[i][j-1];   // W
                nb[4] = cellMap[i][j+1];   // E
                nb[5] = cellMap[i+1][j-1]; // SW
                nb[6] = cellMap[i+1][j];   // S
                nb[7] = cellMap[i+1][j+1]; // SE
            } else if (i == 0 && (j != 0 && j != gw)) {
                // wrap row from top to bottom
                int wrapToBottom = gh + 1;
                nb[0] = cellMap[wrapToBottom+i-1][j-1]; // NW
                nb[1] = cellMap[wrapToBottom+i-1][j];   // N
                nb[2] = cellMap[wrapToBottom+i-1][j+1]; // NE
                nb[3] = cellMap[i][j-1];   // W
                nb[4] = cellMap[i][j+1];   // E
                nb[5] = cellMap[i+1][j-1]; // SW
                nb[6] = cellMap[i+1][j];   // S
                nb[7] = cellMap[i+1][j+1]; // SE
            } else if (i == gh && (j != 0 && j != gw)) {
                // wrap row from bottom to top
                int wrapToTop = -gh + 1;                
                nb[0] = cellMap[i-1][j-1]; // NW
                nb[1] = cellMap[i-1][j];   // N
                nb[2] = cellMap[i-1][j+1]; // NE
                nb[3] = cellMap[i][j-1];   // W
                nb[4] = cellMap[i][j+1];   // E
                nb[5] = cellMap[wrapToTop+i+1][j-1]; // SW
                nb[6] = cellMap[wrapToTop+i+1][j];   // S
                nb[7] = cellMap[wrapToTop+i+1][j+1]; // SE
            } else if (j == 0 && (i != 0 && i != gh)) {
                // wrap row from the left column to the right
                int wrapToRight = gw + 1;
                nb[0] = cellMap[i-1][wrapToRight+j-1]; // NW
                nb[1] = cellMap[i-1][j];   // N
                nb[2] = cellMap[i-1][j+1]; // NE
                nb[3] = cellMap[i][wrapToRight+j-1];   // W
                nb[4] = cellMap[i][j+1];   // E
                nb[5] = cellMap[i+1][wrapToRight+j-1]; // SW
                nb[6] = cellMap[i+1][j];   // S
                nb[7] = cellMap[i+1][j+1]; // SE
            } else if (j == gw && (i != 0 && i != gh)) {
                // wrap from the right colum to the left
                int wrapToLeft = -gw + 1;
                nb[0] = cellMap[i-1][j-1]; // NW
                nb[1] = cellMap[i-1][j];   // N
                nb[2] = cellMap[i-1][wrapToLeft+j+1]; // NE
                nb[3] = cellMap[i][j-1];   // W
                nb[4] = cellMap[i][wrapToLeft+j+1];   // E
                nb[5] = cellMap[i+1][j-1]; // SW
                nb[6] = cellMap[i+1][j];   // S
                nb[7] = cellMap[i+1][wrapToLeft+j+1]; // SE                
            }
            if (i == 0 && j == 0) {
                nb[0] = cellMap[gh][gw];
            }
            if (i == 0 && j == gw) {
                nb[2] = cellMap[gh][0];
            }
            if (i == gh && j == 0) {
                nb[5] = cellMap[0][gw];
            }
            if (i == gh && j == gw) {
                nb[7] = cellMap[0][0];
            }
            */


                // if the row [i] is the first/top [0], wrap around to the last/bottom [gh] 
                // if the row [i] is the last/bottom [gh], wrap around to the first/top [0]

                // if the column [j] is the first/left [0], wrap around to the last/right [gw] 
                // if the column [j] is the last/right [gw], wrap around to the first/left [0] 



            // if both column and row are either first or last/top or bottom, other rules apply

            /*
            nb[0] = (i-1 < 0 || j-1 < 0) ? 0 : cellMap[i-1][j-1]; // NW
            nb[1] = (i-1 < 0) ? 0 : cellMap[i-1][j]; // N
            nb[2] = (i-1 < 0 || j+1 > gw) ? 0 : cellMap[i-1][j+1]; // NE
            nb[3] = (j-1 < 0) ? 0 : cellMap[i][j-1]; // W
            nb[4] = (j+1 > gw) ? 0 : cellMap[i][j+1]; // E
            nb[5] = (i+1 > gh || j-1 < 0) ? 0 : cellMap[i+1][j-1]; // SW
            nb[6] = (i+1 > gh) ? 0 : cellMap[i+1][j]; // S
            nb[7] = (i+1 > gh || j+1 > gw) ? 0 : cellMap[i+1][j+1]; //SE
            */

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
                newCellMap[i][j] = 0;
                // if it's dead and has exactly three neighbours, it comes alive!
                if (nbno == 3) {
                    newCellMap[i][j] = 1;
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
            // keyup events
            if (Event.type == SDL_KEYUP) {
                //std::cout << "Key: " << Event.key.keysym.sym << std::endl;
                // 'P' for pause
                if (Event.key.keysym.sym == SDLK_SPACE) {
                    if (!paused) {
                        std::cout << "Game paused." << std::endl;
                        paused = true;
                    } else {
                        std::cout << "Game resumed." << std::endl;
                        paused = false;                        
                    }
                }
                // 'f' for faster
                if (Event.key.keysym.sym == SDLK_f || Event.key.keysym.sym == SDLK_KP_PLUS) {
                    std::cout << "Speed increased." << std::endl;
                    updateInterval -= 10;
                }
                // 's' for slower
                if (Event.key.keysym.sym == SDLK_s || Event.key.keysym.sym == SDLK_KP_MINUS) {
                    std::cout << "Speed decreased." << std::endl;
                    updateInterval += 10;
                }
                // 'r' for restart
                if (Event.key.keysym.sym == SDLK_r) {
                    std::cout << "Restarted." << std::endl;
                    initialize();
                }
                // 'c' to clear
                if (Event.key.keysym.sym == SDLK_c) {
                    std::cout << "Cleared." << std::endl;
                    initializeEmpty();
                }                
            }
            //mouse events
            if (Event.type == SDL_MOUSEMOTION) {
                HandleMouseMotion(Event.motion);
            }
            if (Event.type == SDL_MOUSEBUTTONDOWN || Event.type == SDL_MOUSEBUTTONUP) {
                HandleMouseClick(Event.button);
            }
        }

            // set color to black and color everything
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);
            // attempt to draw the grid
            for (int i = 0; i < GRID_WIDTH; ++i) {
                for (int j = 0; j < GRID_HEIGHT; ++j) {
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

        // refresh the current tick count since the game started        
        ticksNow = SDL_GetTicks();
        // only update once per updateInterval
        if (((ticksNow - ticksUpdate) > updateInterval) && !paused) {
            // update live/dead status for all cells
            update();

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