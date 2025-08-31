// CellMap.h
#ifndef CELLMAP_H
#define CELLMAP_H

#include <iostream>
#include <vector>
#include <list>
#include <numeric>
#include <chrono>

class CellMap {

    private:
        // grid with cols = x, rows = y
        std::vector<std::vector<long>> grid{};
        long cols{};
        long rows{};

        // list of neighbors
        bool neighbors[8] = {};
        // short neighbor_no; declared locally
        // list of cells that need to be updated
        std::list<std::vector<long>> update_cells;
        // state of the cell
        //bool cell_status; //declared locally

        // get the alive/dead status for all 8 neighbors and return their sum
        short get_neighbors(long x, long y) {
            //std::cout << "get_nb: start." << std::endl;
            //std::cout << "get_nb: cols/rows: " << cols << " " << rows << std::endl;
            //std::cout << "get_nb: x/y: " << x << " " << y << std::endl;

            neighbors[0] = grid[((x-1 % cols) + cols) % cols][((y-1 % rows) + rows) % rows]; // NW
            neighbors[1] = grid[((x % cols) + cols) % cols][((y-1 % rows) + rows) % rows]; // NW
            neighbors[2] = grid[((x+1 % cols) + cols) % cols][((y-1 % rows) + rows) % rows]; // NW
            neighbors[3] = grid[((x-1 % cols) + cols) % cols][((y % rows) + rows) % rows]; // NW
            neighbors[4] = grid[((x+1 % cols) + cols) % cols][((y % rows) + rows) % rows]; // NW
            neighbors[5] = grid[((x-1 % cols) + cols) % cols][((y+1 % rows) + rows) % rows]; // NW
            neighbors[6] = grid[((x % cols) + cols) % cols][((y+1 % rows) + rows) % rows]; // NW
            neighbors[7] = grid[((x+1 % cols) + cols) % cols][((y+1 % rows) + rows) % rows]; // NW

            // return the sum of alive neighbors
            return std::accumulate(std::begin(neighbors), std::end(neighbors), 0);
        }
        // apply the rules and return whether or not the cell needs to switch its status
        bool apply_rules(short nb_no, bool status) {
            if (status == true) {
                // dies, needs to switch
                if (nb_no < 2 || nb_no > 3) {
                    return true;
                }
                // stays alive, no need to switch
                return false;
            } else if (nb_no == 3) {
                // comes alive, needs to switch
                return true;
            }
            // is dead and stays dead
            return false;
        }
        // get alive/dead status
        bool get_status(long x, long y) {
            return grid[x][y];
        }
        // set alive/dead status
        bool set_status(long x, long y, bool status) {
            grid[x][y] = status;
            return 1;
        }

    public:
        CellMap(long cols, long rows) : cols(cols), rows(rows), grid(cols, std::vector<long>(rows)){ }

        // TODO fill with initial pattern: random, empty, etc.
        void initialize() {
            std::cout << "Initializing with random distribution." << std::endl;
            for (int i = 0; i < cols; ++i) {
                for (int j = 0; j < rows; ++j) {
                    grid[i][j] = rand() % 100 > 50 ? 0 : 1;
                }
            }
        }

        std::list<std::vector<long>> update() {
            std::cout << "Updating." << std::endl;
            // reset update_cells array
            update_cells.clear();
            // store invert signal temporarily
            bool invert;
            // start time measurement
            auto start = std::chrono::high_resolution_clock::now();
            // iterate over the whole grid
            for (int x = 0; x < cols; ++x) {
                for (int y = 0; y < rows; ++y) {
                    bool cell_status = get_status(x,y);
                    //std::cout << "get_status." << cell_status << std::endl;
                    short neighbor_no = get_neighbors(x,y);
                    //std::cout << "get_neighbors." << neighbor_no << std::endl;
                    //invert = get_status(x, y); //moved to if 
                    if (get_status(x, y)) {
                        //std::cout << "Adding to update list." << std::endl;
                        update_cells.push_back(std::vector<long>({x,y}));
                    }
                }
            }
            // stop time measurement and log
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
            std::cout << "new CellMap update duration: " << duration.count() << std::endl;

            std::cout << size(update_cells) << " cells need to be inverted." << std::endl;
            return update_cells;
        }
};

#endif