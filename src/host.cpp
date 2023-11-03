#include "wdk.h"


int main(int argc, char* argv[])
{
  if(argc != 6)
  {
    std::cout << "Usage: " << argv[0] << " <number of polynomes> <degree> <lower restriction for coefficients> <upper restriction for coefficients> < break condition(if(error < eps) break;)>\n";
    exit(EXIT_FAILURE);
  }
  
  size_t numberOfPolynomials = atoi(argv[1]);
  unsigned short int degree = atoi(argv[2]);
  WDK::limits<short int> limit{};
  unsigned short int radius = limit.upper - limit.lower;
  limit.lower = atoi(argv[3]);
  limit.upper = atoi(argv[4]);
  if(limit.lower > limit.upper)
  {
    std::cerr << "ERROR: lower restriction must be smaller than upper restriction!\n";
    exit(EXIT_FAILURE);
  }
  float tolerance = atof(argv[5]);

  bool host = true;
  WDK::WdkSetup<float> wdkSetup(degree, numberOfPolynomials, tolerance, limit, host);

  /////////////////////////////////////////////////////////////////////////////////////
  
  auto start = std::chrono::system_clock::now();

  unsigned long long int seed = time(NULL);

  WDK::InitializationPlusMinusOne<float> init(seed);
  
  #pragma omp parallel for
  for(size_t i = 0; i < wdkSetup.numberOfPolynomials; ++i)
  {   
    WDK::initPolynomial(wdkSetup, radius, init);
    WDK::weierstrassIteration(wdkSetup);
  }
  
  auto end = std::chrono::system_clock::now();

  /////////////////////////////////////////////////////////////////////////////////////
  
  std::chrono::duration<float> elapsed = end - start;
  //std::cout << "#computation took " << elapsed.count() << " seconds.\n";
  std::cout << elapsed.count() << "\n";
  
  return EXIT_SUCCESS;
}
