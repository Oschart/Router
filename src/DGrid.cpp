

DGrid::DGrid(){}

DGrid::DGrid(int height_, int width_, int layers_, vector<int> tracks_, vector<int> tracks_per_gcell_, int mdirect_){
    H = height_;
    W = width_;
    layers = layers_;
    mdirect = mdirect_;

    grid.resize(layers);
    heights.resize(layers);
    widths.resize(layers);

    for (int i = 0 ; i < layers ; i++){
        int dheight, dwidth;
        dwidth = tracks_per_gcell[i];
        dheight = tracks_per_gcell[i + (i == layers-1 ? -1 : 1)];
        if ((i+mdirect)%2 == 0) { swap(dwidth, dheight);}
        heights[i] = dheight;
        widths[i] = dwidth;
        grid[i].assign(H*dheight + 2, vector<Cell>(W*dwidth + 2));
    }


    //grid.assign(H+2, vector<DCell>(W+2));
    for (int k = 0 ; k < layers ; k++)
    for (int i = 0 ; i < grid[k].size() ; i++) for (int j = 0 ; j < grid[k][i].size() ; j++) {
        grid[k][i][j].seti(i); grid[k][i][j].setj(j);
        if (i*j == 0 || i == grid[k].size()-1 || j == grid[k][i].size()-1) {grid[k][i][j].setS(7); grid[k][i][j].setC(0);}
        else {grid[k][i][j].setS(0); grid[k][i][j].setC(0);}
    }

}
//getter for height
int DGrid::getHeight(){
    return H;
}

//getter for width
int DGrid::getWidth(){
    return W;
}

void DGrid::clean(){
    for (int k = 0 ; k < layers ; k++)
    for (int i = 0 ; i < grid[k].size() ; i++)
    for (int j = 0 ; j < grid[k][i].size() ; j++){
        if (grid[k][i][j].getS() < 7) grid[k][i][j].setS(0);
        grid[k][i][j].setC(0);
    }
}
