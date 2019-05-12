#ifndef DCELL_H
#define DCELL_H

#include <vector>
using namespace std;

class DCell {
private:
    char S; // definition/traceback aid
    char C; // reach flag
    int cost; //GCell cost
    int indi, indj, indk; //coordinates

public:
    DCell();
    DCell (char, char);
    bool dir[4];
    vector<vector<int>> detailed;
    vector<int> nets; //indexes of the nets assigned to this
    void setS(char); //setters and getters
    void setC(char);
    char getS();
    char getC();
    int geti();
    int getj();
    int getk();
    void seti(int);
    void setj(int);
    void setk(int);
    int getCost();
    void incCost(int); //increments the cost

};

#include "DCell.cpp"

#endif // DCELL_H



