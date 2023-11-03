FPGA_BINARY_DEGREE=32
DIR=/mnt/RAID/saves/work_offload/Git/CPP/clWDK

INCLUDE=-I $(DIR)/include -I $(XILINX_XRT)/include -I /home/thomas/libraries/clRNG/include -I /home/thomas/libraries/clRNG/cl/include
DEFINE=-D DEBUG_OCL
LIBRARY=-lOpenCL -lclRNG
X_LIBRARY=-L $(XILINX_XRT)/lib -lxilinxopencl -lclRNG
CXX=g++
LLVMSPIR=/home/thomas/libraries/SPIRV-LLVM-Translator/build/tools/llvm-spirv/llvm-spirv
SPIRLINK=/home/thomas/libraries/SPIRV-Tools/build/tools/spirv-link
#SPIRLINK=spirv-link


P_SW_K=$(DIR)/build/clWDK_kernels/Emulation-SW
P_HW_K=$(DIR)/build/clWDK_kernels/Emulation-HW
P_K=$(DIR)/build/clWDK_kernels/Hardware
M_SW_K=$(P_SW_K)/makefile
M_HW_K=$(P_HW_K)/makefile
M_K=$(P_K)/makefile

P_SW=$(DIR)/build/clWDK_system_hw_link/Emulation-SW
P_HW=$(DIR)/build/clWDK_system_hw_link/Emulation-HW
P=$(DIR)/build/clWDK_system_hw_link/Hardware
M_SW=$(P_SW)/makefile
M_HW=$(P_HW)/makefile
M=$(P)/makefile


## all : makes build, test and doc
all:
	make build_host
	make build_gpu
	make build_fpga
	make test
	make doc

## build_host   : compiles executable for host
build_host: ocl.o wdk.o
	$(CXX) $(INCLUDE) src/host.cpp build/ocl.o build/wdk.o $(LIBRARY) -O3 -Wall -Wextra -std=c++17 -o bin/wdk_host -fopenmp $(DEFINE)

## build_gpu    : compiles executables for gpu
build_gpu: ocl.o wdk.o weierstrass.spv
	$(CXX) $(INCLUDE) src/gpu.cpp build/ocl.o build/wdk.o $(LIBRARY) -O3 -Wall -Wextra -std=c++17 -o bin/wdk_gpu -fopenmp $(DEFINE)

## build_fpga   : compiles host executable for xilinx fpga
build_fpga: ocl_d.o wdk_d.o
	$(CXX) $(INCLUDE) src/fpga.cpp build/ocl_d.o build/wdk_d.o $(X_LIBRARY) -O3 -Wall -Wextra -std=c++17 -o bin/wdk_fpga -fopenmp $(DEFINE) -D XOCL

## build_device : builds fpga device binary
build_device: weierstrass.xclbin

wdk.o:
	$(CXX) $(INCLUDE) -c src/wdk.cpp $(LIBRARY) -std=c++17 -Wall -Wextra -O3 -o build/wdk.o -fopenmp $(DEFINE)
ocl.o:
	$(CXX) $(INCLUDE) -c src/ocl.cpp $(LIBRARY) -std=c++17 -Wall -Wextra -O3 -o build/ocl.o $(DEFINE)
wdk_d.o:
	$(CXX) $(INCLUDE) -c src/wdk.cpp $(X_LIBRARY) -std=c++17 -Wall -Wextra -O3 -o build/wdk_d.o -fopenmp $(DEFINE) -D XOCL
ocl_d.o:
	$(CXX) $(INCLUDE) -c src/ocl.cpp $(X_LIBRARY) -std=c++17 -Wall -Wextra -O3 -o build/ocl_d.o $(DEFINE) -D XOCL

weierstrass.spv:
	mkdir -p bin
	mkdir -p bin/spirv
	mkdir -p bin/llvm

	clang -c -cl-std=CLC++ -target spir -O0 -emit-llvm src/ocl/weierstrass.cl -Xclang -finclude-default-header $(INCLUDE) -o bin/llvm/weierstrass8.bc -D DEGREE=8 -Wall -Wextra
	clang -c -cl-std=CLC++ -target spir -O0 -emit-llvm src/ocl/weierstrass.cl -Xclang -finclude-default-header $(INCLUDE) -o bin/llvm/weierstrass16.bc -D DEGREE=16 -Wall -Wextra
	clang -c -cl-std=CLC++ -target spir -O0 -emit-llvm src/ocl/weierstrass.cl -Xclang -finclude-default-header $(INCLUDE) -o bin/llvm/weierstrass32.bc -D DEGREE=32 -Wall -Wextra
	clang -c -cl-std=CLC++ -target spir -O0 -emit-llvm src/ocl/weierstrass.cl -Xclang -finclude-default-header $(INCLUDE) -o bin/llvm/weierstrass64.bc -D DEGREE=64 -Wall -Wextra
	clang -c -cl-std=CLC++ -target spir -O0 -emit-llvm src/ocl/weierstrass.cl -Xclang -finclude-default-header $(INCLUDE) -o bin/llvm/weierstrass128.bc -D DEGREE=128 -Wall -Wextra
	clang -c -cl-std=CLC++ -target spir -O0 -emit-llvm src/ocl/weierstrass.cl -Xclang -finclude-default-header $(INCLUDE) -o bin/llvm/weierstrass256.bc -D DEGREE=256 -Wall -Wextra

	#$(SPIRLINK) bin/llvm/weierstrass8.bc -o help.bc
	#$(LLVMSPIR) bin/llvm/weierstrass8.bc -o bin/spirv/weierstrass8.spv
	#$(SPIRLINK) bin/llvm/weierstrass16.bc -o help.bc
	#$(LLVMSPIR) help.bc -o bin/spirv/weierstrass16.spv
	#$(SPIRLINK) bin/llvm/weierstrass32.bc -o help.bc
	#$(LLVMSPIR) help.bc -o bin/spirv/weierstrass32.spv
	#$(SPIRLINK) bin/llvm/weierstrass64.bc -o help.bc
	#$(LLVMSPIR) help.bc -o bin/spirv/weierstrass64.spv
	#$(SPIRLINK) bin/llvm/weierstrass128.bc -o help.bc
	#$(LLVMSPIR) help.bc -o bin/spirv/weierstrass128.spv
	#$(SPIRLINK) bin/llvm/weierstrass256.bc -o help.bc
	#$(LLVMSPIR) help.bc -o bin/spirv/weierstrass256.spv

	#rm help.bc

weierstrass.sw.xclbin:
	cd $(P_SW); \
	make -f $(M_SW) all
	mkdir -p bin/fpga/sw
	cp build/clWDK_kernels/Emulation-SW/build/*.xo bin/fpga/sw/weierstrass$(FPGA_BINARY_DEGREE).xclbin
weierstrass.hw.xclbin:
	cd $(P_HW); \
	make -f $(M_HW) all
	mkdir -p bin/fpga/hw
	cp build/clWDK_kernels/Emulation-SW/build/*.xo bin/fpga/hw/weierstrass$(FPGA_BINARY_DEGREE).xclbin
weierstrass.xclbin:
	cd $(P); \
	make -f $(M) all
	mkdir -p bin/fpga
	cp build/clWDK_kernels/Emulation-SW/build/*.xo bin/fpga/weierstrass$(FPGA_BINARY_DEGREE).xclbin

weierstrass.sw.xo:
	cd $(P_SW_K); \
	export DEGREE=$(FPGA_BINARY_DEGREE); \
	make -f $(M_SW_K) all
weierstrass.hw.xo:
	cd $(P_HW_K); \
	export DEGREE=$(FPGA_BINARY_DEGREE); \
	make -f $(M_HW_K) all
weierstrass.xo:
	cd $(P_K); \
	export DEGREE=$(FPGA_BINARY_DEGREE); \
	make -f $(M_K) all

.PHONY: test
## test  : compiles and executes tests, creates example plots
test: build_gpu
	$(CXX) $(INCLUDE) test/test.cpp test/wdk_test.cpp build/wdk.o build/ocl.o $(LIBRARY) -O3 -Wall -Wextra -std=c++17 -o test/test -fopenmp $(DEFINE)
	test/test -s -r junit -o test/testreport.json
	python3 test/plot.py

.PHONY: doc
## doc   : creates doxygen documentation and additional information
doc:
	doxygen Doxyfile
	lualatex ./doc/doc.tex
	lualatex ./doc/doc.tex
	mv doc.pdf doc
	rm -f *.aux
	rm -f *.out
	rm -f *.log.

PHONY: clean
## clean : removes all generated files
clean:
	rm -r test/*.json
	rm -r build/*.o
	rm -r doc/doc.pdf.

PHONY: clean_device
## clean_device : removes all generated device object files
clean_device:
	cd $(P_SW_K); \
	make -f $(M_SW_K) clean
	cd $(P_HW_K); \
	make -f $(M_HW_K) clean
	cd $(P_K); \
	make -f $(M_K) clean
	cd $(P_SW); \
	make -f $(M_SW) clean
	cd $(P_HW); \
	make -f $(M_HW) clean
	cd $(P); \
	make -f $(M) clean

PHONY: realclean
## realclean : removes all generated files
realclean: clean
	rm -r test/test
	rm -r bin/*
	rm -rf build/*

PHONY : help
## help  : prints help
help : Makefile
	@sed -n 's/^##//p' $<
