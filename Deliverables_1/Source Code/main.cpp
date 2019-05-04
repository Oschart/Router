#include <iostream>
#include <algorithm>
#include <stack>
#include "soukup_router.cpp"
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
void reservePins(vector<net>& netlist){
    for (int i = 0 ; i < netlist.size(); i++){
        for (int j = 0 ; j < netlist[i].pins.size(); j++){
            Cell pin = netlist[i].pins[j];
            grid.grid[pin.getk()][pin.geti()][pin.getj()].incCost(i);
            if (grid.grid[pin.getk()][pin.geti()][pin.getj()].getCost() == limit){pin.setS(7); update_grid(pin);}
        }
    }
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
    int layers = getMinLayers(netlist);
    cout << "Please enter the direction for Metal1 (0 for horizontal, 1 for vertical)" << endl;
    cout << "Higher metal layers alternate between horizontal and vertical" << endl;
    int mdirect; cin >> mdirect;
    for (int i = 0 ; i < layers ; i++) metal_directions.push_back((i+mdirect)%2);
    cout << "Please enter the maximum capacity for each global cell" << endl;
    cin >> limit;
    //vector<net> netlist = getFakeNetlist();
    generateBoxSizes(netlist);

    sort(netlist.begin(), netlist.end(), comp);
    grid = GGrid(Height, Width, layers);
    reservePins(netlist);
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
