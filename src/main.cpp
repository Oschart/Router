#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include "Parser.cpp"
#include "soukup_router.cpp"
#include "soukup_detailed_router.cpp"
#define INT_MAX 1000000000
using namespace std;

//represents a net of two or more pins
struct net {
    vector<Cell> pins;
    int boxsize = 1;
};

//comparator function to sort nets in descending order according to their bounding box size
bool comp (net& a, net& b){ return a.boxsize > b.boxsize;}

//gets the minimum number of layers based on the input netlist
int getMinLayers (vector<net>& netlist) {
    int ans = -1;
    for (int i = 0 ; i < netlist.size(); i++)
        for (int j = 0 ; j < netlist[i].pins.size(); j++){
            ans = max(ans, (netlist[i].pins[j]).getk());
        }
    return ans + 1;
}

//generates 2D bounding box size for all nets in the netlist
void generateBoxSizes(vector<net>& netlist){
    for (int i = 0 ; i < netlist.size(); i++){
        int mini = INT_MAX, maxi = -1, minj = INT_MAX, maxj = -1, mink = INT_MAX, maxk = -1;
        for (int j = 0 ; j < netlist[i].pins.size(); j++){
            mini = min(mini, netlist[i].pins[j].geti());
            minj = min(minj, netlist[i].pins[j].getj());
            mink = min(mink, netlist[i].pins[j].getk());
            maxi = max(maxi, netlist[i].pins[j].geti());
            maxj = max(maxj, netlist[i].pins[j].getj());
            maxk = max(maxk, netlist[i].pins[j].getk());
        }
        netlist[i].boxsize = (maxi-mini+1)*(maxj-minj+1)/**(maxk-mink+1)*/;
    }
}

//reserves capacity for all the pins in the netlist
vector<net> reservePins(vector<net>& netlist){
    vector<net> dnetlist(netlist.size());
    for (int i = 0 ; i < netlist.size(); i++){
        net dnet; dnet.pins.resize(netlist[i].pins.size());
        for (int j = 0 ; j < netlist[i].pins.size(); j++){
            Cell pin = netlist[i].pins[j];
            dnet.pins[j] = netlist[i].pins[j];
            int gk = pin.getk();
            int dheight = dgrid.heights[gk];
            int dwidth = dgrid.widths[gk];
            int gi = pin.geti() / dheight + 1;
            int gj = pin.getj() / dwidth + 1;
            dgrid.grid[gk][pin.geti()][pin.getj()].setS(7);
            netlist[i].pins[j].seti(gi);
            netlist[i].pins[j].setj(gj);
            grid.grid[gk][gi][gj].incCost(i);
        }
        dnetlist[i] = dnet;
    }
    return dnetlist;
}

//generates arbitrary netlists for testing purposes
vector<net> getFakeNetlist(){
    Cell cell1;
    cell1.seti(1);
    cell1.setj(1);
    cell1.setk(0);
    Cell cell2;
    cell2.seti(4);
    cell2.setj(4);
    cell2.setk(0);
    Cell cell3;
    cell3.seti(3);
    cell3.setj(4);
    cell3.setk(0);
    Cell cell4;
    cell4.seti(3);
    cell4.setj(5);
    cell4.setk(0);
    Cell cell5;
    cell5.seti(3);
    cell5.setj(9);
    cell5.setk(0);
    Cell cell6;
    cell6.seti(3);
    cell6.setj(7);
    cell6.setk(0);
    net net1, net2, net3;
    net1.pins.push_back(cell1);
    net1.pins.push_back(cell2);
    net2.pins.push_back(cell4);
    net2.pins.push_back(cell6);
    net3.pins.push_back(cell5);
    net3.pins.push_back(cell3);

    vector<net> netlist;
    netlist.push_back(net1);
    netlist.push_back(net2);
    netlist.push_back(net2);
    netlist.push_back(net3);
    return netlist;
}


int main() {
    freopen("in.txt", "r", stdin);
    //cout << "Please enter the width and the height of the global routing grid" << endl;
    int Width, Height;
    cin >> Width >> Height;
    //cout << "Please enter the number of nets in your netlist" << endl;
    int n; cin >> n;
    vector<net> netlist;
    for (int i = 0 ; i < n ; i++){
        net mynet;
        //cout << "Please enter the number of pins for the " << i+1 << (i == 0? "st" : i == 1? "nd" : i == 2? "rd" : "th") << " net" << endl;
        int m; cin >> m;
        for (int j = 0 ; j < m; j++){
            //cout << "please enter the three dimensions for pin number " << j+1 << endl;
            int pi,pj,pk; cin >> pi >> pj >> pk; --pk;
            Cell cell; cell.seti(pi); cell.setj(pj);cell.setk(pk);
            mynet.pins.push_back(cell);
        }
        netlist.push_back(mynet);
    }
//    cout << "Please enter the direction for Metal1 (0 for horizontal, 1 for vertical)" << endl;
//    cout << "Higher metal layers alternate between horizontal and vertical" << endl;
   // cin >> mdirect;
    cin.ignore();
    readLEF();
    readDEF();
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
    generateBoxSizes(netlist);
    sort(netlist.begin(), netlist.end(), comp);
    grid = GGrid(Height, Width, n_layers);
    dgrid = DGrid(Height, Width, n_layers, tracks, tracks_per_gcell, mdirect);
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
                if (i > lheight || j > lwidth) dgrid.grid[k][i][j].setS(7);
            }
        }
    }
    vector<net> dnetlist = reservePins(netlist);
    for (int i = 0 ; i < netlist.size(); i++){
        netind = i;
        Cell source = netlist[i].pins[0];
        int ind = getClosest(source, netlist[i].pins, 0);
        Cell closest = netlist[i].pins[ind];
        vector<Cell> initial_path;
        if (source.geti() == closest.geti() && source.getj() == closest.getj() && source.getk() == closest.getk()){
            closest.setS(10);
            update_grid(closest);
            initial_path.push_back(closest);
        }
        else {
            closest.setS(6);
            update_grid(closest);
            targetCells.push_back(closest);
            first = 1;
            initial_path = soukup_route(source);
            if (initial_path.size() == 0) {cout << "Global routing failed" << endl; return -1;}
        }
        Cell dsource = dnetlist[i].pins[0];
        Cell dclosest = dnetlist[i].pins[ind];
        dclosest.setS(6);
        dupdate_grid(dclosest);
        dtargetCells.push_back(dclosest);
        vector<Cell> dinitial_path = dsoukup_route(dsource);
        if (dinitial_path.size() == 0) {cout << "Detailed routing failed" << endl; return -1;}
        first = 0;
        targetCells.clear();
        dtargetCells.clear();

        for (int j = 1; j < netlist[i].pins.size(); j++){
            if (j == ind) continue;
            for (auto cell : initial_path){targetCells.push_back(cell); }
            initial_path = soukup_route(netlist[i].pins[j]);
            if (initial_path.size() == 0){cout << "Global routing failed" << endl; return -1;}
            for (auto cell : dinitial_path){dtargetCells.push_back(cell); }
            dinitial_path = dsoukup_route(dnetlist[i].pins[j]);
            if (dinitial_path.size() == 0){cout << "Detailed routing failed" << endl; return -1;}
        }
        segments.push_back(current);
        current.clear();
        grid.cleartens();
        targetCells.clear();
        dtargetCells.clear();
    }
    grid.print();
    dgrid.print();
    for (int i = 0 ; i < segments.size(); i++){
        for (int j = 0; j < segments[i].size(); j++){
            segments[i][j].trackIdx *= metalLayers[segments[i][j].metalLayer].pitch;
            segments[i][j].c1 *= metalLayers[segments[i][j].metalLayer + (segments[i][j].metalLayer == n_layers-1? -1 : 1)].pitch;
            segments[i][j].c2 *= metalLayers[segments[i][j].metalLayer + (segments[i][j].metalLayer == n_layers-1? -1 : 1)].pitch;
        }
    }
    return 0;
}
