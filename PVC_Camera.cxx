

PVC_Camera::PVC_Camera()
{

	this->camState.FocalPoint[0] = 0.0;
	this->camState.FocalPoint[1] = 0.0;
	this->camState.FocalPoint[2] = 0.0;
	
	this->camState.ViewUp[0] = 0.0;
	this->camState.ViewUp[1] = 1.0;
	this->camState.ViewUp[2] = 0.0;	
	
	this->camState.Position[0] = 0.0;
	this->camState.Position[1] = 0.0;
	this->camState.Position[2] = 1.0;
	
	this->DirectionOfProjection[0] = 0.0;
	this->DirectionOfProjection[1] = 0.0;
	this->DirectionOfProjection[2] = 0.0;
	
	this->Center[1] = 0.0;
	this->Center[2] = 0.0;
	this->Center[3] = 0.0;
	
	this->ViewPlaneNormal[1] = 0.0;
	this->ViewPlaneNormal[2] = 0.0;
	this->ViewPlaneNormal[3] = 0.0;
	
	this->Distance = 0.0;
	
	this->ParallelScale = 1.0;
	
	this->viewAngle = 30;
	
	ViewTransform = new PVC_Transform;
	Transform = new PVC_Transform;
}

PVC_Camera::~PVC_Camera()
{
	ViewTransform->Delete();
	Transform->Delete();
}

void PVC_Camera::OrthogonalizeViewUp()
{
	PVC_Matrix *matrix = this->ViewTransform->GetMatrix();
	
	this->camState.ViewUp[0] = matrix->GetElement(1,0);
	this->camState.ViewUp[1] = matrix->GetElement(1,1);
	this->camState.ViewUp[2] = matrix->GetElement(1,2);
}

void PVC_Camera::ApplyTransform(PVC_Transform *t)
{
	float posOld[4], posNew[4], fpOld[4], fpNew[4], vuOld[4], vuNew[4];
	for(int i = 0; i < 3; i++)
	{
		fpOld[i] = camState.FocalPoint[i];
		vuOld[i] = camState.ViewUp[i];
		posOld[i] = camState.Position[i];
	}
	
	posOld[3] = 1.0;
	fpOld[3] = 1.0;
	vuOld[3] = 1.0;

	vuOld[0] += posOld[0];
	vuOld[1] += posOld[1];
	vuOld[2] += posOld[2];
	
	t->MultiplyPoint(posOld, posNew);
	t->MultiplyPoint(fpOld, fpNew);
	t->MultiplyPoint(vuOld, vuNew);
	
	vuNew[0] -= posNew[0];
	vuNew[1] -= posNew[1];
	vuNew[2] -= posNew[2];
	
	this->SetPosition(posNew[0], posNew[1], posNew[2]);
	this->SetFocalPoint(fpNew[0], fpNew[1], fpNew[2]);
	this->SetViewUp(vuNew[0], vuNew[1], vuNew[2]);
}

CameraState PVC_Camera::GetCamState()
{
	return this->camState;
}

void PVC_Camera::SetCamState(const CameraState cam)
{
	memcpy(&camState, &cam, sizeof(CameraState));
}

void PVC_Camera::CopyCamState(CameraState &cam)
{
	memcpy(&cam, &camState, sizeof(CameraState));
}

void PVC_Camera::GetFocalPoint(float x[3])
{
	for(int i = 0; i < 3; i++)
	{
		x[i] = this->camState.FocalPoint[i];
	}
}

void PVC_Camera::SetFocalPoint(float x, float y, float z)
{
	if(x == this->camState.FocalPoint[0] && 
	   y == this->camState.FocalPoint[1] &&
	   z == this->camState.FocalPoint[2])
	{
		return;
	}
	
	this->camState.FocalPoint[0] = x;
	this->camState.FocalPoint[1] = y;
	this->camState.FocalPoint[2] = z;
	
	this->ComputeViewTransform();
	this->ComputeDistance();
	this->ViewTransform->Update();
}

void PVC_Camera::GetViewUp(float x[3])
{
	for(int i = 0; i < 3; i++)
	{
		x[i] = this->camState.ViewUp[i];
	}
}

void PVC_Camera::SetViewUp(float x, float y, float z)
{
	// normalize ViewUp, but do _not_ orthogonalize it by default
	float norm = sqrt(x*x + y*y + z*z);
	
	if(norm != 0)
	{
		x /= norm;
		y /= norm;
		z /= norm;
	}
	else
	{
		x = 0;
		y = 1;
		z = 0;
	}
	
	if (x == camState.ViewUp[0] &&
		y == camState.ViewUp[1] &&
		z == camState.ViewUp[2])
	{
		return;
	}
	
	camState.ViewUp[0] = x;
	camState.ViewUp[1] = y;
	camState.ViewUp[2] = z;
	
	ComputeViewTransform();
	ComputeDistance();
	this->ViewTransform->Update();
}

void PVC_Camera::GetPosition(float x[3])
{
	for(int i = 0; i < 3; i++)
	{
		x[i] = this->camState.Position[i];
	}
}

void PVC_Camera::SetPosition(float x, float y, float z)
{
	if(x == this->camState.Position[0] && 
	   y == this->camState.Position[1] &&
	   z == this->camState.Position[2])
	{
		return;
	}
	
	this->camState.Position[0] = x;
	this->camState.Position[1] = y;
	this->camState.Position[2] = z;
	
	this->ComputeViewTransform();
	this->ComputeDistance();
	this->ViewTransform->Update();
}

void PVC_Camera::GetDirectionOfProjection(float x[3])
{
	for(int i = 0; i < 3; i++)
	{
		x[i] = this->DirectionOfProjection[i];
	}
}

void PVC_Camera::GetCenter(float x[3])
{
	for(int i = 0; i < 3; i++)
	{
		x[i] = this->Center[i];
	}
}

void PVC_Camera::SetCenter(/*float x, float y, float z*/)
{
	/*if(x == this->Center[0] && 
	   y == this->Center[1] &&
	   z == this->Center[2])
	{
		return;
	}
	
	this->Center[0] = x;
	this->Center[1] = y;
	this->Center[2] = z;*/

	this->GetFocalPoint(this->Center);
}

void PVC_Camera::GetViewPlaneNormal(float x[3])
{
	for(int i = 0; i < 3; i++)
	{
		x[i] = this->ViewPlaneNormal[i];
	}
}

void PVC_Camera::SetParallelScale(float scale)
{
	this->ParallelScale = scale;
}

void PVC_Camera::ComputeViewTransform()
{
	this->Transform->Identity();
	this->Transform->SetupCamera(this->camState.Position, this->camState.FocalPoint, this->camState.ViewUp);
	this->ViewTransform->SetMatrix(this->Transform->GetMatrix());
}

void PVC_Camera::ComputeViewPlaneNormal()
{
	this->ViewPlaneNormal[0] = -this->DirectionOfProjection[0];
	this->ViewPlaneNormal[1] = -this->DirectionOfProjection[1];
	this->ViewPlaneNormal[2] = -this->DirectionOfProjection[2];
}

void PVC_Camera::ComputeDistance()
{
	float dx = this->camState.FocalPoint[0] - this->camState.Position[0];
	float dy = this->camState.FocalPoint[1] - this->camState.Position[1];
	float dz = this->camState.FocalPoint[2] - this->camState.Position[2];
	
	this->Distance = sqrt(dx*dx + dy*dy + dz*dz);
	
	if (this->Distance < 1e-20)
	{
		this->Distance = 1e-20;		
		float *vec = this->DirectionOfProjection;
		
		// recalculate FocalPoint
		this->camState.FocalPoint[0] = this->camState.Position[0] + vec[0]*this->Distance;
		this->camState.FocalPoint[1] = this->camState.Position[1] + vec[1]*this->Distance;
		this->camState.FocalPoint[2] = this->camState.Position[2] + vec[2]*this->Distance;
	}
	
	this->DirectionOfProjection[0] = dx/this->Distance;
	this->DirectionOfProjection[1] = dy/this->Distance;
	this->DirectionOfProjection[2] = dz/this->Distance;
	
	this->ComputeViewPlaneNormal();
}

float PVC_Camera::GetDistance()
{
	float distance;
	float dx = this->Center[0] - this->camState.Position[0];
	float dy = this->Center[1] - this->camState.Position[1];
	float dz = this->Center[2] - this->camState.Position[2];
	
	distance = sqrt(dx*dx + dy*dy + dz*dz);
	
	if (distance < 1e-20)
	{
		distance = 1e-20;		
	}
	
	return distance;
}