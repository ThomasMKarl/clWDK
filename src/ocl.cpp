#include "ocl.h"



namespace cl {

std::string oclGenErrorString(const cl_int error)
{
  cl_int index = -error;
  if(index == 9999) index = 95;
  if(index >= 1000) index -= 929;
  if(index > 19 && index < 30) return "--error code reserved--";
  if(index > 95) return "--unknown error code--";

  std::string errorString = "Error '";
  errorString.append(OpenClErrorFlag[index]);
  errorString.append("' occurs\n");
  errorString.append(OpenClErrorDescription[index]);
  errorString.append("\n possible functions: ");
  errorString.append(OpenClErrorFunctionCause[index]);

  return errorString;
}

std::string clrngGenErrorString(const cl_int error)
{
  switch(error)
  {
   case CLRNG_SUCCESS:
    return oclGenErrorString(CL_SUCCESS);
   case CLRNG_OUT_OF_RESOURCES:
    return oclGenErrorString(CL_OUT_OF_RESOURCES);
   case CLRNG_INVALID_VALUE:
    return oclGenErrorString(CL_INVALID_VALUE);
   case CLRNG_INVALID_RNG_TYPE:
    return "CLRNG_INVALID_RNG_TYPE";
   case CLRNG_INVALID_STREAM_CREATOR:
    return "CLRNG_INVALID_STREAM_CREATOR";
   case CLRNG_INVALID_SEED:
    return "CLRNG_INVALID_SEED";
   case CLRNG_FUNCTION_NOT_IMPLEMENTED:
    return "CLRNG_FUNCTION_NOT_IMPLEMENTED";
   default:
    return "--unknown error code--";
  }
}

};//namespace cl
