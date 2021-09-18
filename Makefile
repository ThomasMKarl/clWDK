INCLUDE=-I ./include -I /home/thomas/libraries/clRNG/include -I /home/thomas/libraries/clRNG/cl/include
DEFINE=-D DEBUG_OCL
LIBRARY=-lOpenCL -lclRNG
CXX=g++
LLVMSPIR=/home/thomas/libraries/SPIRV-LLVM-Translator/build/tools/llvm-spirv/llvm-spirv
SPIRLINK=/home/thomas/libraries/SPIRV-Tools/build/tools/spirv-link
#SPIRLINK=spirv-link

## all   : makes build, test and doc
all:
	make build
	make test
	make doc

## build : compiles executables
build: ocl.o wdk.o weierstrass.spv
	$(CXX) $(INCLUDE) src/gpu.cpp  src/ocl.o src/wdk.o $(LIBRARY) -O3 -Wall -Wextra -std=c++17 -o bin/wdk_gpu  -fopenmp $(DEFINE)
	$(CXX) $(INCLUDE) src/host.cpp src/ocl.o src/wdk.o $(LIBRARY) -O3 -Wall -Wextra -std=c++17 -o bin/wdk_host -fopenmp $(DEFINE)

wdk.o:
	$(CXX) $(INCLUDE) -c src/wdk.cpp $(LIBRARY) -std=c++17 -Wall -Wextra -O3 -o src/wdk.o -fopenmp $(DEFINE)
ocl.o:
	$(CXX) $(INCLUDE) -c src/ocl.cpp $(LIBRARY) -std=c++17 -Wall -Wextra -O3 -o src/ocl.o $(DEFINE)

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

.PHONY: test
## test  : compiles and executes tests, creates example plots
test: build
	$(CXX) $(INCLUDE) -c src/wdk.cpp $(LIBRARY) -std=c++17 -Wall -Wextra -O3 -o src/wdk.o 
	$(CXX) $(INCLUDE) test/test.cpp test/wdk_test.cpp src/wdk.o $(LIBRARY) -O3 -Wall -Wextra -std=c++17 -o test/test
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
	rm -r bin/*
	rm -r test/*
	rm -r src/*.o
	rm -r doc/doc.pdf.

PHONY : help
## help  : prints help
help : Makefile
	@sed -n 's/^##//p' $<
