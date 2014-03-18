// File: 		PVC_Camera.h
// Author:		Travis Bueter
// Description:	

#ifndef PVC_CAMERA_H
#define PVC_CAMERA_H

#include "PVC_Math.h"
#include "PVC_Transform.h"

#define PI 3.14159265

struct CameraState
{
	float Position[3];
	float FocalPoint[3];
	float ViewUp[3];
};

class PVC_Camera
{
public:
	PVC_Camera();
	~PVC_Camera();
	
	void OrthogonalizeViewUp();
	
	void ApplyTransform(PVC_Transform *t);
	
	void ComputeViewTransform();
	
	void ComputeDistance();
	
	void ComputeViewPlaneNormal();
	
	CameraState GetCamState();
	void SetCamState(const CameraState cam);
	void CopyCamState(CameraState &cam);
	
	void GetFocalPoint(float x[3]);
	float *GetFocalPoint() { return camState.FocalPoint; };
	void SetFocalPoint(const float x, const float y, const float z);
	
	void GetViewUp(float x[3]);
	float *GetViewUp() { return camState.ViewUp; };
	void SetViewUp(const float x, const float y, const float z);
	
	void GetPosition(float x[3]);
	float *GetPosition() { return camState.Position; };
	void SetPosition(const float x, const float y, const float z);
	
	void GetDirectionOfProjection(float x[3]);
	float *GetDirectionOfProjection() { return DirectionOfProjection; };
	
	void GetCenter(float x[3]);
	float *GetCenter() { return Center; };
	void SetCenter(/*const float x, const float y, const float z*/);
	
	void GetViewPlaneNormal(float x[3]);
	float *GetViewPlaneNormal() { return ViewPlaneNormal; };
	
	float GetParallelScale() { return ParallelScale; };
	void SetParallelScale(float scale);
	
	void Delete(){ delete this; }
	
	float GetDistance();
	
	CameraState camState;
	
private:
	float viewAngle;
	float DirectionOfProjection[3];
	float Center[3];
	float ViewPlaneNormal[3];
	PVC_Transform *ViewTransform;
	PVC_Transform *Transform;
	float Distance;
	float ParallelScale;
	
};

#include "PVC_Camera.cxx"

#endif // PVC_CAMERA_H