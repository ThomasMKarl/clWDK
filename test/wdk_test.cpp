#include<catch2/catch.hpp>

#include "wdk.h"



SCENARIO("OpenCL environment can be set up", "[ocl]")
{
    GIVEN("A default OCL environment")
    {
        OclSetup oclSetup{};
        REQUIRE(oclSetup.getLastError() == CL_SUCCESS);

        WHEN("a queue is created")
        {
            oclSetup.oclPrepareQueue(0,0);
            THEN("there is no OCL error")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
        }

        WHEN("a program is built")
        {
            oclSetup.oclBuildProgram(getSpirBinaryPath(8), "");
            THEN("the program can be loaded and compiled for degree 8")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
            oclSetup.oclBuildProgram(getSpirBinaryPath(16), "");
            THEN("the program can be loaded and compiled for degree 16")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
            oclSetup.oclBuildProgram(getSpirBinaryPath(32), "");
            THEN("the program can be loaded and compiled for degree 32")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
            oclSetup.oclBuildProgram(getSpirBinaryPath(64), "");
            THEN("the program can be loaded and compiled for degree 64")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
            oclSetup.oclBuildProgram(getSpirBinaryPath(128), "");
            THEN("the program can be loaded and compiled for degree 128")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
            oclSetup.oclBuildProgram(getSpirBinaryPath(256), "");
            THEN("the program can be loaded and compiled for degree 256")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
        }
        
        WHEN("a kernel is built")
        {
            oclSetup.createKernelSingle();
            THEN("the kernel function is found for single precision")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }

            oclSetup.createKernelDouble();
            THEN("the kernel function is found for double precision")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }

            oclSetup.createKernelHalf();
            THEN("the kernel function is found for half precision")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
        }
    }
}

SCENARIO("Pictures can be created", "[picture]")
{
    WHEN("a completely one-colored picture is created")
    {
        Picture picture(2,3);
        std::fill(picture.imageData.begin(), picture.imageData.end(),255);
        THEN("it can be stored and loaded from disk")
        {
            picture.save("test/test.ppm");
            std::ifstream output("test/test.ppm");
            std::string string((std::istreambuf_iterator<char>(output)),std::istreambuf_iterator<char>());
            REQUIRE(string == "P3\n2 3\n255\n255 0 0 255 0 0 \n255 0 0 255 0 0 \n255 0 0 255 0 0 ");
        }
    }
}

SCENARIO("dummy program can be executed", "[picture],[ocl]")
{
    GIVEN("a default environment")
    {
        limits<short int> limit{};
        limit.lower = -1;
        limit.upper =  1;
        WdkSetup<float> wdkSetup(32, 8, 0.0001, limit);

        Picture picture(3,3);

        OclSetup oclSetup{};
        oclSetup.oclPrepareQueue();
        std::string spirBinaryPath = getSpirBinaryPath(32);
        oclSetup.oclBuildProgram(spirBinaryPath, getClrngIncludePath());
        oclSetup.createKernelSingle();

        WHEN("the kernel is executed")
        {
            computePictureData(oclSetup, picture, wdkSetup);
            THEN("a result can be retrieved")
            {
                REQUIRE(oclSetup.getLastError() == CL_SUCCESS);
            }
        }
    }
}