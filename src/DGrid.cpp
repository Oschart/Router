

DGrid::DGrid(){}

DGrid::DGrid(int height_, int width_, int layer_, vector<int> tracks_, vector<int> tracks_per_gcell_){
    height = height_;
    width = width_;
    layer = layer_;
    grid.assign(height, vector<Cell>(width));

}
