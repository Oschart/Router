#include "netAbstract.cpp"
//#include "DEF_Writer.h"
using namespace std;

int main()
{
    readLEF();
    readDEF();
    cout << "READ DEF Passed\n";
    abstractNets();
    abstractObs();
    cout << absNets.size() << endl;
    cout << absObs.size() << endl;
    //DEF_Writer defw(metalLayers, getSegments());
    //defw.write_DEF(inDEF);
}
