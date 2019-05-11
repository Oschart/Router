#ifndef DEFUTIL_H
#define DEFUTIL_H

#include <utility>
using namespace std;


struct seg
{
    int metalLayer, trackIdx;
    double c1, c2;
};

struct via{
    bool Default;
    int botLayerIndex, cutLayerIndex, topLayerIndex;
    vector <pair<double, double> > botLayerShape, cutLayerShape, topLayerShape;
};

struct cut{
    string label;
    int index;
    double spacing;
};

struct metal{
    string label;
    int index;
    char dir;
    double pitch, width, spacing, offset;

};

struct pin{

};

struct macro{
    string name;
    pair<double, double> size;

};

struct LEF{
    string ver;
    char div, bus_left, bus_right;
    int database;
};

struct DEF{
	string ver, design, tech;
	char div, bus_left, bus_right;
	int unit;
	vector<pair<int,int> > dieArea;
};


// Add whatever utility structs you're going to extract (most importantly, Vias and Metal Layers)

#endif // DEFUTIL_H

