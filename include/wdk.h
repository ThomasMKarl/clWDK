#ifndef WDK_H
#define WDK_H

#include "ocl.h"

namespace WDK {

//! coefficients for the polynomial are generated (real and imagainary) between the upper and lower bound.
template<typename T>
struct limits
{
 public:
  T upper{1};
  T lower{-1};
  limits() = default;
};

//! callable that sets coefficients to 1 or -1
template<typename T>
class InitializationPlusMinusOne
{
 private:
  std::default_random_engine generator{};
  std::uniform_int_distribution<short int> distribution{0,1};
  
 public:
  InitializationPlusMinusOne(const size_t seed) : generator{seed} {};
  std::complex<T> operator () () {return std::complex<T>(distribution(generator)*2 - 1, 0);}
};

//! a handler for simulation setup data
template <typename T, typename L = short int>
class WdkSetup
{
 public:
  cl_ushort degree{};
  cl_ulong numberOfPolynomials{};
  cl_float tolerance{}; /**< epsilon criterion as break condition */
  limits<L> limit{};
  std::vector<std::complex<T>> hostPolynomial{}; /**< contains coefficients for a polynomial on the host from second highest to lowest degree, the highest is expected to be one and not explicitely stored */
  std::vector<std::complex<T>> hostZeroPoints{}; /**< contains zero Points for a polynomial on the host with no specific order */
  
  /*****************************************************************//**
  * \brief constructs a handler                                        
  *                                                                    
  * If host is set, the computation is expected to happen on    
  * the host and the polynomial is resized to degreeSet         
  *********************************************************************/
  WdkSetup(cl_ushort degreeSet, cl_ulong numberOfPolynomialsSet, cl_float toleranceSet, limits<L> limitSet, bool host = false) 
  : degree{degreeSet}, numberOfPolynomials{numberOfPolynomialsSet}, tolerance{toleranceSet}
  {
    limit.lower = limitSet.lower;
    limit.upper = limitSet.upper;
    if(host)
    {
      hostPolynomial.resize(degree);
      hostZeroPoints.resize(degree);
    }
  }
};

/*****************************************************************//**
* \brief a handler for OCL specific objects                          
*********************************************************************/
class OclSetup
{
 public:  
  cl::Platform platform{};
  cl::Device device{};

  cl::Context context{};

  cl::CommandQueue queue{};

  cl::Program program{};
  cl::Kernel kernel{};

  OclSetup() = default;
  
  /*****************************************************************//**
  * \brief tests a program for compilation errors and prints result    
  *********************************************************************/
  void oclProgramTest() const;
  void oclPrepareQueue(
    const unsigned short int platformNumber = 0,
    const unsigned short int deviceNumber   = 0);
  void oclBuildProgram(
    const std::string spirvFilename, 
    const std::string includePath);
  void createKernelSingle();
  void createKernelDouble();
  void createKernelHalf();
  void setError(const cl_int errorSet) {err = errorSet;};
  cl_int getLastError() const {return err;};

 private:
  cl_int err{}; /**< stores last OCL error */
  
};

class Picture
{
 public:
  cl_uint numberOfPixelsXdirection{0};
  cl_uint numberOfPixelsYdirection{0};
  std::vector<cl_uint> imageData{};
  
  Picture() = default;
  Picture(
    unsigned int numberOfPixelsXdirectionSet
   ,unsigned int numberOfPixelsYdirectionSet)
   :numberOfPixelsXdirection{numberOfPixelsXdirectionSet}
   ,numberOfPixelsYdirection{numberOfPixelsYdirectionSet}
   {imageData.resize( numberOfPixelsXdirectionSet*numberOfPixelsYdirectionSet );}

  void save(const std::string filename);
};

/*****************************************************************//**
* \brief fills the data in picture                            
*                                                                    
* Creates stream of random numbers, allocates buffers, enqueues      
* kernel and retrieves image data. The calls are non-blocking.       
* A synchronisation operation on the queue might be needed after     
* the successfull execution of the function.                         
*********************************************************************/
template<typename T>
void computePictureData(OclSetup &oclSetup
		       ,Picture &picture
		       ,const WdkSetup<T> &wdkSetup)
{
  const unsigned int numberOfPixels = picture.numberOfPixelsYdirection * picture.numberOfPixelsYdirection;
  
  const cl_ulong numberOfPolynomials = wdkSetup.numberOfPolynomials;
  const cl_ushort degree = wdkSetup.degree;
  const cl_float tolerance = wdkSetup.tolerance;

  cl_int err{};
  
  clrngMrg31k3pStream *streams{};
  size_t streamBufferSize{};
  streams = clrngMrg31k3pCreateStreams(
    NULL, 
    numberOfPolynomials*degree,
    &streamBufferSize, 
    (clrngStatus*)&err); CLRNG_CALL_RET(err);

  
  cl::Buffer streams_buffer = cl::Buffer(
    oclSetup.context, 
    CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,
    streamBufferSize, 
    streams, 
    &err); oclSetup.setError(err); OCL_CALL_RET(err);
  cl::Buffer image_buffer   = cl::Buffer(
    oclSetup.context, 
    CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
    numberOfPixels*sizeof(cl_uint),
    picture.imageData.data(), 
    &err); oclSetup.setError(err); OCL_CALL_RET(err);

  size_t narg = 0;
  oclSetup.kernel.setArg(narg++, streams_buffer);
  oclSetup.kernel.setArg(narg++, numberOfPolynomials);
  oclSetup.kernel.setArg(narg++, wdkSetup.limit.lower);
  oclSetup.kernel.setArg(narg++, wdkSetup.limit.upper);
  oclSetup.kernel.setArg(narg++, tolerance);
  oclSetup.kernel.setArg(narg++, picture.numberOfPixelsXdirection);
  oclSetup.kernel.setArg(narg++, picture.numberOfPixelsXdirection);
  oclSetup.kernel.setArg(narg++, image_buffer);

  const unsigned short int workgroupSize = 256;
  const size_t numberOfWorkgroups = std::ceil( (degree*numberOfPolynomials)/workgroupSize );
  const size_t globalSize = numberOfWorkgroups * workgroupSize;
  err = oclSetup.queue.enqueueNDRangeKernel(
    oclSetup.kernel, 
    cl::NullRange, 
    cl::NDRange(globalSize), 
    cl::NDRange(workgroupSize)); oclSetup.setError(err); OCL_CALL_RET(err);
  
  err = oclSetup.queue.enqueueMigrateMemObjects({image_buffer}, CL_MIGRATE_MEM_OBJECT_HOST); oclSetup.setError(err); OCL_CALL_RET(err);
}

//////////////////////////////////////////////////////////////////////////////

/*****************************************************************//**
* \brief returns the path of the spirv binary that is compiled for   
* degree                                                      
*                                                                    
* returns an empty string if the file is not found or inaccessible   
* or if degree is not 8, 16, 32, 64, 128 or 256               
*********************************************************************/
std::string getSpirBinaryPath(const unsigned short int degree);
std::string getClrngIncludePath();

//////////////////////////////////////////////////////////////////////////////

template<typename T>
std::complex<T> calculatePolynomial(const WdkSetup<T> &wdkSetup, const std::complex<T> input)
{
  std::complex<T> result(1.0f,0.0f);
  
  for(unsigned short int index = wdkSetup.degree-1; index > 0; --index)
  {
    result *= input;
    result += wdkSetup.hostPolynomial[index];
  }

  return result;
}

/*****************************************************************//**
* \brief calculates the product of the zero points execept for the   
* one with index \param exceptFor                                    
*                                                                    
* calculates \f$\prod_{i=1,i\neq j}^{d}(x-\zeta_j)\f$ for                
* \f$x=\f$ input, \f$j=\f$ exceptFor                             
* zero point \f$\zeta\f$ and degree \f$d\f$                                   
*********************************************************************/
template<typename T>
std::complex<T> calculateProductOfRemainingZeroPoints(const WdkSetup<T> &wdkSetup
						     ,const unsigned short int exceptFor
						     ,const std::complex<T> input)
{
  std::complex<T> result(1.0f,0.0f);
  
  for(unsigned short int index = 0; index < wdkSetup.degree; ++index)
  {
    if(index != exceptFor) result *= input - wdkSetup.hostZeroPoints[index];
  }

  return result;
}

/*************************************************************************//**
* \brief performs a Weierstrass iteration to estimate all zero Points        
*                                                                            
* For each zero point with index $k$ the fixed point iteration               
* \f$x_k^{i+1} = x_k^{i+1} +                                                   
* \frac{\textrm{polynomial}(x_k^i)}{prod_{i=1,i\neq j}^{d}(x_k^i-\zeta_j)}   
* \rightarrow x_k\f$ is calculated until the number of steps exceeded           
* \f$10\times\f$ degree or                                                         
* \f$\textrm{max}\left\{ \frac{|\textrm{polynomial}(x_k^i)|}{|x_k^i|} \right\} 
* \leq \varepsilon\f$ holds.                                                   
*****************************************************************************/
template<typename T>
void weierstrassIteration(WdkSetup<T> &wdkSetup)
{
  const size_t maxIterations = 10*wdkSetup.degree;

  std::complex<T> zeroProduct{};
  std::complex<T> polynomialValue{};
  
  float error{};
  float maxError{wdkSetup.tolerance};
  size_t iteration{0};
  while(maxError >= wdkSetup.tolerance && (iteration < maxIterations))
  {
    maxError = 0;
    for(unsigned short int index = 0; index < wdkSetup.degree; ++index)
    {
      zeroProduct = calculateProductOfRemainingZeroPoints(wdkSetup, index, wdkSetup.hostZeroPoints[index]);
      polynomialValue = calculatePolynomial(wdkSetup, zeroProduct);
      
      error = std::abs(polynomialValue) / std::abs(wdkSetup.hostZeroPoints[index]);
      if(error > maxError) maxError = error;

      wdkSetup.hostZeroPoints[index] -= polynomialValue/zeroProduct;
    }

    ++iteration;
  }
}

/*****************************************************************//**
* \brief initializes the coefficients uniformly between associated   *
* limits and as start values the zero points are equally distributed *
* on a circle in the complex plane with a radius of half the         *
* distance of the limits                                             *
*********************************************************************/
template<typename T>
void initPolynomial(WdkSetup<T> &wdkSetup, const size_t seed)
{  
  std::default_random_engine generator{seed};
  std::uniform_real_distribution<T> distribution(wdkSetup.limit.lower,wdkSetup.limit.upper);

  const unsigned short int degree = wdkSetup.degree;
  for(unsigned short int index = 0; index < degree; ++index)
  {
    wdkSetup.zeroPoints[index] = std::complex<T>((wdkSetup.limit.upper - wdkSetup.limit.lower)/2.0f*cos(index*2.0f*3.1415f/T(degree)),
    			                         (wdkSetup.limit.upper - wdkSetup.limit.lower)/2.0f*sin(index*2.0f*3.1415f/T(degree)));
    wdkSetup.polynomial[index] = std::complex<T>(distribution(generator),distribution(generator));
  }
}

/*****************************************************************//**
* \brief initializes the coefficients with an arbitrary callable     
* and as start values the zero points are equally distributed        
* on a circle in the complex plane with radius                
*********************************************************************/
template<typename T, typename I>
void initPolynomial(WdkSetup<T> &wdkSetup
                   ,const float radius
                   ,I &initialization)
{
  std::generate(wdkSetup.hostPolynomial.begin(), wdkSetup.hostPolynomial.end(), initialization());
  
  for(unsigned short int index = 0; index < wdkSetup.degree; ++index)
  {
    wdkSetup.hostZeroPoints[index] = std::complex<T>(radius * cos(index*2.0f*3.1415f/T(wdkSetup.degree)),
			                             radius * sin(index*2.0f*3.1415f/T(wdkSetup.degree)));
  }
}

}; //namespace WDK
#endif
