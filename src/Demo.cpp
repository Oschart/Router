#include "router.cpp"
#include "DEF_Writer.h"
using namespace std;

int main()
{
    //getSegments();
    DEF_Writer defw(metalLayers, getSegments(), DEF_FILE);
    defw.write_DEF(inDEF);
}