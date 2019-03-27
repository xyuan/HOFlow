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
    std::string inputFileName, logFileName;
    
    
    // start up MPI
    if ( MPI_SUCCESS != MPI_Init( &argc , &argv ) ) {
        throw std::runtime_error("MPI_Init failed");
    }
    
    // HOFlowEnv singleton
    HOFlowEnv & hoflowEnv = HOFlowEnv::self();
      
    stk::diag::setEnabledTimerMetricsMask(stk::diag::METRICS_CPU_TIME | stk::diag::METRICS_WALL_TIME);
    Simulation::rootTimer().start();

    // start initial time
    double start_time = hoflowEnv.hoflow_time();
    
    // command line options.
    bool debug = true;
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "shows HOFlow version")
        ("debug,D", "turn on debug mode")
        ("input,i", po::value<std::string>(&inputFileName)->default_value("input.i"))
        ("log-file,o", po::value<std::string>(&logFileName), "Analysis log file")
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
    
    // deal with logfile name; if none supplied, go with inputFileName.log
    if (!vm.count("log-file")) {
        int dotPos = inputFileName.rfind(".");
        if ( -1 == dotPos ) {  
            // lacking extension
            logFileName = inputFileName + ".log";
        } 
        else {  
            // with extension; swap with .log
            logFileName = inputFileName.substr(0, dotPos) + ".log";
        }
    }
    
    // deal with log file stream
//    hoflowEnv.set_log_file_stream(logFileName, pprint); // disabled to force output to console

    // proceed with reading input file "document" from YAML
    YAML::Node doc = YAML::LoadFile(inputFileName.c_str());
    if (debug) {
        if (!hoflowEnv.parallel_rank())
            HOFlowParsingHelper::emit(std::cout, doc);
    }
    
    // create an Simulation object and give the input file as an argument
    Simulation sim(doc);
    
    // load the data specified in the input file
    sim.debug_ = debug;
    sim.load(doc);
    sim.breadboard();
    sim.initialize();
    sim.run();
    
    // stop timer
    const double stop_time = hoflowEnv.hoflow_time();
    const double total_time = stop_time - start_time;

    // parallel reduce overall times
    double g_sum, g_min, g_max;
    stk::all_reduce_min(hoflowEnv.parallel_comm(), &total_time, &g_min, 1);
    stk::all_reduce_max(hoflowEnv.parallel_comm(), &total_time, &g_max, 1);
    stk::all_reduce_sum(hoflowEnv.parallel_comm(), &total_time, &g_sum, 1);
    const int nprocs = hoflowEnv.parallel_size();

    // output total time
    hoflowEnv.hoflowOutputP0() << "Timing for Simulation: nprocs= " << nprocs << std::endl;
    hoflowEnv.hoflowOutputP0() << "           main() --  " << " \tavg: " << g_sum/double(nprocs)
                           << " \tmin: " << g_min << " \tmax: " << g_max << std::endl;

    // output memory usage
    {
        size_t now, hwm;
        stk::get_memory_usage(now, hwm);
        // min, max, sum
        size_t global_now[3] = {now,now,now};
        size_t global_hwm[3] = {hwm,hwm,hwm};

        stk::all_reduce(hoflowEnv.parallel_comm(), stk::ReduceSum<1>( &global_now[2] ) );
        stk::all_reduce(hoflowEnv.parallel_comm(), stk::ReduceMin<1>( &global_now[0] ) );
        stk::all_reduce(hoflowEnv.parallel_comm(), stk::ReduceMax<1>( &global_now[1] ) );

        stk::all_reduce(hoflowEnv.parallel_comm(), stk::ReduceSum<1>( &global_hwm[2] ) );
        stk::all_reduce(hoflowEnv.parallel_comm(), stk::ReduceMin<1>( &global_hwm[0] ) );
        stk::all_reduce(hoflowEnv.parallel_comm(), stk::ReduceMax<1>( &global_hwm[1] ) );

        hoflowEnv.hoflowOutputP0() << "Memory Overview: " << std::endl;

        hoflowEnv.hoflowOutputP0() << "HOFlow memory: total (over all cores) current/high-water mark= "
                                << std::setw(15) << human_bytes_double(global_now[2])
                                << std::setw(15) << human_bytes_double(global_hwm[2])
                                << std::endl;

        hoflowEnv.hoflowOutputP0() << "HOFlow memory:   min (over all cores) current/high-water mark= "
                                << std::setw(15) << human_bytes_double(global_now[0])
                                << std::setw(15) << human_bytes_double(global_hwm[0])
                                << std::endl;

        hoflowEnv.hoflowOutputP0() << "HOFlow memory:   max (over all cores) current/high-water mark= "
                                << std::setw(15) << human_bytes_double(global_now[1])
                                << std::setw(15) << human_bytes_double(global_hwm[1])
                                << std::endl;
    }

    Simulation::rootTimer().stop();

    //output timings consistent w/ rest of Sierra
    stk::diag::Timer & sierra_timer = Simulation::rootTimer();
    const double elapsed_time = sierra_timer.getMetric<stk::diag::WallTime>().getAccumulatedLap(false);
    stk::diag::Timer & mesh_output_timer = Simulation::outputTimer();
    double mesh_output_time = mesh_output_timer.getMetric<stk::diag::WallTime>().getAccumulatedLap(false);
    double time_without_output = elapsed_time-mesh_output_time;

    stk::parallel_print_time_without_output_and_hwm(hoflowEnv.parallel_comm(), time_without_output, hoflowEnv.hoflowOutputP0());

    stk::diag::printTimersTable(hoflowEnv.hoflowOutputP0(), Simulation::rootTimer(),
                                stk::diag::METRICS_CPU_TIME | stk::diag::METRICS_WALL_TIME,
                                false, hoflowEnv.parallel_comm());

    stk::diag::deleteRootTimer(Simulation::rootTimer());
 
    return 0;
}

