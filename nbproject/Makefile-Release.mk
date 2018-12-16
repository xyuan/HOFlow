#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/HOFlowEnv.o \
	${OBJECTDIR}/src/HOFlowParsing.o \
	${OBJECTDIR}/src/LinearSolver.o \
	${OBJECTDIR}/src/LinearSolverConfig.o \
	${OBJECTDIR}/src/LinearSolvers.o \
	${OBJECTDIR}/src/Realm.o \
	${OBJECTDIR}/src/Realms.o \
	${OBJECTDIR}/src/Simulation.o \
	${OBJECTDIR}/src/TpetraLinearSolver.o \
	${OBJECTDIR}/src/TpetraLinearSolverConfig.o \
	${OBJECTDIR}/src/hoflow.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hoflow

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hoflow: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/hoflow ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/HOFlowEnv.o: src/HOFlowEnv.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/HOFlowEnv.o src/HOFlowEnv.cpp

${OBJECTDIR}/src/HOFlowParsing.o: src/HOFlowParsing.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/HOFlowParsing.o src/HOFlowParsing.cpp

${OBJECTDIR}/src/LinearSolver.o: src/LinearSolver.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/LinearSolver.o src/LinearSolver.cpp

${OBJECTDIR}/src/LinearSolverConfig.o: src/LinearSolverConfig.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/LinearSolverConfig.o src/LinearSolverConfig.cpp

${OBJECTDIR}/src/LinearSolvers.o: src/LinearSolvers.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/LinearSolvers.o src/LinearSolvers.cpp

${OBJECTDIR}/src/Realm.o: src/Realm.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Realm.o src/Realm.cpp

${OBJECTDIR}/src/Realms.o: src/Realms.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Realms.o src/Realms.cpp

${OBJECTDIR}/src/Simulation.o: src/Simulation.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Simulation.o src/Simulation.cpp

${OBJECTDIR}/src/TpetraLinearSolver.o: src/TpetraLinearSolver.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TpetraLinearSolver.o src/TpetraLinearSolver.cpp

${OBJECTDIR}/src/TpetraLinearSolverConfig.o: src/TpetraLinearSolverConfig.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TpetraLinearSolverConfig.o src/TpetraLinearSolverConfig.cpp

${OBJECTDIR}/src/hoflow.o: src/hoflow.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/hoflow.o src/hoflow.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
