/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include <iostream>
#include <iterator>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <boost/program_options.hpp>

#include <Simulation.h>
#include <HOFlowEnv.h>

namespace po = boost::program_options;

int main(int argc, char** argv) {
    const std::string VERSION = "0.0.1";
    std::string inputFileName;
    
    // HOFlowEnv singleton
    HOFlowEnv & hoflowEnv = HOFlowEnv::self();
    
    // command line options.
    bool debug = false;
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "shows HOFlow version")
        ("debug,D", "turn on debug mode")
        ("input,i", po::value<std::string>(&inputFileName)->default_value("input.i"))
    ;

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);  
    
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    
    if (vm.count("version")) {
        std::cout << "HOFlow Version: " << VERSION << std::endl;
        return 0;
    }
    
    if (vm.count("debug")) {
        debug = true;
    }
    
    // error message if no input file is specified, exit program
    std::ifstream fin(inputFileName);
    if (!fin.good()) {
        std::cerr << "Input file is not specified or does not exist: user specified (or default) name= " << inputFileName << std::endl;
        return 0;
    }
    
    // reading input file
    YAML::Node doc = YAML::LoadFile(inputFileName);
    
    // create an Simulation object and give the input file as an argument
    Simulation sim(doc);
    
    // load the data specified in the input file
    std::cout << "load input file.." << std::endl;
    sim.load(doc);
    
    std::cout << "initialize classes.." << std::endl;
    sim.initialize();
    
    std::cout << "run simulation.." << std::endl;
    sim.run();
 
    return 0;
}

