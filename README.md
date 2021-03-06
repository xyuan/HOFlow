![HOFlowLogo](hoflow_logo.png "Logo to show the spirit of the project")

CFD solver based on CVFEM. This projects make extensive use of various packages from the [Trilinos](https://www.trilinos.org) project.
The framework is heavily inspired by the [Nalu](https://github.com/NaluCFD/Nalu) project.

## Installation
### Install dependencies
- Trilinos
- yaml-cpp
- boost

### Build HOFlow
1. Install Spack
2. Install Trilinos with Spack
3. Edit the paths to trilinos and yaml in build/do-configHOFlow, usually spack/opt/etc.
4. Change to build/ directory
4. Execute the following command to run cmake

```
$ ./do-configHOFlow
```

5. Execute the following command to build

```
$ make
```

### Build Documentation
1. Change to doc/ directory
2. Execute the following command to run doxygen

```
$ doxygen
```

If the graphs (e.g. class hierarchy) are not properly displayed, install graphviz.

## Run
1. Add the location of the executable to the PATH variable, usually build/hoflow_d.exe
2. Edit the path to your Spack installation in the bashrc
3. Source the bashrc
2. Switch to the directory containing the simulation case (where the input and mesh file is located)
3. Execute the following command to run the simulation

```
$ hoflow_d.exe --input <input filename>
```

Execute the following command to see all options

```
$ hoflow_d.exe --help
```
