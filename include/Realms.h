/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef REALMS_H
#define REALMS_H

#include <yaml-cpp/yaml.h>
#include <Realm.h>
#include <vector>

class Simulation;

typedef std::vector<Realm *> RealmVector;

//! Stores one or multiple Realms (objects of the class Realm)
class Realms {
public:
    Realms(Simulation & sim);
    ~Realms();
    void load(const YAML::Node & node);
    void initialize();
    Simulation * root();
    Simulation * parent();
    
    size_t size() { return realmVector_.size(); }
    
    Simulation & simulation_;
    RealmVector realmVector_;
};

#endif /* REALMS_H */

