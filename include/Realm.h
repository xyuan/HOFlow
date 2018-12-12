/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef REALM_H
#define REALM_H

#include <yaml-cpp/yaml.h>

class Realm {
public:
    Realm();
    ~Realm();
    void load(const YAML::Node& node);
    void initialize();
};

#endif /* REALM_H */

