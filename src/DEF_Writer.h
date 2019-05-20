#ifndef DEFWR_H
#define DEFWR_H

#include <vector>
#include <string>

using namespace std;

class DEF_Writer
{

public:

	// Public Member functions
    // Class Constructor
    DEF_Writer(vector<metal>, vector<vector<seg> >);

    void write_DEF(string);     // Writes the routed DEF file
	
private:
    vector<metal> metalStack;       // Vector containing metal layers
    vector<int> nonDefWidth, nonDefOffset;    // non-Default rules for width and offset
    vector<seg> wireSeg;        // Wire segments
	vector<vector<string> > routed;     // ROUTED statements
    void wireSize();        // Size the wires (decide the widths and offsets)
    
};

#include "DEF_Writer.cpp"

#endif // DEFWR_H

