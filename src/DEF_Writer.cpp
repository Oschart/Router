#include <iostream>

using namespace std;

DEF_Writer::DEF_Writer(vector<metal> _metalStack, vector<vector<seg>> nets)
{
    metalStack = _metalStack;
    wireSize();
    // Bottom left die corner
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
            int nextLayer = (mIdx < metalLayers.size() - 1 ? 1 : -1) + mIdx;
            if (mIdx % 2 == 0) // Horizontal
            {
                x1 = nets[i][j].c1 + minX + nonDefOffset[nextLayer];
                x2 = nets[i][j].c2 + minX + nonDefOffset[nextLayer];
                y1 = y2 = nets[i][j].trackIdx + minY + nonDefOffset[mIdx];
                same = 0;
            }
            else // Vertical
            {
                x1 = x2 = nets[i][j].trackIdx + minX + nonDefOffset[mIdx];
                y1 = nets[i][j].c1 + minY + nonDefOffset[nextLayer];
                y2 = nets[i][j].c2 + minY + nonDefOffset[nextLayer];
                same = 1;
            }
            routed[i][j] = ""; // Make sure it's an empty string
            // Use the non-default rule
            routed[i][j] += metalStack[mIdx].label + " TAPERRULE LayerScaleRule ";
            routed[i][j] += " ( " + to_string(x1) + " " + to_string(y1) + " ) ";
            // if both end are the same, then it's a via
            if (x1 == x2 && y1 == y2)
            {
                routed[i][j] += "M" + to_string(mIdx + 2) + "_M" + to_string(mIdx + 1);
                continue;
            }
            routed[i][j] += +" ( " + (same ? "* " : to_string(x2)) + (same ? to_string(y2) : " *") + " ) ";
        }
    }
}

// Determine final wire widths and offsets
void DEF_Writer::wireSize()
{
    // First horizontal layer set as constant reference (same as LEF)
    nonDefWidth.push_back(metalStack[0].width);
    nonDefOffset.push_back(metalStack[0].offset);
    // First vertical layer set as constant reference (same as LEF)
    nonDefWidth.push_back(metalStack[1].width);
    nonDefOffset.push_back(metalStack[1].offset);

    // Adjust width and offset for remaining layers
    for (int i = 2; i < metalStack.size(); i++)
    {
        if (i % 2 == 0) // Horizontal
        {
            nonDefWidth.push_back(metalStack[i].width);
            nonDefOffset.push_back(metalStack[i].offset);
        }
        else // Vertical
        {
            nonDefWidth.push_back(metalStack[i - 2].pitch + metalStack[i - 2].width);
            nonDefOffset.push_back(metalStack[i - 2].offset + metalStack[i - 2].pitch / 2);
        }
    }
}

// Write the output (routed) DEF file
void DEF_Writer::write_DEF(string inFile)
{
    string token;
    cout << "Insert output (routed) DEF file name: ";
    getline(cin, token);
    ofstream out(token);
    ifstream in(inFile);
    int k = 0;
    while (getline(in, token))
    {
        out << token << endl;
        if (token.find("END COMPONENTS") != string::npos)
        {
            // Write a non-default rule section to override width and offset in the LEF
            out << "\nNONDEFAULTRULES 1 ;" << endl;
            out << "- LayerScaleRule " << endl;
            for (int i = 0; i < metalStack.size(); ++i)
            {
                out << "+ LAYER " + metalStack[i].label + " WIDTH " + to_string(nonDefWidth[i]);
                if (i == metalStack.size() - 1)
                    out << " ;";
                out << endl;
            }
            out << "END NONDEFAULTRULES \n" << endl;
        }
        
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
                    if (routed[k].size() == 1)
                        out << " ; \n";
                    for (int i = 1; i < routed[k].size(); ++i)
                    {
                        out << " NEW " + routed[k][i];
                        if (i == routed[k].size() - 1)
                            out << " ;";
                        out << endl;
                    }
                    //
                    if (k < routed.size() - 1)
                        ++k;
                }
                else
                    out << token << endl;
            }
            out << "END NETS \n";
        }
        
    }
}
