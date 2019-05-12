#include "router.cpp"
#include "DEF_Writer.h"
using namespace std;

int main()
{
    DEF_Writer defw(metalLayers, getSegments());
    defw.write_DEF(inDEF);
}
