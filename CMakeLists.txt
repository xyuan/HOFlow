############################ BASE ######################################
cmake_minimum_required (VERSION 3.1)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Set HOFlow's compilers, CMAKE_FIND_LIBRARY_PREFIXES
project(HOFlow CXX Fortran)

########################## OPTIONS #####################################
#option(ENABLE_WARNINGS "Add -Wall to show compiler warnings" ON)

########################## TRILINOS ####################################
message("   Trilinos_DIR = ${Trilinos_DIR}")
set(CMAKE_PREFIX_PATH ${Trilinos_DIR} ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH ${Trilinos_PATH} ${CMAKE_PREFIX_PATH})
find_package(Trilinos REQUIRED)
include_directories(SYSTEM ${Trilinos_INCLUDE_DIRS})
include_directories(SYSTEM ${Trilinos_TPL_INCLUDE_DIRS})

message("\nFound Trilinos!  Here are the details: ")
message("   Trilinos_DIR = ${Trilinos_DIR}")
message("   Trilinos_VERSION = ${Trilinos_VERSION}")
message("   Trilinos_PACKAGE_LIST = ${Trilinos_PACKAGE_LIST}")
message("   Trilinos_LIBRARIES = ${Trilinos_LIBRARIES}")
message("   Trilinos_BIN_DIRS = ${Trilinos_BIN_DIRS}")
message("   Trilinos_INCLUDE_DIRS = ${Trilinos_INCLUDE_DIRS}")
message("   Trilinos_LIBRARY_DIRS = ${Trilinos_LIBRARY_DIRS}")
message("   Trilinos_TPL_LIST = ${Trilinos_TPL_LIST}")
message("   Trilinos_TPL_INCLUDE_DIRS = ${Trilinos_TPL_INCLUDE_DIRS}")
message("   Trilinos_TPL_LIBRARIES = ${Trilinos_TPL_LIBRARIES}")
message("   Trilinos_TPL_LIBRARY_DIRS = ${Trilinos_TPL_LIBRARY_DIRS}")
message("   Trilinos_BUILD_SHARED_LIBS = ${Trilinos_BUILD_SHARED_LIBS}")
message("   Trilinos_CXX_COMPILER_FLAGS = ${Trilinos_CXX_COMPILER_FLAGS}")
message("End of Trilinos details\n")

message("Setting and checking of compilers:")
set(CMAKE_CXX_COMPILER ${Trilinos_CXX_COMPILER} )
set(CMAKE_C_COMPILER ${Trilinos_C_COMPILER} )
set(CMAKE_Fortran_COMPILER ${Trilinos_Fortran_COMPILER} )

# Build HOFlow as shared libraries if that is how Trilinos was built
if(Trilinos_BUILD_SHARED_LIBS)
  set(HOFlow_LIBRARY_TYPE SHARED)
  set(BUILD_SHARED_LIBS ON)
  message("-- Building HOFlow with shared libraries")
else(Trilinos_BUILD_SHARED_LIBS)
  set(HOFlow_LIBRARY_TYPE STATIC)
endif(Trilinos_BUILD_SHARED_LIBS)

############################ YAML ######################################
set(CMAKE_PREFIX_PATH ${YAML_DIR} ${CMAKE_PREFIX_PATH})
find_package(YAML-CPP QUIET)
if(YAML-CPP_FOUND)
  # YAML master branch is used
  include_directories(SYSTEM ${YAML_CPP_INCLUDE_DIR})
else()
  # YAML 0.5.3 is used
  find_library(YAML_CPP_LIBRARIES NAMES yaml-cpp PATHS ${YAML_DIR}/lib)
  find_path(YAML_CPP_INCLUDE_DIR yaml.h PATHS ${YAML_DIR}/include/yaml-cpp)
  if((DEFINED YAML_CPP_LIBRARIES) AND (DEFINED YAML_CPP_INCLUDE_DIR))
    include_directories(SYSTEM ${YAML_CPP_INCLUDE_DIR}/..)
    set(YAML-CPP_FOUND TRUE)
  endif()
endif()
if(YAML-CPP_FOUND)
  message("-- Found YAML-CPP = ${YAML_DIR}")
else()
  message(FATAL_ERROR "YAML-CPP NOT FOUND")
endif()

########################### HOFlow #####################################

# Add any extra flags based on compiler and/or OS
message("-- CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message("-- CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(EXTRA_CXX_FLAGS "")
  set(EXTRA_Fortran_FLAGS "")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(EXTRA_CXX_FLAGS "")
  set(EXTRA_Fortran_FLAGS "")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(EXTRA_CXX_FLAGS "")
  set(EXTRA_Fortran_FLAGS "")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  set(EXTRA_CXX_FLAGS "-restrict")
  set(EXTRA_Fortran_FLAGS "")
endif()

if(ENABLE_WARNINGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -Wall")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Trilinos_CXX_COMPILER_FLAGS} ${EXTRA_CXX_FLAGS}")
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${Trilinos_Fortran_COMPILER_FLAGS} ${EXTRA_Fortran_FLAGS}")

message("-- CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message("-- CMAKE_Fortran_FLAGS = ${CMAKE_Fortran_FLAGS}")

file(GLOB SOURCE src/*.cpp src/*/*.cpp src/*/*.F)
file(GLOB HEADER include/*.h include/*/*.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/*.h)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/include)
add_library(hoflow ${SOURCE} ${HEADER})
target_link_libraries(hoflow ${Trilinos_LIBRARIES})
target_link_libraries(hoflow ${YAML_CPP_LIBRARIES})

set(hoflow_ex_name "hoflow.exe")
#set(EXECUTABLE_OUTPUT_PATH ../) # Custom executable output path
message("-- CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
   set(hoflow_ex_name "hoflow_d.exe")
   message("Debug Build")
endif()

add_executable(${hoflow_ex_name} src/hoflow.cpp)
target_link_libraries(${hoflow_ex_name} hoflow)

install(TARGETS ${hoflow_ex_name} hoflow
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

message("\nAnd CMake says...:")