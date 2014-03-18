///////////////////////////////////////////////////////////////////////////
/// File:   PVC_Math.h
/// Author: Travis Bueter - IT Research Support Services
/// Brief:  This header file declares the Math class for the PVController 
///         program.
///////////////////////////////////////////////////////////////////////////

#ifndef PVC_MATH_H
#define PVC_MATH_H

#include <cmath>

///////////////////////////////////////////////////////////////////////////
/// Class:  PVC_Math
/// Brief:  This class contains the functions necessary for performing the 
///         linear transformations for 3x3 matrices. Used when calculating
///         the new camera state. All functions are static and do not
///         require an instance of this class.
///////////////////////////////////////////////////////////////////////////

class PVC_Math
{
public:

    ///////////////////////////////////////////////////////////////////////
    /// Func:   static float PVC_Math::Norm( float[3] )
    /// Brief:  Calculates the norm, or length, of a vector in
    ///         3-dimensional space. 
    /// Pre:    Paramerter must be an array of three floats
    /// Post:   Norm of vector is caluclated and returned
    /// Param:  x - Vector from which the normal will be calculated
    /// Return: Norm of vector is returned as a float
    ///////////////////////////////////////////////////////////////////////
    
    static float Norm(const float x[3])
    {
	  return static_cast<float> (sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] ));
    };
	
    ///////////////////////////////////////////////////////////////////////
    /// Func:   static float PVC_Math::Normalize( float[3] )
    /// Brief:  Calculates the norm, or length, of a vector in
    ///         3-dimensional space
    /// Pre:    Paramerter must be an array of three floats
    /// Post:   Norm of vector is caluclated and returned
    /// Param:  x - Vector from which the normal will be calculated
    /// Return: Norm of vector is returned as a float
    ///////////////////////////////////////////////////////////////////////
    
    static float Normalize(float x[3]);
    
    ///////////////////////////////////////////////////////////////////////
    /// Func:   static void PVC_Math::Cross( const float[3], 
    ///                                     const float[3], 
    ///                                     float[3] )
    /// Brief:  Calculates the cross product of two 3-dimensional vectors 
    /// Pre:    Parameters must be arrays of three floats. Parameters x & y
    ///         must have valid values
    /// Post:   Cross product is caluclated and stored in z
    /// Param:  x - Vector used to calculate cross product
    ///         y - Vector used to calculate cross product
    ///         z - Holds result of cross product
    ///////////////////////////////////////////////////////////////////////    
    
    static void Cross(const float x[3], const float y[3], float z[3]);
		
protected:
    // Default Constructor and Deconstructor. No instance of class should
    // ever be declared.
    PVC_Math() {};
    ~PVC_Math() {};
};

//----------------------------------------------------------------------------
inline float PVC_Math::Normalize(float x[3])
{
  float den;
  if ( ( den = PVC_Math::Norm( x ) ) != 0.0 )
    {
    for (int i=0; i < 3; i++)
      {
      x[i] /= den;
      }
    }
  return den;
}

//----------------------------------------------------------------------------
inline void PVC_Math::Cross(const float x[3], const float y[3], float z[3])
{
  float Zx = x[1] * y[2] - x[2] * y[1];
  float Zy = x[2] * y[0] - x[0] * y[2];
  float Zz = x[0] * y[1] - x[1] * y[0];
  z[0] = Zx; z[1] = Zy; z[2] = Zz;
}

#endif // PVC_MATH_H