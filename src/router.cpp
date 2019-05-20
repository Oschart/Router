#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include "netAbstract.cpp"
#include "soukup_router.cpp"
#include "soukup_detailed_router.cpp"
#include "DEF_Writer.h"
#define INT_MAX 1000000000
using namespace std;

//logical pin struct
struct lpin{
    vector<Cell> cells;
};

//represents a net of two or more pins
struct net {
    vector<lpin> pins;
    int boxsize = 1;
};

//comparator function to sort nets in ascending order according to their bounding box size
bool comp (net& a, net& b){ return a.boxsize < b.boxsize;}

//generates 2D bounding box size for all nets in the netlist
void generateBoxSizes(vector<net>& netlist){
    for (int i = 0 ; i < netlist.size(); i++){
        int mini = INT_MAX, maxi = -1, minj = INT_MAX, maxj = -1, mink = INT_MAX, maxk = -1;
        for (int j = 0 ; j < netlist[i].pins.size(); j++){
            mini = min(mini, netlist[i].pins[j].cells[0].geti());
            minj = min(minj, netlist[i].pins[j].cells[0].getj());
            mink = min(mink, netlist[i].pins[j].cells[0].getk());
            maxi = max(maxi, netlist[i].pins[j].cells[0].geti());
            maxj = max(maxj, netlist[i].pins[j].cells[0].getj());
            maxk = max(maxk, netlist[i].pins[j].cells[0].getk());
        }
        netlist[i].boxsize = (maxi-mini+1)*(maxj-minj+1)/**(maxk-mink+1)*/;
    }
}

//returns associated global cell
Cell getGlobal(Cell detailed){
    Cell ans;
    ans.setk(detailed.getk());
    int dheight = dgrid.heights[ans.getk()];
    int dwidth = dgrid.widths[ans.getk()];
    ans.seti((detailed.geti()-1) / dheight + 1);
    ans.setj((detailed.getj()-1) / dwidth + 1);
    return ans;

}
//reserves capacity for all the pins in the netlist
vector<net> reservePins(vector<net>& netlist){
    vector<net> dnetlist(netlist.size());
    for (int i = 0 ; i < netlist.size(); i++){
        net dnet; dnet.pins.resize(netlist[i].pins.size());
        for (int j = 0 ; j < netlist[i].pins.size(); j++){
            Cell pin = netlist[i].pins[j].cells[0];
            dnet.pins[j] = netlist[i].pins[j];
            int gk = pin.getk();
            int dheight = dgrid.heights[gk];
            int dwidth = dgrid.widths[gk];
            int gi = (pin.geti()-1) / dheight + 1;
            int gj = (pin.getj()-1) / dwidth + 1;
            for (int k = 0; k < netlist[i].pins[j].cells.size(); k++){
                Cell pin1 = netlist[i].pins[j].cells[k];
                dgrid.grid[gk][pin1.geti()][pin1.getj()].setS(7);


            }
            Cell gcell;
            gcell.seti(gi);
            gcell.setj(gj);
            gcell.setk(gk);
            netlist[i].pins[j].cells.clear();
            netlist[i].pins[j].cells.push_back(gcell);
            grid.grid[gk][gi][gj].incCost(i);
        }
        dnetlist[i] = dnet;
    }
    return dnetlist;
}

//parses vector<vector<vector<Cell>>> into vector<net>
vector<net>  parseNetlist(vector<vector<vector<Cell>>>& input){
    vector<net> netlist;
    for (int i = 0 ; i < input.size(); i++){
        net net1;
        for (int j = 0 ; j < input[i].size(); j++){
            lpin pin1;
            pin1.cells = input[i][j];
            net1.pins.push_back(pin1);
        }
        netlist.push_back(net1);
    }
    return netlist;
}

//returns the ind of the closest pin to the source
int getInd(net mynet){
    int dist = INT_MAX, ans;
    Cell s = mynet.pins[0].cells[0];
    for (int i = 1; i < mynet.pins.size(); i++){
        Cell t = mynet.pins[i].cells[0];
        int d = getNormalizedDistance(s.geti(), s.getj(), s.getk(), t.geti(), t.getj(), t.getk());
        if (d < dist){
            dist = d;
            ans = i;
        }
    }
    return ans;
}


vector<vector<seg>> getSegments() {

    //user input
    cout << "Please enter the width and the height of the global routing grid" << endl;
    int Width, Height;
    cin >> Width >> Height;
    cout << "Please enter the limit on the GBox" << endl;
    cin >> limit;
    cout << "Please enter whether or not you want to include the obstructions" << endl;
    int obsb; cin >> obsb;

    //reading files
    vector<net> netlist;
    cin.ignore();
    readLEF();
    readDEF();
    cout << "The size of obs = " << obs.size() << endl;

    //translating rects to cells
    abstractObs();
    abstractNets();
    netlist = parseNetlist(absNets);

    //recording metal layers data
    for (int i = 0 ; i < metalLayers.size(); i++){
        tracks.push_back(metalLayers[i].numTracks);
    }
    mdirect = (metalLayers[0].dir == 'H'? 0 : 1);
    n_layers = tracks.size();
    tracks_per_gcell.resize(n_layers);
    tracks_per_gcell[n_layers-1] = (int) ceil((double)tracks[n_layers-1] /((n_layers + mdirect)%2 ? Height : Width) - 0.005);
    int k = n_layers - 3;
    while (k >= 0) tracks_per_gcell[k] = tracks_per_gcell[n_layers-1]*tracks[k] / tracks[n_layers-1], k-=2;
    tracks_per_gcell[n_layers-2] = (int) ceil((double)tracks[n_layers-2] /((n_layers + mdirect)%2 ? Width : Height) - 0.005);
    k = n_layers - 4;
    while (k >= 0) tracks_per_gcell[k] = tracks_per_gcell[n_layers-2]*tracks[k] / tracks[n_layers-2], k-=2;
    for (int i = 0 ; i < n_layers ; i++) metal_directions.push_back((i+mdirect)%2);

    //processing the netlist
    generateBoxSizes(netlist);
    sort(netlist.begin(), netlist.end(), comp);

    //creating the grids
    grid = GGrid(Height, Width, n_layers);
    dgrid = DGrid(Height, Width, n_layers, tracks, tracks_per_gcell, mdirect);

    //blocking out area outside the core area
    for (int k = 0 ; k < n_layers; k++){
        int lheight  = tracks[k];
        int lwidth = tracks[k + ((k == n_layers-1)? -1 : 1)];
        int dheight = dgrid.heights[k];
        int dwidth = dgrid.widths[k];
        if ((k+mdirect)%2) swap(lheight, lwidth);
        //global grid
        for (int i = 1 ; i <= Height ; i++){
            for (int j = 1 ; j <= Width; j++){
                if ((i-1)*dheight > lheight || (j-1)*dwidth > lwidth)grid.grid[k][i][j].setS(7);
            }
        }
        //detailed grid
        for (int i = 1 ; i < dgrid.grid[k].size()-1 ; i++){
            for (int j = 1; j < dgrid.grid[k][i].size()-1 ; j++){
                if (i > lheight || j > lwidth){ dgrid.grid[k][i][j].setS(7);
                Cell global = getGlobal(dgrid.grid[k][i][j]);
                grid.grid[global.getk()][global.geti()][global.getj()].incCost(-1);


                }
            }
        }
    }

    //applying obstructions
    if (obsb){
        vector<Cell> inf;
        for (int i = 0 ; i < absObs.size(); i++){
            Cell cell = absObs[i];
            dgrid.grid[cell.getk()][cell.geti()][cell.getj()].setS(7);
            inf.push_back(getGlobal(cell));
        }
        vector<Cell> ex;
        for (int i = 0 ; i < inf.size(); i++){
            Cell cell = inf[i];
            if (grid.grid[cell.getk()][cell.geti()][cell.getj()].getS() == 7 || grid.grid[cell.getk()][cell.geti()][cell.getj()].getS() == 8) continue;
            int dheight = dgrid.heights[cell.getk()];
            int dwidth = dgrid.widths[cell.getk()];
            int found = 0;
            for (int j = 0 ; !found && j < dheight; j++){
                for (int k = 0 ; k < dwidth ; k++){
                    if (dgrid.grid[cell.getk()][(cell.geti()-1)*dheight + j+1][(cell.getj()-1)*dwidth + k+1].getS() != 7){
                        grid.grid[cell.getk()][cell.geti()][cell.getj()].setS(8);
                        ex.push_back(cell);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found){
                grid.grid[cell.getk()][cell.geti()][cell.getj()].setS(7);
            }
        }
        for (auto cell : ex){
            grid.grid[cell.getk()][cell.geti()][cell.getj()].setS(0);
            int dheight = dgrid.heights[cell.getk()];
            int dwidth = dgrid.widths[cell.getk()];
            for (int j = 0 ; j < dheight; j++){
                for (int k = 0 ; k < dwidth ; k++){
                    if (dgrid.grid[cell.getk()][(cell.geti()-1)*dheight + j+1][(cell.getj()-1)*dwidth + k+1].getS() == 7){
                        grid.grid[cell.getk()][cell.geti()][cell.getj()].incCost(-1);
                    }
                }
            }
        }
    }

    //reserving pins and generating the global netlist
    vector<net> dnetlist = reservePins(netlist);

    int fail = 0;
    //routing net by net
    for (int i = 0 ; i < netlist.size(); i++){
        netind = i;
        Cell source = netlist[i].pins[0].cells[0];
        int ind = getInd(netlist[i]);
        Cell closest = netlist[i].pins[ind].cells[0];
        vector<Cell> initial_path;
        if (source.geti() == closest.geti() && source.getj() == closest.getj() && source.getk() == closest.getk()){
            grid.grid[closest.getk()][closest.geti()][closest.getj()].allowed = 1;
            if (closest.getk() != grid.getLayers()-1) grid.grid[closest.getk()+1][closest.geti()][closest.getj()].allowed = 1;
            if (closest.getk() != 0) grid.grid[closest.getk()-1][closest.geti()][closest.getj()].allowed = 1;
            initial_path.push_back(closest);
        }
        else {
            targetCells.push_back(closest);
            first = 1;
            //first round of global
            initial_path = soukup_route(source);
            if (initial_path.size() == 0) {cout << "Global routing failed" << endl; break;}
            for (Cell cell : initial_path){
                grid.grid[cell.getk()][cell.geti()][cell.getj()].allowed = 1;
                if (cell.getk() != grid.getLayers()-1) grid.grid[cell.getk()+1][cell.geti()][cell.getj()].allowed = 1;
                if (cell.getk() != 0) grid.grid[cell.getk()-1][cell.geti()][cell.getj()].allowed = 1;
            }
        }

        Cell dsource = dnetlist[i].pins[0].cells[0];
        for (int j = 1; j < dnetlist[i].pins[0].cells.size(); j++){
            dnetlist[i].pins[0].cells[j].invobs = 1;
            dnetlist[i].pins[0].cells[j].setS(7);
            dupdate_grid( dnetlist[i].pins[0].cells[j]);
        }
        //first round of detailed
        Cell dclosest = dnetlist[i].pins[ind].cells[0];
        dclosest.target = 1;
        dupdate_grid(dclosest);
        dtargetCells.push_back(dclosest);
        for (int j = 1 ; j < dnetlist[i].pins[ind].cells.size(); j++) {dnetlist[i].pins[ind].cells[j].target = 1; dnetlist[i].pins[ind].cells[j].setS(7); dupdate_grid(dnetlist[i].pins[ind].cells[j]);}
        vector<Cell> dinitial_path = dsoukup_route(dsource);
        if (dinitial_path.size() == 0) {cout << "Detailed routing failed" << endl; break;}
        for (int j = 1 ; j < dnetlist[i].pins[ind].cells.size(); j++) {dinitial_path.push_back(dnetlist[i].pins[ind].cells[j]);}
        for (Cell cell : dinitial_path){cell.target = 1; cell.setS(7);dupdate_grid(cell);}
        for (int j = 1; j < dnetlist[i].pins[0].cells.size(); j++){
            dnetlist[i].pins[0].cells[j].invobs = 0;
            dnetlist[i].pins[0].cells[j].setS(7);
            dupdate_grid( dnetlist[i].pins[0].cells[j]);
        }
        first = 0;
        targetCells.clear();
        dtargetCells.clear();

        //routing the remaining pins
        for (int j = 1; j < netlist[i].pins.size(); j++){
            if (j == ind) continue;
            for (auto cell : dinitial_path){cell.target = 1; cell.setS(7); dupdate_grid(cell); dtargetCells.push_back(cell); }
            Cell dsource = dnetlist[i].pins[j].cells[0];
            targetCells.clear();
            targetCells.push_back(getGlobal(dtargetCells[dgetClosest(dsource, dtargetCells, -1)]));
            initial_path = soukup_route(netlist[i].pins[j].cells[0]);
            if (initial_path.size() == 0){cout << "Global routing failed" << endl; fail = 1; break;}
            for (Cell cell : initial_path){
                grid.grid[cell.getk()][cell.geti()][cell.getj()].allowed = 1;
                if (cell.getk() != grid.getLayers()-1) grid.grid[cell.getk()+1][cell.geti()][cell.getj()].allowed = 1;
                if (cell.getk() != 0) grid.grid[cell.getk()-1][cell.geti()][cell.getj()].allowed = 1;
            }
            for (int k = 1; k < dnetlist[i].pins[j].cells.size(); k++){
                dnetlist[i].pins[j].cells[k].invobs = 1;
                dnetlist[i].pins[j].cells[k].setS(7);
                dupdate_grid( dnetlist[i].pins[j].cells[k]);
            }
            dinitial_path = dsoukup_route(dnetlist[i].pins[j].cells[0]);
            if (dinitial_path.size() == 0){cout << "Detailed routing failed" << endl; fail = 1; break;}
            for (int k = 1; k < dnetlist[i].pins[j].cells.size(); k++){
                dnetlist[i].pins[j].cells[k].invobs = 0;
                dnetlist[i].pins[j].cells[k].setS(7);
                dupdate_grid( dnetlist[i].pins[j].cells[k]);
            }
        }
        if (fail) break;
        for (auto cell : dinitial_path){cell.setS(7); dupdate_grid(cell);}
        segments.push_back(current);
        current.clear();
        grid.cleartens();
        targetCells.clear();
        dtargetCells.clear();

        for (int k = 0 ; k < n_layers; k++){
            for (int l = 0; l < grid.grid[k].size(); l++) for (int m = 0 ; m < grid.grid[k][l].size(); m++) grid.grid[k][l][m].target = 0;
            for (int l = 0; l < dgrid.grid[k].size(); l++) for (int m = 0 ; m < dgrid.grid[k][l].size(); m++) dgrid.grid[k][l][m].target = 0;
        }
        cout << "net " << i << " success" << endl;
    }

    //processing wire segments
    for (int i = 0 ; i < segments.size(); i++){
        for (int j = 0; j < segments[i].size(); j++){
            cout << segments[i][j].metalLayer << " " << segments[i][j].trackIdx << " " << segments[i][j].c1 << " " << segments[i][j].c2 << endl;
            segments[i][j].trackIdx *= metalLayers[segments[i][j].metalLayer].pitch;
            segments[i][j].c1 *= metalLayers[segments[i][j].metalLayer + (segments[i][j].metalLayer == n_layers-1? -1 : 1)].pitch;
            segments[i][j].c2 *= metalLayers[segments[i][j].metalLayer + (segments[i][j].metalLayer == n_layers-1? -1 : 1)].pitch;
        }
    }
    return segments;
}

int main (){
    auto segs = getSegments();
    DEF_Writer writer(metalLayers, segs);
    writer.write_DEF(inDEF);

}


