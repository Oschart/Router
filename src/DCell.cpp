//Default Constructor
DCell::DCell(){
    for (int i = 0 ; i < 4 ; i++) dir[i] = 0;
    cost = 0;
}

//Constructor
DCell::DCell(char _S, char _C){
    for (int i = 0 ; i < 4 ; i++) dir[i] = 0;
    S = _S;
    C = _C;
    indi = -1;
    indj = -1;
    cost = 0;
}

//Setter for S
void DCell::setS(char _S){
    S = _S;
}

//Setter for C
void DCell::setC(char _C){
    C = _C;
}

//Getter for S
char DCell::getS(){
    return S;
}

//Getter for C
char DCell::getC(){
    return C;
}

//Getter for i
int DCell::geti(){
    return indi;
}

//Getter for j
int DCell::getj(){
    return indj;
}
//
////Getter for k
int DCell::getk(){
    return indk;
}

//Setter for i
void DCell::seti(int _i){
    indi = _i;
}

//Setter for j
void DCell::setj(int _j){
    indj = _j;
}

////Setter for k
void DCell::setk(int _k){

    indk = _k;
}

//Getter for cost
int DCell::getCost(){
    return cost;
}

////increments the cost
//void DCell::incCost(int netind){
//    if (!cost || nets[nets.size()-1] != netind){
//        nets.push_back(netind);
//        ++cost;
//    }
//}

