#ifndef DGRID_H
#define DGRID_H

#include "Cell.h"
#include <vector>


using namespace std;

class DGrid {

private:
    int width; //dimensions of the grid
    int height;
    int layer; //the metal layer containing the cell hosting this grid


public:
    DGrid(); //default constructor
    DGrid (int, int, int, vector<int>, vector<int>); //constructor
    vector<vector<Cell>>grid; //the actual grid
    int getHeight(); //getters
    int getWidth();
    void print(); //prints out the grid
    void clean(); // cleans the grid
    //void addLayer(); //adds an extra layer

};

#include "DGrid.cpp"
#endif // DGRID_H



