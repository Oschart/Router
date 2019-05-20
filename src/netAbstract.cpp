#include <iostream>
#include <vector>
#include "Cell.h"
#include "Parser.cpp"

using namespace std;

vector<vector<vector<Cell>>> absNets;
vector<Cell> absObs;

// Method that abstracts physical pins into abstract (grid) pins
void abstractNets()
{
    absNets.resize(nets.size());
    for (int i = 0; i < nets.size(); ++i)
    {
        absNets[i].resize(nets[i].size());
        for (int j = 0; j < nets[i].size(); ++j)
        {
            for (int k = 0; k < nets[i][j].size(); ++k)
            {
                vector<int> X, Y;
                RECT temp = nets[i][j][k];
                metal hMetal, vMetal; // Horizontal and Vertical metal layers
                int nextLayer = (temp.metalLayer < metalLayers.size() - 1 ? 1 : -1) + temp.metalLayer;
                if (temp.metalLayer % 2 == 0) // Metal layer is horizontal
                {
                    hMetal = metalLayers[temp.metalLayer];
                    vMetal = metalLayers[nextLayer];
                }
                else // Metal layer is vertical
                {
                    hMetal = metalLayers[nextLayer];
                    vMetal = metalLayers[temp.metalLayer];
                }

                // Track index of first horizontal metal track whose field crosses the rectangle
                int hTrackIdx = (temp.y1 - hMetal.start + hMetal.step / 2) / hMetal.step;
                // Track position of first horizontal metal track whose field crosses the rectangle
                int hTrackPos = hMetal.start - hMetal.step / 2 + hTrackIdx * hMetal.step;
                // Track index of first vertical metal track whose field crosses the rectangle
                int vTrackIdx = (temp.x1 - vMetal.start + vMetal.step / 2) / vMetal.step;
                // Track position of first vertical metal track whose field crosses the rectangle
                int vTrackPos = vMetal.start - vMetal.step / 2 + vTrackIdx * vMetal.step;

                // Until you reach the end of the rectangle vertically
                while (hTrackPos <= temp.y2)
                {
                    X.push_back(hTrackIdx);
                    hTrackIdx++;
                    hTrackPos += hMetal.step;
                }

                // Until you reach the end of the rectangle horizontally
                while (vTrackPos <= temp.x2)
                {
                    Y.push_back(vTrackIdx);
                    vTrackIdx++;
                    vTrackPos += vMetal.step;
                }

                Cell aux; // Auxiliary cell that represents an abstract cell
                // Pair the x and y coordinates together to form grid points
                for (int I = 0; I < X.size(); ++I)
                {
                    for (int J = 0; J < Y.size(); ++J)
                    {
                        aux.seti(X[I] + 1), aux.setj(Y[J] + 1), aux.setk(temp.metalLayer);
                        absNets[i][j].push_back(aux);
                    }
                }
                // Check if no cells were realized
                if (absNets[i][j].size() == 0)
                    cout << "Error at " << i << ", " << j << endl;
            }
        }
    }
}

// Method that abstracts physical blockages into abstract (grid) obstacles
void abstractObs()
{
    for (int k = 0; k < obs.size(); ++k)
    {
        vector<int> X, Y;
        RECT temp = obs[k];
        metal hMetal, vMetal; // Horizontal and Vertical metal layers
        int nextLayer = (temp.metalLayer < metalLayers.size() - 1 ? 1 : -1) + temp.metalLayer;
        if (temp.metalLayer % 2 == 0) // Metal layer is horizontal
        {
            hMetal = metalLayers[temp.metalLayer];
            vMetal = metalLayers[nextLayer];
        }
        else // Metal layer is horizontal
        {
            hMetal = metalLayers[nextLayer];
            vMetal = metalLayers[temp.metalLayer];
        }
        // Track index of first horizontal metal track whose field crosses the rectangle
        int hTrackIdx = (temp.y1 - hMetal.start + hMetal.step / 2) / hMetal.step;
        // Track position of first horizontal metal track whose field crosses the rectangle
        int hTrackPos = hMetal.start - hMetal.step / 2 + hTrackIdx * hMetal.step;
        // Track index of first vertical metal track whose field crosses the rectangle
        int vTrackIdx = (temp.x1 - vMetal.start + vMetal.step / 2) / vMetal.step;
        // Track position of first vertical metal track whose field crosses the rectangle
        int vTrackPos = vMetal.start - vMetal.step / 2 + vTrackIdx * vMetal.step;

        // Until you reach the end of the rectangle vertically
        while (hTrackPos <= temp.y2)
        {
            X.push_back(hTrackIdx);
            hTrackIdx++;
            hTrackPos += hMetal.step;
        }

        // Until you reach the end of the rectangle horizontally
        while (vTrackPos <= temp.x2)
        {
            Y.push_back(vTrackIdx);
            vTrackIdx++;
            vTrackPos += vMetal.step;
        }

        Cell aux; // Auxiliary cell that represents an abstract cell
        // Pair the x and y coordinates together to form grid points
        for (int I = 0; I < X.size(); ++I)
        {
            for (int J = 0; J < Y.size(); ++J)
            {
                aux.seti(X[I] + 1), aux.setj(Y[J] + 1), aux.setk(temp.metalLayer);
                absObs.push_back(aux);
            }
        }
    }
}