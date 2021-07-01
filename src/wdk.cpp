#include "wdk.h"



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

#define CL_CONTEXT_ADAPTER_D3D9_KHR NULL
#define CL_CONTEXT_ADAPTER_D3D9EX_KHR NULL
#define CL_CONTEXT_ADAPTER_DXVA_KHR NULL
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

const char * oclGenErrorString(cl_int error)
{
  static char p [] = " --- OpenCL newer error than 2.0";
  int i;

  //see cl.h for a list of errors
  switch(error)
  {
    case CL_SUCCESS:
    return "CL_SUCCESS";
    
    case CL_DEVICE_NOT_FOUND:
    return "CL_DEVICE_NOT_FOUND";
    
    case CL_DEVICE_NOT_AVAILABLE:
    return "CL_DEVICE_NOT_AVAILABLE";
    
    case CL_COMPILER_NOT_AVAILABLE:
    return "CL_COMPILER_NOT_AVAILABLE";
    
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
    return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    
    case CL_OUT_OF_RESOURCES:
    return "CL_OUT_OF_RESOURCES";
    
    case CL_OUT_OF_HOST_MEMORY:
    return "CL_OUT_OF_HOST_MEMORY";
    
    case CL_PROFILING_INFO_NOT_AVAILABLE:
    return "CL_PROFILING_INFO_NOT_AVAILABLE";
    
    case CL_MEM_COPY_OVERLAP:
    return "CL_MEM_COPY_OVERLAP";
    
    case CL_IMAGE_FORMAT_MISMATCH:
    return "CL_IMAGE_FORMAT_MISMATCH";
    
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
    return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    
    case CL_BUILD_PROGRAM_FAILURE:
    return "CL_BUILD_PROGRAM_FAILURE";
    
    case CL_MAP_FAILURE:
    return "CL_MAP_FAILURE";
    
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:
    return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    
    case CL_COMPILE_PROGRAM_FAILURE:
    return "CL_COMPILE_PROGRAM_FAILURE";
    
    case CL_LINKER_NOT_AVAILABLE:
    return "CL_LINKER_NOT_AVAILABLE";
    
    case CL_LINK_PROGRAM_FAILURE:
    return "CL_LINK_PROGRAM_FAILURE";
    
    case CL_DEVICE_PARTITION_FAILED:
    return "CL_DEVICE_PARTITION_FAILED";
    
    case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
    return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
    
    case CL_INVALID_VALUE:
    return "CL_INVALID_VALUE";
    
    case CL_INVALID_DEVICE_TYPE: 
    return "CL_INVALID_DEVICE_TYPE";
    
    case CL_INVALID_PLATFORM: 
    return "CL_INVALID_PLATFORM";
    
    case CL_INVALID_DEVICE:
    return "CL_INVALID_DEVICE";
 
    case CL_INVALID_CONTEXT:
    return "CL_INVALID_CONTEXT";
    
    case CL_INVALID_QUEUE_PROPERTIES: 
    return "CL_INVALID_QUEUE_PROPERTIES";
    
    case CL_INVALID_COMMAND_QUEUE: 
    return "CL_INVALID_COMMAND_QUEUE";
    
    case CL_INVALID_HOST_PTR :  
    return "CL_INVALID_HOST_PTR";
    
    case CL_INVALID_MEM_OBJECT:
    return "CL_INVALID_MEM_OBJECT";
    
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    
    case CL_INVALID_IMAGE_SIZE:
    return "CL_INVALID_IMAGE_SIZE";
    
    case CL_INVALID_SAMPLER: 
    return "CL_INVALID_SAMPLER";
    
    case CL_INVALID_BINARY:
    return "CL_INVALID_BINARY";
    
    case CL_INVALID_BUILD_OPTIONS:
    return "CL_INVALID_BUILD_OPTIONS";
    
    case CL_INVALID_PROGRAM:
    return "CL_INVALID_PROGRAM";
    
    case CL_INVALID_PROGRAM_EXECUTABLE:  
    return "CL_INVALID_PROGRAM_EXECUTABLE";
    
    case CL_INVALID_KERNEL_NAME:  
    return "CL_INVALID_KERNEL_NAME";
    
    case CL_INVALID_KERNEL_DEFINITION:
    return "CL_INVALID_KERNEL_DEFINITION";
    
    case CL_INVALID_KERNEL:
    return "CL_INVALID_KERNEL";
    
    case CL_INVALID_ARG_INDEX:
    return "CL_INVALID_ARG_INDEX ";
    
    case CL_INVALID_ARG_VALUE:
    return "CL_INVALID_ARG_VALUE ";
    
    case CL_INVALID_ARG_SIZE:
    return "CL_INVALID_ARG_SIZE ";
    
    case CL_INVALID_KERNEL_ARGS:
    return "CL_INVALID_KERNEL_ARGS ";
    
    case CL_INVALID_WORK_DIMENSION:  
    return "CL_INVALID_WORK_DIMENSION";
    
    case CL_INVALID_WORK_GROUP_SIZE:
    return "CL_INVALID_WORK_GROUP_SIZE";
    
    case CL_INVALID_WORK_ITEM_SIZE:
    return "CL_INVALID_WORK_ITEM_SIZE ";
    
    case CL_INVALID_GLOBAL_OFFSET: 
    return "CL_INVALID_GLOBAL_OFFSET";
    
    case CL_INVALID_EVENT_WAIT_LIST: 
    return "CL_INVALID_EVENT_WAIT_LIST";
    
    case CL_INVALID_EVENT:   
    return "CL_INVALID_EVENT";
    
    case CL_INVALID_OPERATION:
    return "CL_INVALID_OPERATION";
    
    case CL_INVALID_GL_OBJECT:
    return "CL_INVALID_GL_OBJECT";
    
    case CL_INVALID_BUFFER_SIZE:
    return "CL_INVALID_BUFFER_SIZE";
    
    case CL_INVALID_MIP_LEVEL:
    return "CL_INVALID_MIP_LEVEL";
    
    case CL_INVALID_GLOBAL_WORK_SIZE:
    return "CL_INVALID_GLOBAL_WORK_SIZE";
    
    case CL_INVALID_PROPERTY:
    return "CL_INVALID_PROPERTY";
    
    case CL_INVALID_IMAGE_DESCRIPTOR:
    return "CL_INVALID_IMAGE_DESCRIPTOR";
    
    case CL_INVALID_COMPILER_OPTIONS:
    return "CL_INVALID_COMPILER_OPTIONS";
    
    case CL_INVALID_LINKER_OPTIONS:
    return "CL_INVALID_LINKER_OPTIONS";
    
    case CL_INVALID_DEVICE_PARTITION_COUNT: 
    return "CL_INVALID_DEVICE_PARTITION_COUNT";
    
    /*case CL_INVALID_PIPE_SIZE:   
    return "CL_INVALID_PIPE_SIZE";
    
    case CL_INVALID_DEVICE_QUEUE: 
    return "CL_INVALID_DEVICE_QUEUE";*/
    
    default:
    if (error < 0) { *p = '-'; error = -error; }
    i = 10;
    while (error != 0) { p [i] = (error % 10) + '0'; error /= 10; i--; }
    return p;   
  }
}
