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

class Realms {
public:
    Realms();
    ~Realms();
    void load(const YAML::Node & node);
    void initialize();
    
    size_t size() {return realmVector_.size();}
    typedef std::vector<Realm *> RealmVector;
    
    //Simulation & simulation_;
    RealmVector realmVector_;
};

#endif /* REALMS_H */

