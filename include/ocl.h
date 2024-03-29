#ifndef OCL_H
#define OCL_H

#include "std.h"

#ifdef XOCL
  #define CL_HPP_CL_1_2_DEFAULT_BUILD 1
  #define CL_TARGET_OPENCL_VERSION 120
  #define CL_HPP_TARGET_OPENCL_VERSION 120
  #define CL_HPP_MINIMUM_OPENCL_VERSION 120
  #define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
  #define CL_USE_DEPRECATED_OPENCL_1_2_APIS
  #include <CL/cl_ext_xilinx.h>
#endif
#ifndef XOCL
  #define CL_HPP_TARGET_OPENCL_VERSION 200
  #define CL_TARGET_OPENCL_VERSION 300
#endif

#include <CL/opencl.hpp>

#include <clRNG/clRNG.h>
#include <clRNG/mrg31k3p.h>

#ifdef DEBUG_OCL
#define   OCL_CALL_DIE(x) do { \
 cl_int error = x; if((error)!=CL_SUCCESS) { \
  std::cerr << " Error " << error << " at line " << __LINE__ << " in file " << __FILE__ << "\n" <<   cl::oclGenErrorString(error) << "\n"; \
  return EXIT_FAILURE; \
 } \
} while(0)

#define   OCL_CALL_RET(x) do { \
 cl_int error = x; if((error)!=CL_SUCCESS) { \
  std::cerr << " Error " << error << " at line " << __LINE__ << " in file " << __FILE__ << "\n" <<   cl::oclGenErrorString(error) << "\n"; \
  return; \
 } \
} while(0)

#define   OCL_CALL_OMP(x) do { \
 cl_int error = x; if((error)!=CL_SUCCESS) { \
  std::cerr << " Error " << error << " at line " << __LINE__ << " in file " << __FILE__ << "\n" <<   cl::oclGenErrorString(error) << "\n"; \
 } \
} while(0)

#define CLRNG_CALL_DIE(x) do { \
 cl_int error = x; if((error)!=CLRNG_SUCCESS) {	\
  std::cerr << " Error " << error << " at line " << __LINE__ << " in file " << __FILE__ << "\n" << cl::clrngGenErrorString(error) << "\n"; \
  return EXIT_FAILURE; \
 } \
} while(0)

#define CLRNG_CALL_RET(x) do { \
 cl_int error = x; if((error)!=CLRNG_SUCCESS) {	\
  std::cerr << " Error " << error << " at line " << __LINE__ << " in file " << __FILE__ << "\n" << cl::clrngGenErrorString(error) << "\n"; \
  return; \
 } \
} while(0)

#define CLRNG_CALL_OMP(x) do { \
 cl_int error = x; if((error)!=CLRNG_SUCCESS) {	\
  std::cerr << " Error " << error << " at line " << __LINE__ << " in file " << __FILE__ << "\n" << cl::clrngGenErrorString(error) << "\n"; \
 } \
} while(0)
#endif

#ifndef DEBUG_OCL
#define   OCL_CALL_DIE(x) do { x;} while(0)
 
#define   OCL_CALL_RET(x) do { x;} while(0)

#define   OCL_CALL_OMP(x) do { x;} while(0)

#define CLRNG_CALL_DIE(x) do { x;} while(0)
 
#define CLRNG_CALL_RET(x) do { x;} while(0)

#define CLRNG_CALL_OMP(x) do { x;} while(0)
#endif

namespace cl {

static const std::vector<std::string> OpenClErrorFlag = {
  "CL_SUCCESS",
  "CL_DEVICE_NOT_FOUND",
  "CL_DEVICE_NOT_AVAILABLE",
  "CL_COMPILER_NOT_AVAILABLE",
  "CL_MEM_OBJECT_ALLOCATION_FAILURE",
  "CL_OUT_OF_RESOURCES",
  "CL_OUT_OF_HOST_MEMORY",
  "CL_PROFILING_INFO_NOT_AVAILABLE",
  "CL_MEM_COPY_OVERLAP",
  "CL_IMAGE_FORMAT_MISMATCH",
  "CL_IMAGE_FORMAT_NOT_SUPPORTED",
  "CL_BUILD_PROGRAM_FAILURE",
  "CL_MAP_FAILURE",
  "CL_MISALIGNED_SUB_BUFFER_OFFSET",
  "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
  "CL_COMPILE_PROGRAM_FAILURE",
  "CL_LINKER_NOT_AVAILABLE",
  "CL_LINK_PROGRAM_FAILURE",
  "CL_DEVICE_PARTITION_FAILED",
  "CL_KERNEL_ARG_INFO_NOT_AVAILABLE",
  "","","","","","","","","","",
  
  "CL_INVALID_VALUE",
  "CL_INVALID_DEVICE_TYPE",
  "CL_INVALID_PLATFORM",
  "CL_INVALID_DEVICE",
  "CL_INVALID_CONTEXT",
  "CL_INVALID_QUEUE_PROPERTIES",
  "CL_INVALID_COMMAND_QUEUE",
  "CL_INVALID_HOST_PTR",
  "CL_INVALID_MEM_OBJECT",
  "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
  "CL_INVALID_IMAGE_SIZE",
  "CL_INVALID_SAMPLER",
  "CL_INVALID_BINARY",
  "CL_INVALID_BUILD_OPTIONS",
  "CL_INVALID_PROGRAM",
  "CL_INVALID_PROGRAM_EXECUTABLE",
  "CL_INVALID_KERNEL_NAME",
  "CL_INVALID_KERNEL_DEFINITION",
  "CL_INVALID_KERNEL",
  "CL_INVALID_ARG_INDEX",
  "CL_INVALID_ARG_VALUE",
  "CL_INVALID_ARG_SIZE",
  "CL_INVALID_KERNEL_ARGS",
  "CL_INVALID_WORK_DIMENSION",
  "CL_INVALID_WORK_GROUP_SIZE",
  "CL_INVALID_WORK_ITEM_SIZE",
  "CL_INVALID_GLOBAL_OFFSET",
  "CL_INVALID_EVENT_WAIT_LIST",
  "CL_INVALID_EVENT",
  "CL_INVALID_OPERATION",
  "CL_INVALID_GL_OBJECT",
  "CL_INVALID_BUFFER_SIZE",
  "CL_INVALID_MIP_LEVEL",
  "CL_INVALID_GLOBAL_WORK_SIZE",
  "CL_INVALID_PROPERTY",
  "CL_INVALID_IMAGE_DESCRIPTOR",
  "CL_INVALID_COMPILER_OPTIONS",
  "CL_INVALID_LINKER_OPTIONS",
  "CL_INVALID_DEVICE_PARTITION_COUNT",
  "CL_INVALID_PIPE_SIZE",
  "CL_INVALID_DEVICE_QUEUE",

  "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR",
  "CL_PLATFORM_NOT_FOUND_KHR",
  "CL_INVALID_D3D10_DEVICE_KHR",
  "CL_INVALID_D3D10_RESOURCE_KHR",
  "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR",
  "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR",
  "CL_INVALID_D3D11_DEVICE_KHR",
  "CL_INVALID_D3D11_RESOURCE_KHR",
  "CL_D3D11_RESOURCE_ALREADY_ACQUIRED_KHR",
  "CL_D3D11_RESOURCE_NOT_ACQUIRED_KHR",
  "CL_INVALID_D3D9_DEVICE_NV or CL_INVALID_DX9_DEVICE_INTEL",
  "CL_INVALID_D3D9_RESOURCE_NV or CL_INVALID_DX9_RESOURCE_INTEL",
  "CL_D3D9_RESOURCE_ALREADY_ACQUIRED_NV or CL_DX9_RESOURCE_ALREADY_ACQUIRED_INTEL",
  "CL_D3D9_RESOURCE_NOT_ACQUIRED_NV or CL_DX9_RESOURCE_NOT_ACQUIRED_INTEL",
  
  "CL_EGL_RESOURCE_NOT_ACQUIRED_KHR",
  "CL_INVALID_EGL_OBJECT_KHR",
  "CL_INVALID_ACCELERATOR_INTEL",
  "CL_INVALID_ACCELERATOR_TYPE_INTEL",
  "CL_INVALID_ACCELERATOR_DESCRIPTOR_INTEL",
  "CL_ACCELERATOR_TYPE_NOT_SUPPORTED_INTEL",
  "CL_INVALID_VA_API_MEDIA_ADAPTER_INTEL",
  "CL_INVALID_VA_API_MEDIA_SURFACE_INTEL",
  "CL_VA_API_MEDIA_SURFACE_ALREADY_ACQUIRED_INTEL",
  "CL_VA_API_MEDIA_SURFACE_NOT_ACQUIRED_INTEL",

  "NVidia"
};
  
static const std::vector<std::string> OpenClErrorFunctionCause = {
  "",
  "clGetDeviceIDs",
  "clCreateContext",
  "clBuildProgram",
  "",
  "",
  "",
  "clGetEventProfilingInfo",
  "clEnqueueCopyBuffer, clEnqueueCopyBufferRect, clEnqueueCopyImage",
  "clEnqueueCopyImage",
  "clCreateImage",
  "clBuildProgram",
  "clEnqueueMapBuffer, clEnqueueMapImage",
  "",
  "",
  "clCompileProgram",
  "clLinkProgram",
  "clLinkProgram",
  "clCreateSubDevices",
  "clGetKernelArgInfo",
  "","","","","","","","","","",

  "clGetDeviceIDs, clCreateContext",
  "clGetDeviceIDs",
  "clGetDeviceIDs",
  "clCreateContext, clBuildProgram",
  "",
  "clCreateCommandQueue",
  "",
  "clCreateImage, clCreateBuffer",
  "",
  "",
  "",
  "clGetSamplerInfo, clReleaseSampler, clRetainSampler, clSetKernelArg",
  "clCreateProgramWithBinary, clBuildProgram",
  "clBuildProgram",
  "",
  "",
  "clCreateKernel",
  "clCreateKernel",
  "",
  "clSetKernelArg, clGetKernelArgInfo",
  "clSetKernelArg, clGetKernelArgInfo",
  "clSetKernelArg",
  "","","","","","","","","",
  "clCreateBuffer, clCreateSubBuffer",
  "OpenGL-functions",
  "",
  "clCreateContext",
  "clCreateImage",
  "clCompileProgram",
  "clLinkProgram",
  "clCreateSubDevices",
  "clCreatePipe",
  "clSetKernelArg",

  "clGetGLContextInfoKHR, clCreateContext",
  "clGetPlatform",
  "clCreateContext, clCreateContextFromType",
  "clCreateFromD3D10BufferKHR, clCreateFromD3D10Texture2DKHR, clCreateFromD3D10Texture3DKHR",
  "clEnqueueAcquireD3D10ObjectsKHR",
  "clEnqueueReleaseD3D10ObjectsKHR",
  "clCreateContext, clCreateContextFromType",
  "clCreateFromD3D11BufferKHR, clCreateFromD3D11Texture2DKHR, clCreateFromD3D11Texture3DKHR",
  "clEnqueueAcquireD3D11ObjectsKHR",
  "clEnqueueReleaseD3D11ObjectsKHR",
  "clCreateContext, clCreateContextFromType",
  "clCreateFromD3D9VertexBufferNV, clCreateFromD3D9IndexBufferNV, clCreateFromD3D9SurfaceNV, clCreateFromD3D9TextureNV, clCreateFromD3D9CubeTextureNV, clCreateFromD3D9VolumeTextureNV",
  "clEnqueueAcquireD3D9ObjectsNV",
  "clEnqueueReleaseD3D9ObjectsNV",
  "clEnqueueReleaseEGLObjectsKHR",
  "clCreateFromEGLImageKHR, clEnqueueAcquireEGLObjectsKHR",
  "clSetKernelArg",
  "clSetKernelArg, clCreateAccelerator",
  "clCreateAccelerator",
  "clCreateAccelerator",
  "clCreateContext, clCreateContextFromType",
  "clEnqueueReleaseVA_APIMediaSurfacesINTEL",
  "clEnqueueReleaseVA_APIMediaSurfacesINTEL",
  "clEnqueueReleaseVA_APIMediaSurfacesINTEL",

  "clEnqueueNDRangeKernel"
};

//! following: https://streamhpc.com/blog/2013-04-28/opencl-error-codes/
static const std::vector<std::string> OpenClErrorDescription = {
  "",
  "if no OpenCL devices that matched device_type were found.",
  "if a device in devices is currently not available even though the device was returned by clGetDeviceIDs.",
  "if program is created with clCreateProgramWithSource and a compiler is not available i.e. CL_DEVICE_COMPILER_AVAILABLE specified in the table of OpenCL Device Queries for clGetDeviceInfo is set to CL_FALSE.",
  "if there is a failure to allocate memory for buffer object.",
  "if there is a failure to allocate resources required by the OpenCL implementation on the device.",
  "if there is a failure to allocate resources required by the OpenCL implementation on the host.",
  "if the CL_QUEUE_PROFILING_ENABLE flag is not set for the command-queue, if the execution status of the command identified by event is not CL_COMPLETE or if event is a user event object.",
  "if src_buffer and dst_buffer are the same buffer or subbuffer object and the source and destination regions overlap or if src_buffer and dst_buffer are different sub-buffers of the same associated buffer object and they overlap. The regions overlap if src_offset ≤ to dst_offset ≤ to src_offset + size - 1, or if dst_offset ≤ to src_offset ≤ to dst_offset + size - 1.",
  "if src_image and dst_image do not use the same image format.",
  "if the image_format is not supported.",
  "if there is a failure to build the program executable. This error will be returned if clBuildProgram does not return until the build has completed.",
  "if there is a failure to map the requested region into the host address space. This error cannot occur for image objects created with CL_MEM_USE_HOST_PTR or CL_MEM_ALLOC_HOST_PTR.",
  "if a sub-buffer object is specified as the value for an argument that is a buffer object and the offset specified when the sub-buffer object is created is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for device associated with queue.",
  "if the execution status of any of the events in event_list is a negative integer value.",
  "if there is a failure to compile the program source. This error will be returned if clCompileProgram does not return until the compile has completed.",
  "if a linker is not available i.e. CL_DEVICE_LINKER_AVAILABLE specified in the table of allowed values for param_name for clGetDeviceInfo is set to CL_FALSE.",
  "if there is a failure to link the compiled binaries and/or libraries.",
  "if the partition name is supported by the implementation but in_device could not be further partitioned.",
  "if the argument information is not available for kernel.",
  "","","","","","","","","","",

  "if two or more coupled parameters had errors.",
  "if an invalid device_type is given.",
  "if an invalid platform was given.",
  "if devices contains an invalid device or are not associated with the specified platform.",
  "if context is not a valid context.",
  "if specified command-queue-properties are valid but are not supported by the device.",
  "if command_queue is not valid.",
  "This flag is valid only if host_ptr is not NULL. If specified, it indicates that the application wants the OpenCL implementation to allocate memory for the memory object and copy the data from memory referenced by host_ptr.CL_MEM_COPY_HOST_PTR and CL_MEM_USE_HOST_PTR are mutually exclusive.CL_MEM_COPY_HOST_PTR can be used with CL_MEM_ALLOC_HOST_PTR to initialize the contents of the cl_mem object allocated using host-accessible (e.g. PCIe) memory.",
  "if memobj is not a valid OpenCL memory object.",
  "if the OpenGL/DirectX texture internal format does not map to a supported OpenCL image format.",
  "if an image object is specified as an argument value and the image dimensions (image width, height, specified or compute row and/or slice pitch) are not supported by device associated with queue.",
  "if sampler is not a valid sampler object.",
  "if the provided binary is unfit for the selected device orif program is created with clCreateProgramWithBinary and devices listed in device_list do not have a valid program binary loaded.",
  "if the build options specified by options are invalid.",
  "if program object is not valid.",
  "if there is no successfully built program executable available for device associated with the command_queue.",
  "if kernel_name is not found in program.",
  "if the function definition for __kernel function given by kernel_name such as the number of arguments, the argument types are not the same for all devices for which the program executable has been built.",
  "if kernel is not a valid kernel object.",
  "if arg_index is not a valid argument index.",
  "if arg_value specified is not a valid value.",
  "if arg_size does not match the size of the data type for an argument that is not a memory object or if the argument is a memory object and arg_size != sizeof(cl_mem) or if arg_size is zero and the argument is declared with the __local qualifier or if the argument is a sampler and arg_size != sizeof(cl_sampler).",
  "if the kernel argument values have not been specified.",
  "if work_dim is not a valid value (i.e. a value between 1 and 3).",
  "if local_work_size is specified and number of work-items specified by global_work_size is not evenly divisable by size of work-group given by local_work_size or does not match the work-group size specified for kernel using the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier in program source.if local_work_size is specified and the total number of work-items in the work-group computed as local_work_size[0] *... local_work_size[work_dim - 1] is greater than the value specified by CL_DEVICE_MAX_WORK_GROUP_SIZE in the table of OpenCL Device Queries for clGetDeviceInfo.if local_work_size is NULL and the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier is used to declare the work-group size for kernel in the program source.",
  "if the number of work-items specified in any of local_work_size[0], ...local_work_size[work_dim - 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], ... . CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim - 1].",
  "if the value specified in global_work_size + the corresponding values in global_work_offset for any dimensions is greater than the sizeof(size_t) for the device on which the kernel execution will be enqueued.",
  "if event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.",
  "if event objects specified in event_list are not valid event objects.",
  "if interoperability is specified by setting CL_CONTEXT_ADAPTER_D3D9_KHR, CL_CONTEXT_ADAPTER_D3D9EX_KHR or CL_CONTEXT_ADAPTER_DXVA_KHR to a non-NULL value, and interoperability with another graphics API is also specified. (only if the cl_khr_dx9_media_sharing extension is supported).",
  "if texture is not a GL texture object whose type matches texture_target, if the specified miplevel of texture is not defined, or if the width or height of the specified miplevel is zero.",
  "if size is 0.Implementations may return CL_INVALID_BUFFER_SIZE if size is greater than the CL_DEVICE_MAX_MEM_ALLOC_SIZE value specified in the table of allowed values for param_name for clGetDeviceInfo for all devices in context.",
  "if miplevel is greater than zero and the OpenGL implementation does not support creating from non-zero mipmap levels.",
  "if global_work_size is NULL, or if any of the values specified in global_work_size[0], ...global_work_size [work_dim - 1] are 0 or exceed the range given by the sizeof(size_t) for the device on which the kernel execution will be enqueued.",
  "(Vague error, depends on the function)",
  "if values specified in image_desc are not valid or if image_desc is NULL.",
  "if the compiler options specified by options are invalid.",
  "if the linker options specified by options are invalid.",
  "if the partition name specified in properties is CL_DEVICE_PARTITION_BY_COUNTS and the number of sub-devices requested exceeds CL_DEVICE_PARTITION_MAX_SUB_DEVICES or the total number of compute units requested exceeds CL_DEVICE_PARTITION_MAX_COMPUTE_UNITS for in_device, or the number of compute units requested for one or more sub-devices is less than zero or the number of sub-devices requested exceeds CL_DEVICE_PARTITION_MAX_COMPUTE_UNITS for in_device.",
  "if pipe_packet_size is 0 or the pipe_packet_size exceeds CL_DEVICE_PIPE_MAX_PACKET_SIZE value for all devices in context or if pipe_max_packets is 0.",
  "if an argument is of type queue_t when it is not a valid device queue object.",

  "if CL and GL not on the same device (only when using a GPU).",
  "if no valid ICDs found.",
  "if the Direct3D 10 device specified for interoperability is not compatible with the devices against which the context is to be created.",
  "if the resource is not a Direct3D 10 buffer or texture object.",
  "if a mem_object is already acquired by OpenCL.",
  "if a mem_object is not acquired by OpenCL.",
  "if the Direct3D 11 device specified for interoperability is not compatible with the devices against which the context is to be created.",
  "if the resource is not a Direct3D 11 buffer or texture object.",
  "if a mem_object is already acquired by OpenCL.",
  "if a mem_object is not acquired by OpenCL.",
  "if the Direct3D 9 device specified for interoperability is not compatible with the devices against which the context is to be created.",
  "if a mem_object is not a Direct3D 9 resource of the required type.",
  "if any of the mem_objects is currently already acquired by OpenCL.",
  "if any of the mem_objects is currently not acquired by OpenCL.",
  "if a mem_object is not acquired by OpenCL.",
  "if a mem_object is not a EGL resource of the required type.",
  "if arg_value is not a valid accelerator object, and by clRetainAccelerator, clReleaseAccelerator, and clGetAcceleratorInfo when \"accelerator\" is not a valid accelerator object.",
  "if arg_value is not an accelerator object of the correct type, or when accelerator_type is not a valid accelerator type.",
  "if values described by descriptor are not valid, or if a combination of values is not valid.",
  "if accelerator_type is a valid accelerator type, but it not supported by any device in context.",
  "if the VA API display specified for interoperability is not compatible with the devices against which the context is to be created.",
  "if surface is not a VA API surface of the required type, by clGetMemObjectInfo when param_name is CL_MEM_VA_API_MEDIA_SURFACE_INTEL when was not created from a VA API surface, and from clGetImageInfo when param_name is CL_IMAGE_VA_API_PLANE_INTEL and image was not created from a VA API surface.",
  "if any of the mem_objects is already acquired by OpenCL.",
  "if any of the mem_objects are not currently acquired by OpenCL.",

  "if read or write to a buffer is illegal."
};


std::string oclGenErrorString(const cl_int error);
std::string clrngGenErrorString(const cl_int error);

}; //namespace cl

#endif
