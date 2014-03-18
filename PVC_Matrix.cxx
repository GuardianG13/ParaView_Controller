///////////////////////////////////////////////////////////////////////////
/// File:   PVC_Matrix.cxx
/// Author: Travis Bueter - IT Research Support Services
/// Brief:  This c++ source file contains the implementations of the  
///         PVC_Matrix.h functions
///////////////////////////////////////////////////////////////////////////

// Useful for treating a float[16] as a float[4][4]
typedef float (*SqMatPtr)[4];

PVC_Matrix::PVC_Matrix()
{
    //Set matrix element to identity matrix
	this->Identity(*this->Element);
}

PVC_Matrix::~PVC_Matrix()
{
	
}

void PVC_Matrix::Identity()
{
	this->Identity(*this->Element);
}

void PVC_Matrix::Identity(float Elements[16])
{
    // Main diagonal elements of matrix
	Elements[0] = Elements[5] = Elements[10] = Elements[15] = 1.0;
    
    // Other elements
	Elements[1] = Elements[2] = Elements[3] = Elements[4] =
	Elements[6] = Elements[7] = Elements[8] = Elements[9] =
	Elements[11] = Elements[12] = Elements[13] = Elements[14] = 0.0;
}

void PVC_Matrix::MultiplyPoint(const float t[16], const float in[4], float out[4])
{
    // Store input values
	float v1 = in[0];
	float v2 = in[1];
	float v3 = in[2];
	float v4 = in[3];
    
    // Multiplying 1x4 matrix by 4x4 matrix :
	out[0] = v1*t[0]  + v2*t[1]  + v3*t[2]  + v4*t[3];
	out[1] = v1*t[4]  + v2*t[5]  + v3*t[6]  + v4*t[7];
	out[2] = v1*t[8]  + v2*t[9]  + v3*t[10] + v4*t[11];
	out[3] = v1*t[12] + v2*t[13] + v3*t[14] + v4*t[15];
}

void PVC_Matrix::Multiply4x4(const float InputA[16], const float InputB[16], float Output[16])
{
	SqMatPtr aMat = (SqMatPtr) InputA; // Treat InputA as a 4x4 matrix
	SqMatPtr bMat = (SqMatPtr) InputB; // Treat InputB as a 4x4 matrix
	SqMatPtr cMat = (SqMatPtr) Output; // Treat Output as a 4x4 matrix
	int i, k; // indexes for loops
	float Accum[4][4]; // Accumulator
	
    // Multiplying 4x4 matrix 'InputA' by 4x4 matrix 'InputB':
	for (i = 0; i < 4; i++)
	{
		for (k = 0; k < 4; k++)
		{
		  Accum[i][k] = aMat[i][0] * bMat[0][k] +
						aMat[i][1] * bMat[1][k] +
						aMat[i][2] * bMat[2][k] +
						aMat[i][3] * bMat[3][k];
		}
	}
	
	// Copy result to output
	for (i = 0; i < 4; i++)
	{
		cMat[i][0] = Accum[i][0];
		cMat[i][1] = Accum[i][1];
		cMat[i][2] = Accum[i][2];
		cMat[i][3] = Accum[i][3];
	}
}