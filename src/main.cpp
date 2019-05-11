#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
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

            //grid.grid[pin.getk()][pin.geti()][pin.getj()].incCost(i);
            //if (grid.grid[pin.getk()][pin.geti()][pin.getj()].getCost() == limit){pin.setS(7); update_grid(pin);}
            //grid.grid[gk][gi][gj].detailed.grid[pin.geti() % dheight][pin.getj() % dwidth].setS(7);
            dgrid.grid[gk][pin.geti()][pin.getj()].setS(7);
            netlist[i].pins[j].seti(gi);
            netlist[i].pins[j].setj(gj);
            cout << "gi = " << gi << " gj = " << gj << " gk = " << gk << endl;
            grid.grid[gk][gi][gj].incCost(i);
            //netlist[i].pins[j].setk(gk);
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
    cout << "Please enter the width and the height of the global routing grid" << endl;
    int Width, Height;
    cin >> Width >> Height;
    cout << "Please enter the number of nets in your netlist" << endl;
    int n; cin >> n;
    vector<net> netlist;
    for (int i = 0 ; i < n ; i++){
        net mynet;
        cout << "Please enter the number of pins for the " << i+1 << (i == 0? "st" : i == 1? "nd" : i == 2? "rd" : "th") << " net" << endl;
        int m; cin >> m;
        for (int j = 0 ; j < m; j++){
            cout << "please enter the three dimensions for pin number " << j+1 << endl;
            int pi,pj,pk; cin >> pi >> pj >> pk; --pk;
            Cell cell; cell.seti(pi); cell.setj(pj);cell.setk(pk);
            mynet.pins.push_back(cell);
        }
        netlist.push_back(mynet);
    }
    cout << "Please enter the direction for Metal1 (0 for horizontal, 1 for vertical)" << endl;
    cout << "Higher metal layers alternate between horizontal and vertical" << endl;
    cin >> mdirect;

    tracks.push_back(13);
    tracks.push_back(50);
    tracks.push_back(13);
    tracks.push_back(25);
    tracks.push_back(tracks[tracks.size()-2]);
    n_layers = tracks.size();
    tracks_per_gcell.resize(n_layers);
    tracks_per_gcell[n_layers-1] = (int) ceil((double)tracks[n_layers-1] /((n_layers + mdirect)%2 ? Height : Width) - 0.005);
    int k = n_layers - 3;
    while (k >= 0) tracks_per_gcell[k] = tracks_per_gcell[n_layers-1]*tracks[k] / tracks[n_layers-1], k-=2;
    tracks_per_gcell[n_layers-2] = (int) ceil((double)tracks[n_layers-2] /((n_layers + mdirect)%2 ? Width : Height) - 0.005);
    k = n_layers - 4;
    while (k >= 0) tracks_per_gcell[k] = tracks_per_gcell[n_layers-2]*tracks[k] / tracks[n_layers-2], k-=2;
    for (int i = 0 ; i < n_layers ; i++){
        cout << "The number of tracks per gcell for metal " << i << " is " << tracks_per_gcell[i] << endl;
    }
    ratioh = tracks[1];
    ratiov = tracks[0];
    if ((n_layers+mdirect)%2 == 0) swap(ratioh, ratiov);
    for (int i = 0 ; i < n_layers ; i++) metal_directions.push_back((i+mdirect)%2);
    cout << "Please enter the maximum capacity for each global cell" << endl;
    cin >> limit;
    //vector<net> netlist = getFakeNetlist();
    generateBoxSizes(netlist);

    sort(netlist.begin(), netlist.end(), comp);
    cout << "About to create the grid" << endl;
    grid = GGrid(Height, Width, n_layers);
    dgrid = DGrid(Height, Width, n_layers, tracks, tracks_per_gcell, mdirect);
    cout << "Done creating grid" << endl;
//     for (int i = 0 ; i < n_layers; i++) {
//        cout << "Layer :" << i << endl;
//        cout << "Its height = " << grid.grid[i][1][1].detailed.getHeight() << endl;
//        cout << "Its width = " << grid.grid[i][1][1].detailed.getWidth() << endl;
//    }
    cout << "done here" << endl;
    vector<net> dnetlist = reservePins(netlist);
    cout << netlist[0].pins[0].geti() << " " << netlist[0].pins[0].getj() << " " << netlist[0].pins[0].getk() << endl;
    cout << netlist[0].pins[1].geti() << " " << netlist[0].pins[1].getj() << " " << netlist[0].pins[1].getk() << endl;
    cout << "reserving successful" << endl;
    for (int i = 0 ; i < netlist.size(); i++){
        netind = i;
        Cell source = netlist[i].pins[0];

        int ind = getClosest(source, netlist[i].pins);
        Cell closest = netlist[i].pins[ind];
        closest.setS(6);
        update_grid(closest);
        targetCells.push_back(closest);
        first = 1;
        vector<Cell> initial_path = soukup_route(source);
        cout << "Global route successful" << endl;
        Cell dsource = dnetlist[i].pins[0];
        Cell dclosest = dnetlist[i].pins[ind];
        dclosest.setS(6);
        dupdate_grid(dclosest);
        dtargetCells.push_back(dclosest);
        dsoukup_route(dsource);
        cout << "A7A" << endl;
        if (initial_path.size() == 0){metal_directions.push_back((grid.getLayers()+mdirect)%2); grid.addLayer(); initial_path = soukup_route(source);}

        first = 0;
        targetCells.clear();

        for (int j = 1; j < netlist[i].pins.size(); j++){
            if (j == ind) continue;
            for (auto cell : initial_path){cell.setS(6); cell.setC(0); targetCells.push_back(cell); update_grid(cell);}
            initial_path = soukup_route(netlist[i].pins[j]);
            if (initial_path.size() == 0){metal_directions.push_back((grid.getLayers()+mdirect)%2); grid.addLayer(); --j;}
        }
    }
    grid.print();

    return 0;
}
