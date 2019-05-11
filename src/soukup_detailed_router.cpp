#include <iostream>
#include <stack>
#include <chrono>
#include "GGrid.h"
#include "DGrid.h"
#define INT_MAX 1000000000

using namespace std;


DGrid dgrid;
int sdi, sdj, sdk, tdi, tdj, tdk; //source and target coordinates
int mdirect;
vector<Cell> dtargetCells; //the list of all possible current target

//struct seg {
//    int metalLayer, trackPos;
//    int c1, c2;
//
//};

vector<seg> segments;


//sets a cell as the target
void dmake_target(Cell cell){
    tdi = cell.geti();
    tdj = cell.getj();
    tdk = cell.getk();
}

int getGCellS(Cell cell){
    int dheight = dgrid.heights[cell.getk()];
    int dwidth = dgrid.widths[cell.getk()];
    int gi = cell.geti() / dheight + 1;
    int gj = cell.getj() / dwidth + 1;
    return grid.grid[cell.getk()][gi][gj].getS();

}

//commits changes in cell to the grid
void dupdate_grid(Cell cell){
    dgrid.grid[cell.getk()][cell.geti()][cell.getj()].setS(cell.getS());
    dgrid.grid[cell.getk()][cell.geti()][cell.getj()].setC(cell.getC());
}

//step3 in the Soukup paper
void dstep3 (stack<Cell>& RN, stack<Cell>& RO, int& goto4, int& goto5, int& goto8){
    while (!RO.empty()){
        Cell cell = RO.top();

        for (int i = 0 ; i < 4 ; i++){
            if (cell.dir[i]) continue; //checking if this neighbor was considered earlier
            RO.top().dir[i] = 1; //marking the neighbor as considered
            if (i > 1 && tdk > cell.getk() && cell.getk() != 0 && cell.getk() != grid.getLayers()-1) i = 5 - i;
            if (i == 2 && cell.getk() == 0 || i == 3 && cell.getk()==grid.getLayers()-1) continue; //unavailable metal layers

            //finding the next neighbor
            int levelk = cell.getk() + (i < 2 ? 0 : i == 2? -1 : 1);
            int levelj = cell.getj() + ((metal_directions[cell.getk()] || i > 1)? 0 : dj[i]);
            int leveli = cell.geti() + ((metal_directions[cell.getk()] == 0 || i > 1)? 0 : dj[i]);
            if (levelk > cell.getk()){ //went up
                if ((cell.getk()+mdirect)%2 == 0){ //went up from horizontal
                    leveli = (leveli - 1)/((cell.getk()+2 >= grid.getLayers()? 1 : tracks[cell.getk()]/tracks[cell.getk()+2]))+1;
                }
                else { //went up from vertical
                    levelj = (levelj - 1)/((cell.getk()+2 >= grid.getLayers()? 1 : tracks[cell.getk()]/tracks[cell.getk()+2]))+1;
                }
            }
            else if (levelk < cell.getk()){ //went down
                if ((cell.getk()+mdirect)%2 == 0){ //went down from horizontal
                    levelj = ((cell.getk() == 0 || cell.getk() == grid.getLayers()-1? 1 : tracks[cell.getk()-1]/tracks[cell.getk()+1]))*levelj;
                }
                else { //went down from vertical
                    leveli = ((cell.getk() == 0 || cell.getk() == grid.getLayers()-1? 1 : tracks[cell.getk()-1]/tracks[cell.getk()+1]))*leveli;
                }
            }

            neighbor = dgrid.grid[levelk][leveli][levelj];
            int GS = getGCellS(neighbor);

            //evaluating the neighbor
            int C = neighbor.getC(), S = neighbor.getS();
            if (C == 2 || S == 7 || GS != 10) continue;
            if (S == 6) { neighbor_traceback = 2 + 4*(i >= 2) - i; goto8 = 1; return;}
            if (C <= 0){ //DFS / Line Search phase
                if (abs(tdi - leveli) + abs(tdj - levelj) + abs(tdk - levelk) < abs(tdi - cell.geti()) + abs(tdj - cell.getj())+abs(tdk - cell.getk())){
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
                dupdate_grid(neighbor);
                RN.push(neighbor);
            }
            if (i > 1 && tk > cell.getk() && cell.getk() != 0 && cell.getk() != grid.getLayers()) i = 5 - i;
        }
        RO.pop();
    }
    goto4 = 1;
}

//steps 6 and 7 in the Soukup paper
void dsteps67(stack<Cell>& RO, int& goto3, int& goto6, int& goto8){
    int dir = 3 + 4*(neighbor_traceback>=3) - neighbor_traceback;
    while (1){
        neighbor.setC(2);
        if (neighbor.getS() <= 4){
            neighbor.setS(neighbor_traceback);
        }
        dupdate_grid(neighbor);
        RO.push(neighbor);

        if (dir < 3) neighbor = dgrid.grid[neighbor.getk()][neighbor.geti()+(metal_directions[neighbor.getk()])*dj[dir-1]][neighbor.getj()+((metal_directions[neighbor.getk()]==0)%2)*dj[dir-1]];
        else {
            if (dir == 4){ //went up
                if ((neighbor.getk()+mdirect)%2 == 0){ //went up from horizontal
                    neighbor.seti((neighbor.geti() - 1)/((neighbor.getk() + 2 >= grid.getLayers()? 1 : tracks[neighbor.getk()]/tracks[neighbor.getk()+2]))+1);
                }
                else { //went up from vertical
                    neighbor.setj((neighbor.getj() - 1)/((neighbor.getk() + 2 >= grid.getLayers()? 1 : tracks[neighbor.getk()]/tracks[neighbor.getk()+2]))+1);
                }
            }
            else if (dir == 3){ //went down
                if ((neighbor.getk()+mdirect)%2 == 0){ //went down from horizontal
                    neighbor.setj(((neighbor.getk() == 0 || neighbor.getk() == grid.getLayers()-1? 1 : tracks[neighbor.getk()-1]/tracks[neighbor.getk()+1]))*neighbor.getj());
                }
                else { //went down from vertical
                    neighbor.seti(((neighbor.getk() == 0 || neighbor.getk() == grid.getLayers()-1? 1 : tracks[neighbor.getk()-1]/tracks[neighbor.getk()+1]))*neighbor.geti());
                }
            }
            neighbor.setk(neighbor.getk() + (dir == 3? 1 : -1));
            neighbor = dgrid.grid[neighbor.getk()][neighbor.geti()][neighbor.getj()];
        }
        int C = neighbor.getC();
        int S = neighbor.getS();
        int GS = getGCellS(neighbor);

        if (C == 2 || S == 7 || GS != 10) {goto3 = 1; break;}
        if (S == 6) {goto8 = 1; break;}
        if(abs(tdi - neighbor.geti()) + abs(tdj - neighbor.getj()) > abs(tdi - RO.top().geti()) + abs(tdj - RO.top().getj())){
            goto3 = 1;
            break;
        }
    }
}

//builds the constructed path and calculates cost
vector<Cell> dtraceback() {
    vector<Cell> path;
    Cell father;
    path.push_back(neighbor);
    neighbor.setS(7);
    dupdate_grid(neighbor);
    int c1, c2, layer, idx;
    layer = neighbor.getk();
    if ((neighbor.getk()+mdirect)%2 == 0){idx = neighbor.geti()-1; c1 = neighbor.getj();}
    else {idx = neighbor.getj()-1 ; c1 = neighbor.geti();}
    int tar = 1;

    //each iteration is a step from the target back to the source
    while (1){
        if (neighbor_traceback >= 3){
            if ((neighbor.getk()+mdirect)%2 == 0)c2 = neighbor.getj();
            else c2 = neighbor.geti();
            if (!tar){
                seg myseg;
                myseg.c1 = c1-1;
                myseg.c2 = c2-1;
                myseg.metalLayer = layer;
                myseg.trackIdx = idx;
                segments.push_back(myseg);
//                cout << "c1 = " << c1 << endl;
//                cout << "c2 = " << c2 << endl;
//                cout << "layer = " << layer << endl;
//                cout << "idx = " << idx << endl;
            }

            if (neighbor_traceback==3) --layer;
            c1 = c2;
            seg myseg;
            myseg.c1 = c1-1;
            myseg.c2 = c2-1;
            myseg.metalLayer = layer;
            myseg.trackIdx = idx;
            segments.push_back(myseg);
//            cout << "c1 = " << c1 << endl;
//            cout << "c2 = " << c2 << endl;
//            cout << "layer = " << layer << endl;
//            cout << "idx = " << idx << endl;

        }
        tar = 0;
        father = neighbor;
        //getting the next neighbor using the traceback grid
        int i = neighbor_traceback - 1;
        int levelk = neighbor.getk() + (i < 2 ? 0 : i == 2? -1 : 1);
        int levelj = neighbor.getj() + ((metal_directions[neighbor.getk()] || i > 1)? 0 : dj[i]);
        int leveli = neighbor.geti() + ((metal_directions[neighbor.getk()] == 0 || i > 1)? 0 : dj[i]);
        if (levelk > neighbor.getk()){ //went up
            if ((neighbor.getk()+mdirect)%2 == 0){ //went up from horizontal
                leveli = (leveli - 1)/((neighbor.getk()+2 >= grid.getLayers()? 1 : tracks[neighbor.getk()]/tracks[neighbor.getk()+2]))+1;
            }
            else { //went up from vertical
                levelj = (levelj - 1)/((neighbor.getk()+2 >= grid.getLayers()? 1 : tracks[neighbor.getk()]/tracks[neighbor.getk()+2]))+1;
            }
        }
        else if (levelk < neighbor.getk()){ //went down
            if ((neighbor.getk()+mdirect)%2 == 0){ //went down from horizontal
                levelj = ((neighbor.getk() == 0 || neighbor.getk() == grid.getLayers()-1? 1 : tracks[neighbor.getk()-1]/tracks[neighbor.getk()+1]))*levelj;
            }
            else { //went down from vertical
                leveli = ((neighbor.getk() == 0 || neighbor.getk() == grid.getLayers()-1? 1 : tracks[neighbor.getk()-1]/tracks[neighbor.getk()+1]))*leveli;
            }
        }
        neighbor = dgrid.grid[levelk][leveli][levelj];
        if (neighbor_traceback >= 3){
            layer = neighbor.getk();
            if ((neighbor.getk()+mdirect)%2 == 0){idx = neighbor.geti()-1; c1 = neighbor.getj();}
            else {idx = neighbor.getj()-1 ; c1 = neighbor.geti();}
        }
        int n_traceback = neighbor.getS();
        neighbor.setS(7);
        path.push_back(neighbor);
        dupdate_grid(neighbor);
        if (n_traceback == 5){
            if (neighbor_traceback < 3){
            if ((neighbor.getk()+mdirect)%2 == 0)c2 = neighbor.getj();
            else c2 = neighbor.geti();
            seg myseg;
            myseg.c1 = c1-1;
            myseg.c2 = c2-1;
            myseg.metalLayer = layer;
            myseg.trackIdx = idx;
            segments.push_back(myseg);
//            cout << "c1 = " << c1 << endl;
//            cout << "c2 = " << c2 << endl;
//            cout << "layer = " << layer << endl;
//            cout << "idx = " << idx << endl;
    }
        }
        neighbor_traceback = n_traceback;

        //found the source
        if (neighbor_traceback == 5) break;
    }

    return path;
}

//initiates a soukup route from a source cell to the closest target
vector<Cell> dsoukup_route(Cell source){
    if (dgrid.grid[source.getk()][source.geti()][source.getj()].getS() == 6) { return vector<Cell>();}
    stack<Cell> RO, RN;

    //marking source and target cells
    source.setS(5);
    source.setC(2);
    Cell target = dtargetCells[getClosest(source, dtargetCells, -1)];
    sdi = source.geti();
    sdj = source.getj();
    sdk = source.getk();
    dmake_target(target);
    target.setC(0);
    target.setS(6);
    dupdate_grid(source);
    dupdate_grid(target);
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
            dstep3(RN, RO, goto4, goto5, goto8);
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
            dsteps67(RO, goto3, goto6, goto8);
        }
        if (goto8){
            //found a solution
            goto8 = 0;
            found = 1;
            path = dtraceback();
            dgrid.clean();
            break;
        }
    }
    if (found) return path;
    else return vector<Cell>();
}

