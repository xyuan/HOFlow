/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef OUTPUTINFO_H
#define OUTPUTINFO_H

#include <HOFlowParsing.h>

#include <string>
#include <set>

namespace Ioss{
  class PropertyManager;
}

/** Container class for the information about simulation output
 * in the input file.
 * 
 * Reads input file and stores the values specified. If nothing
 * specified, default values are used.
 */
class OutputInfo {
public:
    OutputInfo();
    ~OutputInfo();
    
    /** Reads input file and store the value specified
     * or if nothing present use default value.
     */
    void load(const YAML::Node & node);

    // helper methods for compression options
    int get_output_compression();
    bool get_output_shuffle();

    int get_restart_compression();
    bool get_restart_shuffle();

    std::string outputDBName_;

    // catalyst options
    std::string catalystFileName_;
    std::string catalystParseJson_;
    std::string paraviewScriptName_;

    int outputFreq_;
    int outputStart_;
    bool outputNodeSet_; 
    int serializedIOGroupSize_;
    bool hasOutputBlock_;
    bool hasRestartBlock_;
    bool activateRestart_;
    bool meshAdapted_;
    double restartTime_;
    std::string restartDBName_;
    int restartFreq_;
    int restartStart_;
    int restartMaxDataBaseStepSize_;
    bool restartNodeSet_;
    int outputCompressionLevel_;
    bool outputCompressionShuffle_;
    int restartCompressionLevel_;
    bool restartCompressionShuffle_;

    std::pair<bool, double> userWallTimeResults_;
    std::pair<bool, double> userWallTimeRestart_;

    // manage the properties for io
    Ioss::PropertyManager *outputPropertyManager_;
    Ioss::PropertyManager *restartPropertyManager_;

    std::set<std::string> outputFieldNameSet_;
    std::set<std::string> restartFieldNameSet_;
};

#endif /* OUTPUTINFO_H */

