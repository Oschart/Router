#include <iomanip>

//default constructor
GGrid::GGrid(){
}

//constructor
GGrid::GGrid (int W, int H, int layers_){
    layers = layers_;
    height = H;
    width = W;
    grid.resize(layers);
    for (int i = 0; i < layers; i++)
        grid[i].assign(H+2, vector<Cell>(W+2));

    for (int k = 0 ; k < layers ; k++)
    for (int i = 0 ; i < H+2 ; i++) for (int j = 0 ; j < W+2 ; j++) {
        grid[k][i][j].seti(i); grid[k][i][j].setj(j); grid[k][i][j].setk(k);
        if (i*j == 0 || i == H+1 || j == W+1) {grid[k][i][j].setS(7); grid[k][i][j].setC(0);}
        else {grid[k][i][j].setS(0); grid[k][i][j].setC(0);}
    }
}

//getter for height
int GGrid::getHeight(){
    return height;
}

//getter for width
int GGrid::getWidth(){
    return width;
}

//getter for layers
int GGrid::getLayers(){
    return layers;
}

//displays the grid
void GGrid::print(){
    for (int k = 0 ; k < layers ; k++){
        cout << "Metal " << k+1 << ":\n";
        for (int i = 0 ; i < 3 * width + 2 ; i++) cout << "=";
        cout << endl;
        for (int i = 1 ; i <= height ; i++){
            cout << "|";
            for (int j = 1 ; j <= width ; j++){
                cout << setw(2);
                if (grid[k][i][j].getCost() == 0) cout << " ";
                //else if (grid[k][i][j].getCost() == 7) cout << "X";
                else cout << (int)grid[k][i][j].getCost();
                cout << " ";
            }
            cout << "|";
            cout << endl;
        }
        for (int i = 0 ; i < 3 * width + 2 ; i++) cout << "=";
        cout << endl;
    }
}

//clears up the grid after routing of a net
void GGrid::clean(){
    for (int k = 0 ; k < layers ; k++)
    for (int i = 0 ; i < height+2 ; i++)
    for (int j = 0 ; j < width+2 ; j++){
        if (grid[k][i][j].getS() < 7) grid[k][i][j].setS(0);
        grid[k][i][j].setC(0);
    }
}

void GGrid::cleartens(){
    for (int k = 0 ; k < layers ; k++)
    for (int i = 0 ; i < height+2 ; i++)
    for (int j = 0 ; j < width+2 ; j++){
        grid[k][i][j].allowed = 0;
    }
}

//adds a new layer to the global grid
void GGrid::addLayer(){
    grid.push_back(vector<vector<Cell>>(height+2, vector<Cell>(width+2)));
    for (int i = 0 ; i < height+2 ; i++) for (int j = 0 ; j < width+2 ; j++) {
        grid[layers][i][j].seti(i); grid[layers][i][j].setj(j); grid[layers][i][j].setk(layers);
        if (i*j == 0 || i == height+1 || j == width+1) {grid[layers][i][j].setS(7); grid[layers][i][j].setC(0);}
        else {grid[layers][i][j].setS(0); grid[layers][i][j].setC(0);}
    }
    ++layers;
}
