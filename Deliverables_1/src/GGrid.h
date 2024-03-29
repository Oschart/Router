#ifndef GGRID_H
#define GGRID_H

#include "Cell.h"
#include <vector>


using namespace std;

class GGrid {

private:
    int width; //dimensions of the grid
    int height;
    int layers;


public:
    GGrid(); //default constructor
    GGrid (int, int, int); //constructor
    vector<vector<vector<Cell>>>grid; //the actual grid
    int getHeight(); //getters
    int getWidth();
    int getLayers();
    void print(); //prints out the grid
    void clean(); // cleans the grid
    void addLayer(); //adds an extra layer

};

#include "GGrid.cpp"
#endif // GGRID_H



