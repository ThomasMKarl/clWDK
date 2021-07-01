#include "ocl/polynomial.clh"



class Picture
{
 private:
  unsigned int numberOfPixelsXdirection{0};
  unsigned int numberOfPixelsYdirection{0};
  __global unsigned int *data = nullptr; /**< a pointer the image buffer in global memory */

 public:
  Picture() = default;
  Picture(unsigned int numberOfPixelsXdirectionSet
	 ,unsigned int numberOfPixelsYdirectionSet
	 ,__global unsigned int *dataSet)
    :numberOfPixelsXdirection{numberOfPixelsXdirectionSet}
    ,numberOfPixelsYdirection{numberOfPixelsYdirectionSet}
    ,data{dataSet} {}
  
  /*****************************************************************//**
  * \brief converts zero points to pixels and updates image data
  *
  * The zero points are assumed to lie on a circle with the half
  * distance given by limit and a center displaced form the
  * origin by the lower limit plus the radius from the origin.
  *********************************************************************/
  template<typename T, unsigned long long int degree = 32, typename L = short int>
  void update(__local Polynomial<T,degree> &polynomial, limits<L> &limit)
  {
    const unsigned short int localIndex = get_local_id(0);

    const complex<T> zeroPoint = polynomial.getZeroPoint(localIndex);
    
    const L difference = limit.upper - limit.lower;
    const T radius = 0.5f * difference;
    const complex<T> center(limit.lower+radius,limit.lower+radius);
    
    const T positionXtransformed = zeroPoint.getRealPart()      + center.getRealPart()      + radius;
    const int positionXscaled = positionXtransformed/(2.0f*radius) *numberOfPixelsXdirection;

    const T positionYtransformed = zeroPoint.getImaginaryPart() + center.getImaginaryPart() + radius;
    const int positionYscaled = positionYtransformed/(2.0f*radius) *numberOfPixelsYdirection;
    
    const int position = positionXscaled*(numberOfPixelsXdirection) + positionYscaled;

    if(  positionXscaled < numberOfPixelsXdirection && positionXscaled >= 0
      && positionYscaled < numberOfPixelsYdirection && positionYscaled >= 0)
    {
      ++data[(unsigned int)(position)];
    }
  }
};


/*****************************************************************//**
* \brief updates image data                                          
*                                                                    
* Initializes polynomials, computes zero points and updates pixels.  
*********************************************************************/
template<typename T, unsigned long long int degree = 32>
void updatePicture(__global clrngMrg31k3pHostStream *streams
		  ,limits<short int> limit
		  ,const float tolerance
		  ,Picture &rgbPicture
		  ,__local Polynomial<T,degree> &polynomial)
{
  polynomial = {};
  
  const float radius = 1.0f;
  polynomial.init(InititalizationPlusMinusOne<T>{}, radius, streams);
    
  polynomial.weierstrassIteration(tolerance);
  
  rgbPicture.update(polynomial, limit);
}

#ifndef DEGREE
  #define DEGREE 32
#endif

/*****************************************************************//**
* \brief single precision                                            
*********************************************************************/
__kernel
void computeZeroPointPicture_single(__global clrngMrg31k3pHostStream *streams
			           ,const unsigned int numberOfPolynomials
			           ,const short int upperLimit
			           ,const short int lowerLimit
			           ,const float tolerance
			           ,const unsigned int numberOfPixelsX
			           ,const unsigned int numberOfPixelsY
			           ,__global unsigned int *rgbPictureData)
{
  const unsigned long long int globalIndex = get_global_id(0);

  __local Polynomial<float,DEGREE> polynomial;
  
  if(globalIndex < numberOfPolynomials) //might leave cores inactive
  {
    limits<short int> limit{};
    limit.upper = upperLimit;
    limit.lower = lowerLimit;

    Picture rgbPicture{numberOfPixelsX, numberOfPixelsY, rgbPictureData};
    updatePicture<float,DEGREE>(streams, limit, tolerance, rgbPicture, polynomial); 
  }

  polynomial.~Polynomial();
}

/*****************************************************************//**
* \brief double precision                                            
*********************************************************************/
__kernel
void computeZeroPointPicture_double(__global clrngMrg31k3pHostStream *streams
			           ,const unsigned int numberOfPolynomials
			           ,const short int upperLimit
			           ,const short int lowerLimit
			           ,const float tolerance
			           ,const unsigned int numberOfPixelsX
			           ,const unsigned int numberOfPixelsY
			           ,__global unsigned int *rgbPictureData)
{  
  const unsigned long long int globalIndex = get_global_id(0);

  __local Polynomial<double,DEGREE> polynomial;
  
  if(globalIndex < numberOfPolynomials)
  {
    limits<short int> limit{};
    limit.upper = upperLimit;
    limit.lower = lowerLimit;

    Picture rgbPicture{numberOfPixelsX, numberOfPixelsY, rgbPictureData};
    updatePicture<double,DEGREE>(streams, limit, tolerance, rgbPicture, polynomial); 
  }

  polynomial.~Polynomial();
}

/*****************************************************************//**
* \brief half precision                                              
*********************************************************************/
__kernel
void computeZeroPointPicture_half(__global clrngMrg31k3pHostStream *streams
			         ,const unsigned int numberOfPolynomials
			         ,const short int upperLimit
			         ,const short int lowerLimit
			         ,const float tolerance
			         ,const unsigned int numberOfPixelsX
			         ,const unsigned int numberOfPixelsY
			         ,__global unsigned int *rgbPictureData)
{
  const unsigned long long int globalIndex = get_global_id(0);

  __local Polynomial<half,DEGREE> polynomial;
  
  if(globalIndex < numberOfPolynomials)
  {
    limits<short int> limit{};
    limit.upper = upperLimit;
    limit.lower = lowerLimit;

    Picture rgbPicture{numberOfPixelsX, numberOfPixelsY, rgbPictureData};
    updatePicture<half,DEGREE>(streams, limit, tolerance, rgbPicture, polynomial); 
  }
  
  polynomial.~Polynomial();
}
