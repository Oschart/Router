#ifndef DEFUTIL_H
#define DEFUTIL_H

#include <utility>
#include <map>
using namespace std;


struct RECT{
    int metalLayer;
    int x1, y1, x2, y2;
};

struct seg{
    int metalLayer, trackIdx;
    double c1, c2;
};

struct via{
    bool Default;
    string label;
    int botLayerIndex, cutLayerIndex, topLayerIndex;
    vector <pair<double, double> > botLayerShape, cutLayerShape, topLayerShape;
};

struct cut{
    string label;
    int index;
    double spacing;
};

struct component{
    string instance, macro, orientation;
    int x_pos, y_pos;
};

struct metal{
    string label;
    int index;
    char dir;
    int pitch, width, spacing, offset;
    //
    int start, step;
    int numTracks;

};

struct pin{
    string label;
    vector<RECT> ports;
};

struct macro{
    string label, Class;
    int width, height;
    map <string, pin> pins;
    vector<RECT> obs;
};

struct LEF{
    string ver;
    char div, bus_left, bus_right;
    int unit;
};

struct DEF{
	string ver, design, tech;
	char div, bus_left, bus_right;
	int unit;
	vector<pair<int,int> > dieArea;
};


// Add whatever utility structs you're going to extract (most importantly, Vias and Metal Layers)

#endif // DEFUTIL_H

