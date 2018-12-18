CXX = g++
C =  gcc


#GIT_VERSION := $(shell echo "`git symbolic-ref HEAD 2> /dev/null | cut -b 12-`-`git log --pretty=format:\"%h\" -1`")

#LDFLAGS= -ldl -lm -lpthread -lmpi -fPIC -lz -L$(MPI_ARCH_PATH)/lib64
LDFLAGS= -ldl -lm -lpthread -lmpi -fPIC -lz -lstk_io -lyaml-cpp -lmpi_cxx -lopen-rte -lopen-pal -lteuchoscomm 	-lteuchosnumerics -lteuchosparameterlist -lteuchoscore -lkokkoscore -lstk_mesh_base -lstk_topology -lstk_util_parallel -lstk_util_util -lshards -lstk_util_diag



#INCLUDES=-I./lnInclude/ -I/opt/local/include -I./ToolBox -I./ToolBox/GSL/include/ -I$(MPI_ARCH_PATH)/include
INCLUDES=-I/home/raphi/spack/opt/spack/linux-ubuntu18.04-x86_64/gcc-7.3.0/trilinos-master-cuwfubxtt2ngh5yb6pxqjb2pdaokophd/include -I/home/raphi/spack/opt/spack/linux-ubuntu18.04-x86_64/gcc-7.3.0/openmpi-3.1.3-tfegwcywpztpm57cl44hxf64kbckdjmu/include

LIBS=-L/home/raphi/spack/opt/spack/linux-ubuntu18.04-x86_64/gcc-7.3.0/trilinos-master-cuwfubxtt2ngh5yb6pxqjb2pdaokophd/lib -L/home/raphi/spack/opt/spack/linux-ubuntu18.04-x86_64/gcc-7.3.0/openmpi-3.1.3-tfegwcywpztpm57cl44hxf64kbckdjmu/lib

#CXXOPTIONS= -fdiagnostics-color=auto -std=c++14 -O3 -ftree-vectorizer-verbose=2 -ffast-math -ftree-vectorize -g3
CXXOPTIONS= -fdiagnostics-color=auto -std=c++17 -O3 
COPTIONS= -O3 


CFLAGS =  -c  $(INCLUDES) $(COPTIONS) $(LIBS)
CPPFLAGS =  -c  $(INCLUDES) $(CXXOPTIONS) $(LIBS)





executableName=main.exe



RM=rm -fr
Version=Opt
ObjDir=./1_obj
DepDir=./2_dep

COMMON_SRC := $(shell find . -name "*.cpp")

#lnInclude := $(shell rm -fr lnInclude; ./makeInclude .)

VPATH := $(dir $(COMMON_SRC))
COMMON_OBJS := $(patsubst %.cpp, $(ObjDir)/$(Version)/%.o, $(notdir $(COMMON_SRC)))
COMMON_DEPS := $(patsubst %.cpp, $(DepDir)/$(Version)/%.dep, $(notdir $(COMMON_SRC)))

$(ObjDir)/$(Version)/%.o : %.cpp
	@echo creating $@ ...
	@mkdir -p $(DepDir) $(ObjDir)
	@mkdir -p $(DepDir)/$(Version) $(ObjDir)/$(Version)
	$(CXX) $(CPPFLAGS) -c -o $@ $<
	@- $(CXX) $(CPPFLAGS) -MM -MT $@ $< -o $(DepDir)/$(Version)/$(notdir $(basename $<)).dep

# This will make the cbsdk shared library
./$(executableName): $(COMMON_OBJS)
	@echo Compilation done. Linking executable $(executableName)...
	@- $(CXX) $(COMMON_OBJS) $(LDFLAGS) $(LIBS) -o $@

-include $(COMMON_DEPS)

clean:
	@echo cleaning done
	@- $(RM) $(COMMON_OBJS) $(ObjDir)/$(Version) $(DepDir)/$(Version) $(executableName)
