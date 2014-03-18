// File: 		WiiMain.cpp
// Author:		Travis Bueter
// Description:	Application of WiiMote interaction with ParaView

#include "PVC_Wii.h"

using namespace std;

int main(int argc, char** argv)
{
    PVC_Wii wii;
    wii.FindWiimote();
    wii.Run();
    return 0;
}