#include <iostream>
#include <vector>
#include "Cell.h"
//#include "DEF_Util.h"
#include "Parser.cpp"

using namespace std;

vector<vector<vector<Cell>>> absNets;
vector<Cell> absObs;
//vector<RECT> obs;

void abstractNets()
{
    absNets.resize(nets.size());
    for (int i = 0; i < nets.size(); ++i)
    {
        absNets[i].resize(nets[i].size());
        for (int j = 0; j < nets[i].size(); ++j)
        {
            absNets[i].resize(nets[i][j].size());
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
                else
                {
                    hMetal = metalLayers[nextLayer];
                    vMetal = metalLayers[temp.metalLayer];
                }

                int hTrackIdx = (temp.y1 - hMetal.start + hMetal.step / 2) / hMetal.step;
                int hTrackPos = hMetal.start - hMetal.step / 2 + hTrackIdx * hMetal.step;
                int vTrackIdx = (temp.x1 - vMetal.start + vMetal.step / 2) / vMetal.step;
                int vTrackPos = vMetal.start - vMetal.step / 2 + vTrackIdx * vMetal.step;

                while (hTrackPos <= temp.y2)
                {
                    X.push_back(hTrackIdx);
                    hTrackIdx++;
                    hTrackPos += hMetal.step;
                }

                while (vTrackPos <= temp.x2)
                {
                    Y.push_back(vTrackIdx);
                    vTrackIdx++;
                    vTrackPos += vMetal.step;
                }

                Cell aux;
                for (int I = 0; I < X.size(); ++I)
                {
                    for (int J = 0; J < Y.size(); ++J)
                    {
                        aux.seti(X[I] + 1), aux.setj(Y[J] + 1), aux.setk(temp.metalLayer);
                        absNets[i][j].push_back(aux);
                    }
                }
            }
        }
    }
}

void abstractObs() // Abstract Obstructions
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
        else
        {
            hMetal = metalLayers[nextLayer];
            vMetal = metalLayers[temp.metalLayer];
        }

        int hTrackIdx = (temp.y1 - hMetal.start + hMetal.step / 2) / hMetal.step;
        int hTrackPos = hMetal.start - hMetal.step / 2 + hTrackIdx * hMetal.step;
        int vTrackIdx = (temp.x1 - vMetal.start + vMetal.step / 2) / vMetal.step;
        int vTrackPos = vMetal.start - vMetal.step / 2 + vTrackIdx * vMetal.step;

        while (hTrackPos <= temp.y2)
        {
            X.push_back(hTrackIdx);
            hTrackIdx++;
            hTrackPos += hMetal.step;
        }

        while (vTrackPos <= temp.x2)
        {
            Y.push_back(vTrackIdx);
            vTrackIdx++;
            vTrackPos += vMetal.step;
        }

        Cell aux;
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