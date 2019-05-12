//Default Constructor
Cell::Cell(){
    for (int i = 0 ; i < 4 ; i++) dir[i] = 0;
    cost = 0;
}

//Constructor
Cell::Cell(char _S, char _C){
    for (int i = 0 ; i < 4 ; i++) dir[i] = 0;
    S = _S;
    C = _C;
    indi = -1;
    indj = -1;
    cost = 0;
}

//Setter for S
void Cell::setS(char _S){
    S = _S;
}

//Setter for C
void Cell::setC(char _C){
    C = _C;
}

//Getter for S
char Cell::getS(){
    return S;
}

//Getter for C
char Cell::getC(){
    return C;
}

//Getter for i
int Cell::geti(){
    return indi;
}

//Getter for j
int Cell::getj(){
    return indj;
}

//Getter for k
int Cell::getk(){
    return indk;
}

//Setter for i
void Cell::seti(int _i){
    indi = _i;
}

//Setter for j
void Cell::setj(int _j){
    indj = _j;
}

//Setter for k
void Cell::setk(int _k){

    indk = _k;
}

//Getter for cost
int Cell::getCost(){
    return cost;
}

//increments the cost
void Cell::incCost(int netind){
    if (!cost || nets[nets.size()-1] != netind){
        nets.push_back(netind);
        ++cost;
    }
}

