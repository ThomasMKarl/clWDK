#include "wdk.h"



int main(int argc, char* argv[])
{
  if(argc < 6)
  {
    printf("Usage: ws_ocl <number of polynomials> <degree> <lower restriction for coefficients> <upper restriction for coefficients> <break condition(if(error < eps) break;)>\n");
    exit(EXIT_FAILURE);
  }
  
  cl_ulong numberOfPolynomials = atoi(argv[1]);
  cl_ushort degree = atoi(argv[2]);
  WDK::limits<short int> limit{};
  limit.lower = atof(argv[3]); 
  limit.upper = atof(argv[4]);
  if(limit.lower > limit.upper)
  {
    printf("ERROR: lower restriction must be smaller than upper restriction!\n");
    exit(EXIT_FAILURE);
  }
  cl_float tolerance = atof(argv[5]);
  
  WDK::Picture picture{1080,1080};

  WDK::WdkSetup<float> wdkSetup(degree, numberOfPolynomials, tolerance, limit);
  
  //////////////////////////////////////////////////////////////////////////////////////
  
    auto start = std::chrono::system_clock::now();

  WDK::OclSetup oclSetup{};
  oclSetup.oclPrepareQueue();
  std::string spirBinaryPath = WDK::getSpirBinaryPath(degree);
  oclSetup.oclBuildProgram(spirBinaryPath, WDK::getClrngIncludePath());
  oclSetup.createKernelSingle();
  
  WDK::computePictureData(oclSetup, picture, wdkSetup);

    auto end = std::chrono::system_clock::now();
    
  //////////////////////////////////////////////////////////////////////////////////////
    
  std::chrono::duration<float> elapsed = end - start;
  //std::cout << "#computation took " << elapsed.count() << " seconds.\n";
  std::cout << elapsed.count() << "\n";
  
  picture.save("zero_ocl.ppm");
  
  return EXIT_SUCCESS;
}
