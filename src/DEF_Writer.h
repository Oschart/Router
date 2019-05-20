#ifndef DEFWR_H
#define DEFWR_H

#include <vector>
#include <string>
#include <fstream>
#include "DEF_Util.h"
using namespace std;
// Specification of the class

class DEF_Writer
{

public:

	// Public Member functions
    DEF_Writer(vector<metal>, vector<vector<seg> >);

    void write_DEF(string);     // Writes the routed DEF file
	
private:
    vector<metal> metalStack;       // Vector containing metal layers
    vector<int> nonDefWidth, nonDefOffset;    // nonDefault Rules
    vector<seg> wireSeg;        // Wire segments
	vector<vector<string> > routed;     // ROUTED statements
    void wireSize();        // Size the wires (decide the widths)
    
};

#include "DEF_Writer.cpp"

#endif // DEFWR_H

