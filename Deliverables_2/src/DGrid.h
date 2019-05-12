#ifndef DGRID_H
#define DGRID_H

#include "Cell.h"
#include <vector>


using namespace std;

class DGrid {

private:
    int W; //dimensions of the grid
    int H;
    int layers; //the metal layer containing the cell hosting this grid
    int mdirect;

public:
    DGrid(); //default constructor
    DGrid (int, int, int, vector<int>, vector<int>, int); //constructor
    vector<vector<vector<Cell>>>grid; //the actual grid
    vector<int>heights;
    vector<int>widths;
    int getHeight(); //getters
    int getWidth();
    void print(); //prints out the grid
    void clean(); // cleans the grid

};

#include "DGrid.cpp"
#endif // DGRID_H



