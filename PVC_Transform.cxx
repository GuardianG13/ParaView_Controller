// File: 		PVC_Transform.cpp
// Author:		Travis Bueter
// Description:	Class Definition of Transformation matrix

PVC_Transform::PVC_Transform()
{
	this->Matrix = new PVC_Matrix;
	this->PreMatrix = NULL;
}

PVC_Transform::~PVC_Transform()
{
	this->Matrix->Delete();
}

void PVC_Transform::Identity()
{
	this->PreMatrix->Delete();
	this->PreMatrix = NULL;
}

void PVC_Transform::Translate(float x, float y, float z)
{
	if (x == 0.0 && y == 0.0 && z == 0.0)
	{
		return;
	}

    float matrix[4][4];
    PVC_Matrix::Identity(*matrix);

    matrix[0][3] = x;
    matrix[1][3] = y;
    matrix[2][3] = z;
    
    this->Concatenate(*matrix);
}

void PVC_Transform::RotateWXYZ(float angle, float x, float y, float z)
{
	if (angle == 0.0 || (x == 0.0 && y == 0.0 && z == 0.0))
	{
	    return;
	}
	
	angle = angle*0.017453292f;
	
	float w = cos(0.5*angle);
	float f = sin(0.5*angle)/sqrt(x*x+y*y+z*z);
	x *= f;
	y *= f;
	z *= f;
	
	float matrix[4][4];
	PVC_Matrix::Identity(*matrix);
	
	float ww = w*w;
	float wx = w*x;
	float wy = w*y;
	float wz = w*z;
	
	float xx = x*x;
	float yy = y*y;
	float zz = z*z;
	
	float xy = x*y;
	float xz = x*z;
	float yz = y*z;
	
	float s = ww - xx - yy - zz;
	
	matrix[0][0] = xx*2 + s;
	matrix[1][0] = (xy + wz)*2;
	matrix[2][0] = (xz - wy)*2;
	
	matrix[0][1] = (xy - wz)*2;
	matrix[1][1] = yy*2 + s;
	matrix[2][1] = (yz + wx)*2;
	
	matrix[0][2] = (xz + wy)*2;
	matrix[1][2] = (yz - wx)*2;
	matrix[2][2] = zz*2 + s;
	
	this->Concatenate(*matrix);
}

void PVC_Transform::SetupCamera(const float position[3],
						       const float focalPoint[3],
						       const float viewUp[3])
{
  float matrix[4][4];
  PVC_Matrix::Identity(*matrix);

  // the view directions correspond to the rows of the rotation matrix,
  // so we'll make the connection explicit
  float *viewSideways =    matrix[0];
  float *orthoViewUp =     matrix[1];
  float *viewPlaneNormal = matrix[2];

  // set the view plane normal from the view vector
  viewPlaneNormal[0] = position[0] - focalPoint[0];
  viewPlaneNormal[1] = position[1] - focalPoint[1];
  viewPlaneNormal[2] = position[2] - focalPoint[2];
  PVC_Math::Normalize(viewPlaneNormal);

  // orthogonalize viewUp and compute viewSideways
  PVC_Math::Cross(viewUp,viewPlaneNormal,viewSideways);
  PVC_Math::Normalize(viewSideways);
  PVC_Math::Cross(viewPlaneNormal,viewSideways,orthoViewUp);

  // translate by the vector from the position to the origin
  float delta[4];
  delta[0] = -position[0];
  delta[1] = -position[1];
  delta[2] = -position[2];
  delta[3] = 0.0; // yes, this should be zero, not one

  PVC_Matrix::MultiplyPoint(*matrix,delta,delta);

  matrix[0][3] = delta[0];
  matrix[1][3] = delta[1];
  matrix[2][3] = delta[2];

  // apply the transformation
  this->Concatenate(*matrix);
}

void PVC_Transform::Concatenate(const float elements[16])
{
	if(this->PreMatrix == NULL)
	{
		this->PreMatrix = new PVC_Matrix;
	}
	PVC_Matrix::Multiply4x4(*this->PreMatrix->Element, elements,
	                          *this->PreMatrix->Element);
}

void PVC_Transform::Update()
{
	this->Matrix->Identity();
	if(PreMatrix != NULL)
	{
		PVC_Matrix::Multiply4x4(this->Matrix, this->PreMatrix, this->Matrix);
	}
	
}



