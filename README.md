![HOFlowLogo](hoflow_logo.png "Logo to show the spirit of the project")

CFD solver based on CVFEM. This projects make extensive use of various packages from the [Trilinos](https://www.trilinos.org) project.
The framework is heavily inspired by the [Nalu](https://github.com/NaluCFD/Nalu) project.

## Installation
### Install dependencies
- Trilinos
- yaml-cpp
- boost

### Procedure
1. Install Spack
2. Install Trilinos with Spack
3. Edit the paths to trilinos and yaml in build/doConfigHOFlow, usually in spack/opt/etc.
4. Edit the path to your Spack installation in the bashrc (neccessary to run the executable)
5. Source the bashrc (neccessary to run the executable)

## Run
1. Add the location of the executable to PATH
2. Switch to the directory containing the simulation case (input and mesh file)
3. Execute the following command to run the simulation

```
$ hoflow_d.exe --input <input filename>
```

Execute the following command to see all options

```
$ hoflow --help
```
