// File: 		PVC_Transform.h
// Author:		Travis Bueter
// Description:	

#ifndef PVC_TRANSFORM_H
#define PVC_TRANSFORM_H

#include "PVC_Matrix.h"

class PVC_Transform
{
public:
	PVC_Transform();
	~PVC_Transform();
	
	void Identity();
	void Translate(float x, float y, float z);
	void RotateWXYZ(float angle, float x, float y, float z);
	PVC_Matrix *GetMatrix(){ this->Update(); return this->Matrix; };
	void SetupCamera(const float position[3], const float focalPoint[3], const float viewUp[3]);
	void SetMatrix(PVC_Matrix* matrix) {
		this->SetMatrix(*matrix->Element); };
	void SetMatrix(const float elements[16]) {
		this->Identity(); this->Concatenate(elements); };
	void Delete() { delete this; };
	void Update();
	void Concatenate(const float elements[16]);
	
	// Description:
	// Compute the transformation in homogeneous (x,y,z,w) coordinates.
	// This method calls this->GetMatrix()->MultiplyPoint().
	void MultiplyPoint(const float in[4], float out[4]) {
	  this->GetMatrix()->MultiplyPoint(in,out);};
	
private:
	PVC_Matrix *Matrix;
	PVC_Matrix *PreMatrix;

};

#include "PVC_Transform.cxx"

#endif // PVC_TRANSFORM_H