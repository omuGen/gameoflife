#include <iostream>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h> //TODO set up text!
#include <numeric>
#include "CellMap.h"
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>

// #DONE wraparound ! doesn't really work yet! DONE Modulo problem!
// #DONE TODO click cells on/off with mouse
// #DONE TODO draw with mouse
// TODO different randomizers
// #DONE TODO pause game
// TODO template library
// #DONE TODO mouse click & movement bug #fixed properly (mousemotion event contains x/y info)
// #DONE TODO bug segfault when mouseout (while button pressed??) #dunno how, but this disappeared
// #DONE TODO Performance issues! How to track? #tracking with chrono, SDL_RENDERER_PRESENTVSYNC flag seems to have helped the fan problem
// TODO implement other universes (rules other than B3/S23)
// TODO switch infinite mode on/off
// TODO DONE fixed x/y issue - why only square grids???
// TODO de-couple window resolution and grid size

/**
 * Constants
 */
const int CELL_OFFSET = 0;
const long WINDOW_WIDTH = 1024;
const long WINDOW_HEIGHT = 768;
const int CELL_SIZE = 7;
const int CELL_BORDER = 1;
const long MENU_BAR_WIDTH = 0;
const long GRID_WIDTH = (WINDOW_WIDTH-MENU_BAR_WIDTH)/(CELL_SIZE+CELL_BORDER);
const long GRID_HEIGHT = WINDOW_HEIGHT/(CELL_SIZE+CELL_BORDER);

/**
 * Variables
 */
// combined cell size and cell border
int CELL_SIZE_F = CELL_SIZE+CELL_BORDER;
// draw the grid or don't?
bool DRAW_GRID = true;
// update after how many miliseconds?
float updateInterval = 100;
// start paused or unpaused?
bool paused = true;
// percent chance to be alive for randomizer
int randomness = 50;
// start with a random distribution or a clear screen
bool start_random = false;

// #DONE TODO bug with specific cells being alive after initialization #switched cellMap type from int to bool!
//bool (*p_currentMap)[GRID_HEIGHT];
bool cellMap[GRID_WIDTH][GRID_HEIGHT]{};
// create a new Cell Map for storing the updated values
bool newCellMap[GRID_WIDTH][GRID_HEIGHT]{};

/**
 * Initialize grid with a random distribution of cells.
 */
void initrandom(int value) {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cellMap[i][j] = rand() % 100 > value ? 0 : 1;
        }
    }
}

/**
 * Initialize grid with only empty cells.
 */ 
void initclear() {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cellMap[i][j] = 0;
        }
    }
}

void savegridtofile() {
    std::ofstream outputFile("test.txt");
    if (outputFile.is_open()) {
        std::string coords;
        for (int i = 0; i < GRID_WIDTH; ++i) {
            coords += std::to_string(i) + "\t";
            for (int j = 0; j < GRID_HEIGHT; ++j) {
                if (cellMap[i][j] == 1) {
                    coords += std::to_string(j) + "\t";
                }
            }
            coords += "\n";
        }
        outputFile << coords;
    }
    outputFile.close();
}

void loadgrid() {
    std::ifstream inputFile("test.txt");
    initclear();
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::vector<std::string> cells;
            size_t position;
            std::string cell;
            while ((position = line.find("\t")) != std::string::npos) {
                cell = line.substr(0, position);
                cells.push_back(cell);
                line.erase(0, position +1);
            }
            for (size_t i = 1; i < cells.size(); i++) {
                int cell_idx = stoi(cells[0]);
                int cell_i = stoi(cells[i]);
                cellMap[cell_idx][cell_i] = 1;
            }
        }
    }
}

// TODO rework the manpulation of the cell map
// mouse event handlers
void HandleMouseMotion(SDL_MouseMotionEvent& event) {
    int x_coord = event.x - CELL_OFFSET;
    int y_coord = event.y - CELL_OFFSET;
    //std::cout << event.state << std::endl;
    if (event.state == SDL_BUTTON_LEFT) {
        //std::cout << "HMM: left button" << std::endl;
        cellMap[x_coord/CELL_SIZE_F][y_coord/CELL_SIZE_F] = 1;
    }
    if (event.state == SDL_BUTTON_RIGHT || event.state == SDL_BUTTON_X1) { // weird sdl button for (maybe only my?) mouse right button
        //std::cout << "HMM: right button" << std::endl;
        cellMap[x_coord/CELL_SIZE_F][y_coord/CELL_SIZE_F] = 0;
    }
}
void HandleMouseButton(SDL_MouseButtonEvent& event) {
    int x_coord = event.x - CELL_OFFSET;
    int y_coord = event.y - CELL_OFFSET;
    if (event.button == SDL_BUTTON_LEFT) {
        if (event.state == SDL_PRESSED) {
            std::cout << "HMB: left button: " << event.x << ", " << event.y << std::endl;
            std::cout << "HMB: left button: " << event.x/CELL_SIZE << ", " << event.y/CELL_SIZE << std::endl;
            cellMap[x_coord/CELL_SIZE_F][y_coord/CELL_SIZE_F] = 1;
        } else if (event.state == SDL_RELEASED) {
            // do nothing
        }
    }
    if (event.button == SDL_BUTTON_RIGHT) {
        if (event.state == SDL_PRESSED) {
            //std::cout << "HMB: right button" << std::endl;
            cellMap[x_coord/CELL_SIZE_F][y_coord/CELL_SIZE_F] = 0;
        } else if (event.state == SDL_RELEASED) {
            // do nothing
        }
    }
}
/**
 * Takes SDL_Events (mouse/keyboard) and handles them.
 * @param {SDL_Event} event 
 * 
 */
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
        if (event.key.keysym.sym == SDLK_KP_PLUS
            || event.key.keysym.sym == SDLK_PLUS) {
            std::cout << "Speed increased." << std::endl;
            updateInterval -= 10;
        }
        // numpad '-' for slower
        if (event.key.keysym.sym == SDLK_KP_MINUS
            || event.key.keysym.sym == SDLK_MINUS) {
            std::cout << "Speed decreased." << std::endl;
            updateInterval += 10;
        }
        // 'r' for restart
        if (event.key.keysym.sym == SDLK_r) {
            std::cout << "Restarted." << std::endl;
            initrandom(randomness);
        }
        // 'c' to clear
        if (event.key.keysym.sym == SDLK_c) {
            std::cout << "Cleared." << std::endl;
            initclear();
        }
        // 'f' to save grid to file
        if (event.key.keysym.sym == SDLK_f) {
            std::cout << "Saved." << std::endl;
            savegridtofile();
        }
       // 'l' to load grid from file
        if (event.key.keysym.sym == SDLK_l) {
            std::cout << "Loaded." << std::endl;
            loadgrid();
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

/**
 * Takes coordinates for a specific cell and determines the neighbor status for
 * the cell found at those coordinates.
 * 
 * @param {int} i x-coordinate for the cell
 * @param {int} j y-coordinate for the cell
 */
int GetNeighborCount(int i, int j) {
    // get the values of the surrounding cells in a new array
    // only 8 neighbouring cells cuz middle cell is occupied
    bool nb[8] = {0,0,0,0,0,0,0,0};
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

void update(std::vector<int> birth, std::vector<int> survive) {
    // start time measurement
    //auto start = std::chrono::high_resolution_clock::now();
    // check the status of each cell and apply the rules
    // main update logic loop
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            // get the sum of the number of live neighbours
            int nbno = GetNeighborCount(i,j);
            // apply the rules according to the number of neighbours
            if (cellMap[i][j] == 1) {
                bool survivor = false;
                for (int i = 0; i < size(survive); i++) {
                    if (nbno == survive[i]) {
                        survivor = true;
                    }
                }
                if (survivor) {
                    newCellMap[i][j] = 1;
                } else {
                    newCellMap[i][j] = 0;
                }
            } else if (cellMap[i][j] == 0) {
                bool newborn = false;
                for (int i = 0; i < size(birth); i++) {
                    if (nbno == birth[i]) {
                        newborn = true;
                    }
                }
                if (newborn) {
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
    //auto end = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    //std::cout << "cellMap update duration: " << duration.count() << " mms." << std::endl;    
}
// struct for cli arguments
struct CLIArg {
    bool flag;
    std::string value;
};
// arg parse
CLIArg GetCLIOption(int arg_count, std::vector<std::string> arg_list, std::string option) {
    struct CLIArg result;
    result.flag = false;
    result.value = "";
    for (int i = 0; i < arg_count; i++) {
        if (arg_list[i] == option) {
            result.flag = true;
            if (i < arg_count-1) {
                result.value = arg_list[i+1];
            }
        }
    }
    return result;
}
// main 
int main(int argc, char* argv[]) {
    // handle command line arguments
    std::vector<std::string> arg_list;
    for (int i = 0; i < argc; i++) {
        arg_list.push_back(std::string(argv[i]));
    }
    // config vars (default values)
    bool random_start = false;
    int random_value = 50;
    // world rules
    std::vector<int> birth;
    std::vector<int> survive;
    // default values
    birth.push_back(2);
    survive.push_back(2);
    survive.push_back(3);
    // CLI queries
    struct CLIArg random = GetCLIOption(argc,arg_list,"--random");
    if (random.flag) {
        std::cout << "Started with 'Random' option: " << random.value << "%" << std::endl;
        random_start = true;
        random_value = std::stoi(random.value);
    }
    struct CLIArg rules = GetCLIOption(argc,arg_list,"--rulestring");
    if (rules.flag) {
        std::cout << "Started with 'Rulestring' option: " << rules.value << std::endl;
        std::string val = rules.value;
        if (val.find('B') != std::string::npos && val.find('S') != std::string::npos) {
            std::cout << "Valid rulestring found. Parsing " << val << std::endl;
            birth.clear();
            survive.clear();
            std::string birth_rule = val.substr(1,val.find("S")-1);
            std::string survive_rule = val.substr(val.find("S")+1,size(val));
            while (size(birth_rule) != 0) {
                birth.push_back(stoi(birth_rule.substr(0,1)));
                birth_rule.erase(0,1);
            }
            while (size(survive_rule) != 0) {
                survive.push_back(stoi(survive_rule.substr(0,1)));
                survive_rule.erase(0,1);
            }
        } else {
            std::cout << "Invalid rulestring. Using default B3S23." << std::endl;
        }
    }
    // init video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("gameoflife", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }
    // running with SDL_RENDERER_PRESENTVSYNC is essential for performance!
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Event Event;
    SDL_Rect rect;
    // number of ticks since the start of the game
    Uint32 ticksStart = SDL_GetTicks();
    // number of ticks at the time of the last update
    Uint32 ticksUpdate = 0;
    // number of ticks elapsed right now
    Uint32 ticksNow = ticksStart;

    // prime the randomizer
    if (random_start == true) {
        initrandom(random_value);
    } else {
        initclear();
    }

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
            update(birth, survive);
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
            for (int i = 0; i <= GRID_WIDTH; ++i) {
                //SDL_RenderDrawLine(renderer, (i*CELL_SIZE)+x_offset, 0, (i*CELL_SIZE)+x_offset, GRID_HEIGHT*CELL_SIZE);
                SDL_RenderDrawLine(renderer, (i*CELL_SIZE_F)+x_offset, 0, (i*CELL_SIZE_F)+x_offset, GRID_HEIGHT*CELL_SIZE_F);
            }
            // draw horizontal lines
            for (int i = 0; i <= GRID_HEIGHT; ++i) {
                //SDL_RenderDrawLine(renderer, 0, (i*CELL_SIZE)+y_offset, GRID_WIDTH*CELL_SIZE, (i*CELL_SIZE)+y_offset);
                SDL_RenderDrawLine(renderer, 0, (i*CELL_SIZE_F)+y_offset, GRID_WIDTH*CELL_SIZE_F, (i*CELL_SIZE_F)+y_offset);
            }            
        }
        // attempt to draw the cells
        int cell_offset = 0;
        for (int i = 0; i < GRID_WIDTH; ++i) {
            for (int j = 0; j < GRID_HEIGHT; ++j) {
                if (cellMap[i][j] == 1) {
                    // set color based on no of neighbors
                    auto nb = GetNeighborCount(i,j) + 1; //plus one to prevent 0 (scales from 1-9)
                    int opacity = (nb*16) + 119;
                    SDL_SetRenderDrawColor(renderer, opacity, opacity, opacity, SDL_ALPHA_OPAQUE);

                    //rect = {(i*CELL_SIZE)+cell_offset, (j*CELL_SIZE)+cell_offset, CELL_SIZE-CELL_BORDER, CELL_SIZE-CELL_BORDER};
                    rect = {(i*CELL_SIZE_F)+cell_offset, (j*CELL_SIZE_F)+cell_offset, CELL_SIZE_F-CELL_BORDER, CELL_SIZE_F-CELL_BORDER};
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

