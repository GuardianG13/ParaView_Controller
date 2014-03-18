// File:		PVC_Interactors.h
// Author:		Travis Bueter
// Description: Function Definitions of ParaView Interactors

#ifndef PVC_INTERACTORS_H
#define PVC_INTERACTORS_H

#include "PVC_Camera.h"

class PVC_Interactors
{
public:
	static void Pan(PVC_Camera *camera, float dx, float dy);
	static void Rotate(PVC_Camera *camera, float dx, float dy);
	static void Dolly(PVC_Camera *camera, float dy);
	static void Zoom(PVC_Camera *camera, float dy);
	static void Roll(PVC_Camera *camera, float dx);
	
private:
	
};

#include "PVC_Interactors.cxx"

#endif //PVC_INTERACTORS_H




