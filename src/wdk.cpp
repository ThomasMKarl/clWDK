#include "wdk.h"



namespace WDK {

void Picture::save(const std::string filename)
{
  std::ofstream FILE{};
  FILE.open(filename);

  float max = 0.0f;
  for(auto value : imageData)
  {
    if(value > max) max = value;
  }
  
  FILE << "P3\n" << numberOfPixelsXdirection << " " << numberOfPixelsYdirection << "\n255";
  for(size_t i = 0; i < imageData.size(); ++i)
  {
    if(i%numberOfPixelsXdirection == 0) FILE << "\n";
    FILE << (short uint)(255*float(imageData[i]/max)) << " 0 0 ";
  }
}

//////////////////////////////////////////////////////////////////////////////

void OclSetup::oclProgramTest() const
{
#ifdef DEBUG_OCL
  cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device);
  if(status != CL_BUILD_ERROR) return;
  
  std::string message = program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(device);
  if(message.empty()) message = "-none-";
  std::cerr << "build options > " << message << "\n";
    
  message = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
  if(message.empty()) message = "-none-";
  std::cerr << "build log > " << message;

  exit(EXIT_FAILURE);
#endif
}

void OclSetup::oclPrepareQueue(const unsigned short int platformNumber, const unsigned short int deviceNumber)
{
  std::vector<cl::Platform> platformList{};
  OCL_CALL_RET(cl::Platform::get(&platformList));
  platform = platformList[platformNumber];

  std::vector<cl::Device> deviceList{};
  OCL_CALL_RET(platform.getDevices(CL_DEVICE_TYPE_GPU, &deviceList));
  device = deviceList[deviceNumber];

  context = cl::Context(device, NULL, NULL, NULL, &err); OCL_CALL_RET(err);

  queue = cl::CommandQueue(context, CL_QUEUE_PROFILING_ENABLE, &err); OCL_CALL_RET(err);
}

void OclSetup::oclBuildProgram(const std::string spirvFilename, const std::string includePath)
{  
  std::ifstream inputStream(spirvFilename.c_str());
  
  std::string spirString((std::istreambuf_iterator<char>(inputStream)),
                         (std::istreambuf_iterator<char>()));
  unsigned long long int binarySize = inputStream.tellg();

  //cl_program prog = clCreateProgramWithIL(context.get(),(void*)(spirString.data()), binarySize, &err); OCL_CALL_RET(err);
  //cl_device_id device_list[] = {device.get()};
  //err = clBuildProgram(program.get(), 1, device_list, includePath.c_str(), NULL, NULL);
  
  cl::Program::Binaries binary{{(unsigned char)*(spirString.data()),(unsigned char)(binarySize)}};
  
  std::vector<cl_int> binaryStatus{};
  program = cl::Program(context, {device}, binary, &binaryStatus, &err); OCL_CALL_RET(binaryStatus[0]); OCL_CALL_RET(err);

  err = program.build({device}, includePath.c_str());
  if(err != CL_SUCCESS) oclProgramTest();
}

void OclSetup::createKernelSingle()
{
  kernel = cl::Kernel(program, "computeZeroPointPicture_single", &err); OCL_CALL_RET(err);
}

void OclSetup::createKernelDouble()
{
  kernel = cl::Kernel(program, "computeZeroPointPicture_double", &err); OCL_CALL_RET(err);
}

void OclSetup::createKernelHalf()
{
  kernel = cl::Kernel(program, "computeZeroPointPicture_half",   &err); OCL_CALL_RET(err);
}

//////////////////////////////////////////////////////////////////////////////

std::string getSpirBinaryPath(unsigned short int degree = 32)
{
  switch(degree)
  {
   case   8:
    return "bin/spirv/weierstrass8.spv";
   case  16:
    return "bin/spirv/weierstrass16.spv";
   case  32:
    return "bin/spirv/weierstrass32.spv";
   case  64:
    return "bin/spirv/weierstrass64.spv";
   case 128:
    return "bin/spirv/weierstrass128.spv";
   case 256:
    return "bin/spirv/weierstrass256.spv";
   default:
    std::cerr << "Degree must be 8, 16, 32, 64, 128 or 256.\n";
    return "";
  }
}

std::string getClrngIncludePath()
{
  std::string clrngRoot{};
  clrngRoot.append( std::getenv("CLRNG_ROOT") );
  if(clrngRoot.empty())
    std::cerr << "Specify environment variable CLRNG_ROOT (path to clRNG include directory).\n";

  std::string includeString{"-I "};
  includeString.append(clrngRoot);
  includeString.append("/include");

  return includeString;
}

};//namespace WDK
