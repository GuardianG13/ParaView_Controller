///////////////////////////////////////////////////////////////////////////
/// File:   PVC_Matrix.h
/// Author: Travis Bueter - IT Research Support Services
/// Brief:  This header file declares the 4x4 matrix class for the  
///         PVController program.
///////////////////////////////////////////////////////////////////////////

#ifndef PVC_MATRIX_H
#define PVC_MATRIX_H

///////////////////////////////////////////////////////////////////////////
/// Class:  PVC_Matrix
/// Brief:  This class defines a 4x4 matrix of floats, initialized as an 
///         identity matrix. It also includes functions to allow matrix 
///         multiplication and point multiplication. All matrices must be
///         4x4 square matrices and all points must have exactly 4 elements.
///         All values will be treated as floats and must be compatible. 
///         16 element arrays may also be used and will be treated as such:
///         
///         One dimensional array:
///           float a[16] = | 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 |
///
///         Two dimensional array:
///                           | (0,0) (0,1) (0,2) (0,3) |   |  0  1  2  3 |   
///                           | (1,0) (1,1) (1,2) (1,3) |   |  4  5  6  7 |
///           float b[4][4] = | (2,0) (2,1) (2,2) (2,3) | = |  8  9 10 11 |
///                           | (3,0) (3,1) (3,2) (3,3) |   | 12 13 14 15 |
///
///         In other words:
///           For a[z] = b[x][y], z = 4*x + y; where x & y are in range [0,3]
///
///////////////////////////////////////////////////////////////////////////

class PVC_Matrix
{
public:
	
    float Element[4][4]; ///< 4x4 matrix used for linear transformations
	
    // Constructor
	PVC_Matrix();
    // Deconstructor
	~PVC_Matrix();
	
    // I do not remember why I included this function. It was used in paraview's matrix class but
    // does not appear to be necessary in this implementation. Function is not called.
	void Delete(){ delete this;}
    
    ///////////////////////////////////////////////////////////////////////
    /// Func:   void PVC_Matrix::Identity()
    /// Brief:  Calls PVC_Matrix::Identity( float[16] ). Allows the 4x4
    ///         to be treated as a 16 element array.
    /// Pre:    None
    /// Post:   Element variable set to identity matrix
    ///////////////////////////////////////////////////////////////////////
	void Identity();
    
    ///////////////////////////////////////////////////////////////////////
    /// Func:   static void PVC_Matrix::Identity( float Elements[16] )
    /// Brief:  Sets Elements to the array representation of the identity 
    ///         matrix 
    /// Pre:    4x4 matrix of floats must be passed in as a pointer
    /// Post:   Elements set to the array represenation of the idenity
    ///         matrix
    /// Param:  Elements - array representation of 4x4 matrix
    ///////////////////////////////////////////////////////////////////////
	static void Identity(float Elements[16]);
    
    ///////////////////////////////////////////////////////////////////////
    /// Func:   static void PVC_Matrix::Multiply4x4( const float InputA[16],
    ///                                             const float InputB[16],
    ///                                             float Output[16] )
    /// Brief:  Multiplies InputA and InputB as 4x4 matrices and stores the
    ///         results in Output
    /// Pre:    4x4 matrix of floats must be passed in as a pointer. 
    /// Post:   Output is multiplication of InputA and InputB as 4x4
    ///         matrices
    /// Param:  InputA - Array representation of first matrix to be 
    ///                  multiplied
    ///         InputB - Array representation of second matrix to be
    ///                  multiplied
    ///         Output - Array representation of multiplication output
    ///////////////////////////////////////////////////////////////////////
	static void Multiply4x4(const float InputA[16], const float InputB[16], float Output[16]);
	
    ///////////////////////////////////////////////////////////////////////
    /// Func:   static void PVC_Matrix::Multiply4x4( const PVC_Matrix *InputA,
    ///                                             const PVC_Matrix *InputB,
    ///                                             PVC_Matrix *Output )
    /// Brief:  Calls PVC_Matrix::Multiply4x4( const float[16], const float[16],
    ///         float[16] ) and passes in the PVC_Matrix objects' Element
    ///         variables as pointers
    /// Pre:    None
    /// Post:   Output->Element contains the multiplication of
    ///         InputA->Element and InputB->Element as 4x4 matrices
    /// Param:  InputA - PVC_Matrix object that contains first matrix to be
    ///                  multiplied
    ///         InputB - PVC_Matrix object that contains second matrix to be
    ///                  multiplied
    ///         Output - PVC_Matrix object that stores result in it's Element
    ///                  variable
    ///////////////////////////////////////////////////////////////////////
	static void Multiply4x4(const PVC_Matrix *InputA, const PVC_Matrix *InputB, PVC_Matrix *Output) {
	  PVC_Matrix::Multiply4x4(*InputA->Element,*InputB->Element,*Output->Element); };
	
    ///////////////////////////////////////////////////////////////////////
    /// Func:   static void PVC_Matrix::MultiplyPoint( const float t[16],
    ///                                               const float in[4],
    ///                                               float out[4] )
    /// Brief:  Multiply a homogeneous coordinate by square matrix
    /// Pre:    4x4 matrix of floats must be passed in as a pointer
    /// Post:   'out' contains the multiplication of 'in' and 't' 
    /// Param:  t   - matrix used in multiplication
    ///         in  - coordinate used in multiplication
    ///         out - resulting coordinate from multiplication
    ///////////////////////////////////////////////////////////////////////
	static void MultiplyPoint(const float t[16], const float in[4], float out[4]);
	
    
    ///////////////////////////////////////////////////////////////////////
    /// Func:   void PVC_Matrix::MultiplyPoint( const float in[4], float out[4] )
    /// Brief:  Calls PVC_Matrix::MultiplyPoint( const float[16], const float[4],
    ///         float[4]) to multiply a homogeneous coordinate by Element
    /// Pre:    None
    /// Post:   'out' contains the multiplication of 'in' and Element 
    /// Param:  in  - coordinate used in multiplication
    ///         out - resulting coordinate from multiplication
    ///////////////////////////////////////////////////////////////////////
	void MultiplyPoint(const float in[4], float out[4])
	  {PVC_Matrix::MultiplyPoint(*this->Element,in,out); }
	
    ///////////////////////////////////////////////////////////////////////
    /// Func:   float PVC_Matrix::GetElement( int x, int y )
    /// Brief:  Returns the value of Element[x][y]
    /// Pre:    'x' and 'y' must be in the range [0,3]
    /// Post:   None
    /// Param:  x - column index
    ///         y - row index
    /// Return: Value contained in Element[x][y]
    ///////////////////////////////////////////////////////////////////////
	float GetElement(int x, int y) { return this->Element[x][y]; };
	
private:
	
};

#include "PVC_Matrix.cxx"

#endif // PVC_MATRIX_H