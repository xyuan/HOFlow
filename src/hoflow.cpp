/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include <iostream>
#include <iterator>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <yaml-cpp/yaml.h>
#include <boost/program_options.hpp>
#include <mpi.h>

// util
#include <stk_util/environment/perf_util.hpp>
#include <stk_util/parallel/ParallelReduce.hpp>

#include <HOFlowParsing.h>
#include <Simulation.h>
#include <HOFlowEnv.h>

namespace po = boost::program_options;

static std::string human_bytes_double(double bytes) {
    const double K = 1024;
    const double M = K*1024;
    const double G = M*1024;

    std::ostringstream out;
    if (bytes < K) {
        out << bytes << " B";
    } else if (bytes < M) {
        bytes /= K;
        out << bytes << " K";
    } else if (bytes < G) {
        bytes /= M;
        out << bytes << " M";
    } else {
        bytes /= G;
        out << bytes << " G";
    }
    
    return out.str();
}

int main(int argc, char** argv) {
    const std::string VERSION = "0.0.1";
    std::string inputFileName;
    
    
    // start up MPI
    if ( MPI_SUCCESS != MPI_Init( &argc , &argv ) ) {
        throw std::runtime_error("MPI_Init failed");
    }
    
    // HOFlowEnv singleton
    HOFlowEnv & hoflowEnv = HOFlowEnv::self();
    
    // command line options.
    bool debug = true;
    
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
    sim.debug_ = debug;
    sim.load(doc);
    
    sim.initialize();
    
    sim.run();
 
    return 0;
}

