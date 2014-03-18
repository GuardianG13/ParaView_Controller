void PVC_Interactors::Pan(PVC_Camera *camera, float dx, float dy)
{
	if(camera == NULL)
		return;
	
	float pos[3], fp[3]; 
	camera->GetPosition(pos);
	camera->GetFocalPoint(fp);
	
	camera->OrthogonalizeViewUp();
	float *up = camera->GetViewUp();
	float *vpn = camera->GetViewPlaneNormal();
	float right[3];
	float scale, tmp;
	camera->GetViewUp(up);
	camera->GetViewPlaneNormal(vpn);
	PVC_Math::Cross(vpn, up, right);
	
	scale = camera->GetParallelScale();
	dx *= scale * 2.0;
	dy *= scale * 2.0;
	
	tmp = (right[0]*dx + up[0]*dy);
	pos[0] += tmp;
	fp[0] += tmp; 
	tmp = (right[1]*dx + up[1]*dy); 
	pos[1] += tmp;
	fp[1] += tmp; 
	tmp = (right[2]*dx + up[2]*dy); 
	pos[2] += tmp;
	fp[2] += tmp; 
	camera->SetPosition(pos[0], pos[1], pos[2]);
	camera->SetFocalPoint(fp[0], fp[1], fp[2]);
	
	//socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
}

void PVC_Interactors::Rotate(PVC_Camera *camera, float dx, float dy)
{	
	
	if(camera == NULL)
		return;
	
	PVC_Transform *transform = new PVC_Transform();
	float scale = PVC_Math::Norm(camera->GetPosition());
	if(scale<=0.0)
	{
		scale = PVC_Math::Norm(camera->GetFocalPoint());
		if(scale<=0.0)
		{
			scale = 1.0;
		}
	}
	
	float* temp = camera->GetFocalPoint();
	camera->SetFocalPoint(temp[0]/scale, temp[1]/scale, temp[2]/scale);
	temp = camera->GetPosition();
	camera->SetPosition(temp[0]/scale, temp[1]/scale, temp[2]/scale);

	float v2[3];
	// translate to center
	float* Center = camera->GetCenter();
	transform->Identity();
	transform->Translate(Center[0]/scale, Center[1]/scale, Center[2]/scale);

	//azimuth
	camera->OrthogonalizeViewUp();
	float *viewUp = camera->GetViewUp();
	transform->RotateWXYZ(360.0*dx, viewUp[0], viewUp[1], viewUp[2]);

	//elevation
	PVC_Math::Cross(camera->GetDirectionOfProjection(), viewUp, v2);
	transform->RotateWXYZ(-360.0*dy, v2[0], v2[1], v2[2]);

	// translate back
	transform->Translate(-Center[0]/scale, -Center[1]/scale, -Center[2]/scale);
	camera->ApplyTransform(transform);
	camera->OrthogonalizeViewUp();

	temp = camera->GetFocalPoint();
	camera->SetFocalPoint(temp[0]*scale, temp[1]*scale, temp[2]*scale);
	temp = camera->GetPosition();
	camera->SetPosition(temp[0]*scale, temp[1]*scale, temp[2]*scale);
	
	//socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
	
	transform->Delete();
}

void PVC_Interactors::Dolly(PVC_Camera *camera, float dy)
{
	if(camera == NULL)
		return;
	
	float pos[3], fp[3], vu[3], Ld[3];
	camera->GetPosition(pos);
	camera->GetFocalPoint(fp);
	camera->GetViewUp(vu);
	
	Ld[0] = fp[0]-pos[0];
	Ld[1] = fp[1]-pos[1];
	Ld[2] = fp[2]-pos[2];
	
	pos[0] = pos[0]-Ld[0]*dy*0.2;
	pos[1] = pos[1]-Ld[1]*dy*0.2;
	pos[2] = pos[2]-Ld[2]*dy*0.2;
	
	//socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
}

void PVC_Interactors::Zoom(PVC_Camera *camera, float dy)
{
	if(camera == NULL)
		return;
	
	float pos[3], fp[3], *norm, k, tmp;
	camera->GetPosition(pos);
	camera->GetFocalPoint(fp);
	norm = camera->GetDirectionOfProjection();
	k = dy * 1000;
	
	tmp = k * norm[0];
	pos[0] += tmp;
	fp[0] += tmp;
  
	tmp = k*norm[1];
	pos[1] += tmp;
	fp[1] += tmp;
  
	tmp = k * norm[2];
	pos[2] += tmp;
	fp[2] += tmp;
	
	camera->SetFocalPoint(fp[0], fp[1], fp[2]);
	camera->SetPosition(pos[0], pos[1], pos[2]);
	
	//socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
}



void PVC_Interactors::Roll(PVC_Camera *camera, float dx)
{

	if(camera == NULL)
			return;
		
	PVC_Transform *transform = new PVC_Transform();
	
	float axis[3];
	
	float* fp = camera->GetFocalPoint();
	float* pos = camera->GetPosition();
	axis[0] = fp[0] - pos[0];
	axis[1] = fp[1] - pos[1];
	axis[2] = fp[2] - pos[2];

	// translate to center
	transform->Identity();
	float* Center = camera->GetCenter();
	transform->Translate(Center[0], Center[1], Center[2]);

	transform->RotateWXYZ(-360.0*dx, axis[0], axis[1], axis[2]);

	// translate back
	transform->Translate(-Center[0], -Center[1], -Center[2]);
	camera->ApplyTransform(transform);
	camera->OrthogonalizeViewUp();
	
	//socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
	
	transform->Delete();
}

