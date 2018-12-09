/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef TPETRALINEARSOLVERCONFIG_H
#define TPETRALINEARSOLVERCONFIG_H

class TpetraLinearSolverConfig : public LinearSolverConfig {
public:
    TpetraLinearSolverConfig();
    ~TpetraLinearSolverConfig();
    void load(const YAML::Node & node);

    virtual void load(const YAML::Node & node) final;
    bool getSummarizeMueluTimer() { return summarizeMueluTimer_; }
    std::string & muelu_xml_file() {return muelu_xml_file_;}
    bool use_MueLu() const {return useMueLu_;}

private:
    std::string muelu_xml_file_;
    bool summarizeMueluTimer_{false};
    bool useMueLu_{false};
};

#endif /* TPETRALINEARSOLVERCONFIG_H */

