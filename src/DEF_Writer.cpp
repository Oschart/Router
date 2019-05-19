//#include "DEF_Writer.h"
#include "DEF_Util.h"
#include "Parser.cpp"
#include <iostream>

using namespace std;

DEF_Writer::DEF_Writer(vector<metal> _metalStack, vector<vector<seg> > nets)
{
    metalStack = _metalStack;
    wireSize();
    int minX = DEF_FILE.dieArea[0].first;
    int minY = DEF_FILE.dieArea[0].second;
    routed.resize(nets.size());
    for (int i = 0; i < nets.size(); ++i)
    {
        routed[i].resize(nets[i].size());
        for (int j = 0; j < nets[i].size(); j++)
        {
            int mIdx = nets[i][j].metalLayer;
            int x1, x2, y1, y2;
            bool same;
            if(mIdx%2 == 0)     //Horizontal
            {
                //int centerOffset = nonDefWidth[mIdx]/2;
                x1 = nets[i][j].c1 + minX;
                x2 = nets[i][j].c2 + minX;
                y1 = y2 = nets[i][j].trackIdx + minY;
                same = 0;
            }
            else        // Vertical
            {
                x1 = x2 = nets[i][j].trackIdx + minX;
                y1 = nets[i][j].c1 + minY;
                y2 = nets[i][j].c2 + minY;
                same = 1;
            }
            routed[i][j] = "";
            routed[i][j] += metalStack[mIdx].label + " TAPERRULE LayerScaleRule ";
            routed[i][j] += " ( " + to_string(x1) + " " + to_string(y1) + " ) ";
            if(x1 == x2 && y1 == y2)
            {
                routed[i][j] += "M" + to_string(mIdx + 2) + "_M" + to_string(mIdx + 1);
                continue; 
            }
            routed[i][j] += + " ( " + (same? "* ": to_string(x1)) + (same? to_string(y1): " *") + " ) ";
        }
    }
}

void DEF_Writer::wireSize()
{

    for (int i = 0; i < metalStack.size(); i++)
    {
        if(i%2 == 0) nonDefWidth.push_back(metalStack[i].width);
        else nonDefWidth.push_back(metalStack[i].width * (i + 1) / 2);   
    }
}

void DEF_Writer::write_DEF(string inFile)
{
    string token;
    cout << "Insert routed DEF file name (no extension): ";
    getline(cin, token);
    ofstream out(token + ".def");
    ifstream in(inFile);
    int k = 0;
    while (getline(in, token))
    {
        out << token << endl;
        // Remove leading spaces
        while (token[0] == ' ')
            token = token.substr(1);
        if (token.substr(0, 4) == "NETS")
        {
            k = 0;
            while (getline(in, token) && token.find("END NETS") == string::npos)
            {
                
                if (token.find(";") != string::npos)
                {
                    out << token.substr(0, token.find(";")) << endl;
                    out << "+ ROUTED " << routed[k][0] << endl;
                    for(int i = 1; i < routed[k].size(); ++i)
                    {
                        out << " NEW " + routed[k][i];
                        if(i == routed[k].size() - 1) out << " ;";
                        out << endl;
                    }
                    //
                    if(k < routed.size()-1)
                    ++k;
                }
                else
                    out << token << endl;
            
            }
        }
    }
    out << "NONDEFAULTRULES 1 ;" << endl;
    out << "- LayerScaleRule " << endl;
    for(int i = 0; i < metalStack.size(); ++i)
    {
        out << "+ LAYER " + metalStack[i].label + " WIDTH " + to_string(nonDefWidth[i]);
        if(i == metalStack.size() - 1) out << " ;";
        out << endl;
    }
    out << "END NONDEFAULTRULES " << endl;
}