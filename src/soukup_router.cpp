#include <iostream>
#include <stack>
#include <chrono>
#include "GGrid.h"
#define INT_MAX 1000000000

using namespace std;

//helper array
int dj[] = {-1, 1};

GGrid grid; //the maze
int si, sj, sk, ti, tj, tk; //source and target coordinates
Cell neighbor; //used for searching for the target and reconstructing the path
int neighbor_traceback; //used for reconstructing the path
int netind; //index of the current net
vector<int> metal_directions; //directions of each metal layer
vector<Cell> targetCells; //the list of all possible current target
int limit; //the global maximum capacity for all Gcells
bool first; //a flag for denoting the first round of routing for a net involving more than two pins
vector<int> tracks;
int n_layers;
vector<int> tracks_per_gcell;



//sets a cell as the target
void make_target(Cell cell){
    ti = cell.geti();
    tj = cell.getj();
    tk = cell.getk();
}

//gets the closest cell from a list of targets to a particular source cell
int getClosest(Cell source, vector<Cell>& targets){
    int mindist = INT_MAX; int ind;
    for (int j = 0 ; j < targets.size(); j++){
        if (source.geti() == targets[j].geti() && source.getj() == targets[j].getj() &&source.getk() == targets[j].getk()) continue;
        int dist =  abs(source.geti()-targets[j].geti()) + \
                    abs(source.getj()-targets[j].getj()) + \
                    abs(source.getk()-targets[j].getk());
        if (dist < mindist){
                mindist = dist;
                ind = j;
        }
    }
    return ind;
}


//commits changes in cell to the grid
void update_grid(Cell cell){
    grid.grid[cell.getk()][cell.geti()][cell.getj()].setS(cell.getS());
    grid.grid[cell.getk()][cell.geti()][cell.getj()].setC(cell.getC());
}

//step2 in the Soukup paper
void step2 (stack<Cell>& RN, stack<Cell>& RO){
    stack<Cell> news;
    while (!RN.empty()) {news.push(RN.top());  RN.pop();}
    while (!news.empty()) {RO.push(news.top()); news.pop();}
}

//step5 in the Soukup paper
void step5 (stack<Cell>& RN, stack<Cell>& RO){
    while (!RN.empty()) {RO.push(RN.top()); RN.pop();}
}

//step3 in the Soukup paper
void step3 (stack<Cell>& RN, stack<Cell>& RO, int& goto4, int& goto5, int& goto8){
    while (!RO.empty()){
        Cell cell = RO.top();

        for (int i = 0 ; i < 4 ; i++){

            if (cell.dir[i]) continue; //checking if this neighbor was considered earlier
            RO.top().dir[i] = 1; //marking the neighbor as considered
            if (i > 1 && tk > cell.getk() && cell.getk() != 0 && cell.getk() != grid.getLayers()-1) i = 5 - i;
            if (i == 2 && cell.getk() == 0 || i == 3 && cell.getk()==grid.getLayers()-1) continue; //unavailable metal layers

            //finding the next neighbor
            int levelk = cell.getk() + (i < 2 ? 0 : i == 2? -1 : 1);
            int levelj = cell.getj() + ((metal_directions[cell.getk()] || i > 1)? 0 : dj[i]);
            int leveli = cell.geti() + ((metal_directions[cell.getk()] == 0 || i > 1)? 0 : dj[i]);
            neighbor = grid.grid[levelk][leveli][levelj];
//            cout << "current: " << endl;
//            cout << cell.geti() << " " << cell.getj() << " " << cell.getk() << endl;
//            cout << "new" << endl;
//            cout << leveli << " " << levelj << " " << levelk << endl;

            //evaluating the neighbor
            int C = neighbor.getC(), S = neighbor.getS();
            if (C == 2 || S == 7) continue;
            if (S == 6) { neighbor_traceback = 2 + 4*(i >= 2) - i; goto8 = 1; return;}
            if (C <= 0){ //DFS / Line Search phase
                if (abs(ti - leveli) + abs(tj - levelj) + abs(tk - levelk) < abs(ti - cell.geti()) + abs(tj - cell.getj())+abs(tk - cell.getk())){
                    goto5 = 1;
                    neighbor_traceback = 2 + 4*(i >= 2) - i;
                    return;
                }
            }
            if (C == 0){ //BFS/Lee expansion phase
                neighbor.setC(1);
                if (S <= 4){
                    neighbor.setS(2 + 4*(i >= 2) - i);
                }
                update_grid(neighbor);
                RN.push(neighbor);
            }
            if (i > 1 && tk > cell.getk() && cell.getk() != 0 && cell.getk() != grid.getLayers()) i = 5 - i;
        }
        RO.pop();
    }
    goto4 = 1;
}

//steps 6 and 7 in the Soukup paper
void steps67(stack<Cell>& RO, int& goto3, int& goto6, int& goto8){
    int dir = 3 + 4*(neighbor_traceback>=3) - neighbor_traceback;
    //finding next neighbor along the same line
    while (1){
        neighbor.setC(2);
        if (neighbor.getS() <= 4){
            neighbor.setS(neighbor_traceback);
        }

        update_grid(neighbor);
        RO.push(neighbor);

        if (dir < 3) neighbor = grid.grid[neighbor.getk()][neighbor.geti()+(metal_directions[neighbor.getk()])*dj[dir-1]][neighbor.getj()+((metal_directions[neighbor.getk()]==0)%2)*dj[dir-1]];
        else neighbor = grid.grid[neighbor.getk() + (dir == 3? 1 : -1)][neighbor.geti()][neighbor.getj()];
//        cout << "The new cell on the linesearch: " << endl;
//        cout << neighbor.geti() << " " << neighbor.getj() << " " << neighbor.getk() << endl;
//        cout << (int)neighbor.getS() << endl;
        int C = neighbor.getC();
        int S = neighbor.getS();
        if (C == 2 || S == 7) {goto3 = 1; break;}
        if (S == 6) {goto8 = 1; break;}
        if(abs(ti - neighbor.geti()) + abs(tj - neighbor.getj()) > abs(ti - RO.top().geti()) + abs(tj - RO.top().getj())){
            goto3 = 1;
            break;
        }
    }
}

//builds the constructed path and calculates cost
vector<Cell> traceback() {
    vector<Cell> path;
    Cell father;
    path.push_back(neighbor);
    neighbor.setS(10);
    update_grid(neighbor);
    if (!first) {
        grid.grid[neighbor.getk()][neighbor.geti()][neighbor.getj()].incCost(netind);
        if (grid.grid[neighbor.getk()][neighbor.geti()][neighbor.getj()].getCost() == limit) {neighbor.setS(7); update_grid(neighbor);}
    }
    update_grid(neighbor);

    //each iteration is a step from the target back to the source
    while (1){

        father = neighbor;
        //getting the next neighbor using the traceback grid
        int i = neighbor_traceback - 1;
        int levelk = neighbor.getk() + (i < 2 ? 0 : i == 2? -1 : 1);
        int levelj = neighbor.getj() + ((metal_directions[neighbor.getk()] || i > 1)? 0 : dj[i]);
        int leveli = neighbor.geti() + ((metal_directions[neighbor.getk()] == 0 || i > 1)? 0 : dj[i]);
        neighbor = grid.grid[levelk][leveli][levelj];
        int n_traceback = neighbor.getS();

        //denoting the route this cell belongs to
        if (n_traceback != 5){
            grid.grid[neighbor.getk()][neighbor.geti()][neighbor.getj()].incCost(netind);
            if (grid.grid[neighbor.getk()][neighbor.geti()][neighbor.getj()].getCost() == limit) {neighbor.setS(7); update_grid(neighbor);}
        }
        path.push_back(neighbor);
        neighbor.setS(10);
        update_grid(neighbor);
        neighbor_traceback = n_traceback;

        //found the source
        if (neighbor_traceback == 5) break;
    }
    return path;
}

//initiates a soukup route from a source cell to the closest target
vector<Cell> soukup_route(Cell source){
    if (grid.grid[source.getk()][source.geti()][source.getj()].getS() == 6) { return vector<Cell>();}
    stack<Cell> RO, RN;

    //marking source and target cells
    source.setS(5);
    source.setC(2);
    Cell target = targetCells[getClosest(source, targetCells)];
    si = source.geti();
    sj = source.getj();
    sk = source.getk();
    make_target(target);
    update_grid(source);
//    cout << "source info:" << endl;
//    cout << source.geti() << " " << source.getj() << " " << source.getk()<< " " << (int)source.getS() << endl;
//    cout << (int)grid.grid[source.getk()][source.geti()][source.getj()].getS() << endl;
//    cout << "target info:" << endl;
//    cout << ti << " " << tj << " " << tk << endl;
//    cout << (int)grid.grid[tk][ti][tj].getS() << endl;
    target.setC(0);
    update_grid(target);
    RN.push(source);


    //each goto flag marks the next step to execute
    int goto2 = 1, goto3 = 0, goto4 = 0,  goto5 = 0, goto6 = 0, goto8 = 0;
    int found = 0;
    vector<Cell> path;
    while (1){
        if (goto2){
            goto2 = 0;
            step2(RN, RO);
            goto3 = 1;
        }
        if (goto3){
            goto3 = 0;
            step3(RN, RO, goto4, goto5, goto8);
        }
        if (goto4){
            goto4 = 0;
            if (RN.empty()) break;
            goto2 = 1;
        }
        if (goto5){
            goto5 = 0;
            step5(RN, RO);
            goto6 = 1;
        }
        if (goto6){
            goto6 = 0;
            steps67(RO, goto3, goto6, goto8);
        }
        if (goto8){
            //found a solution
            goto8 = 0;
            found = 1;
            path = traceback();
            grid.clean();
            break;
        }
    }
    if (found) return path;
    else return vector<Cell>();
}

