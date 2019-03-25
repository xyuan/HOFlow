/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "Realms.h"
#include <Realm.h>
#include <HOFlowParsing.h>
#include <Simulation.h>

#include <yaml-cpp/yaml.h>
#include <iostream>

//! Constructor
Realms::Realms(Simulation & sim) :
    simulation_(sim)
{
    // nothing to do
}

//! Destructor
Realms::~Realms() {
    for (size_t ir = 0; ir < realmVector_.size(); ++ir) {
        delete realmVector_[ir];
    }
}

void Realms::breadboard(){
    for ( size_t irealm = 0; irealm < realmVector_.size(); ++irealm ) {
        realmVector_[irealm]->breadboard();
    }
}

//! Loops through all Realms defined in the input file create a object from class Realm for each
void Realms::load(const YAML::Node & node) {
    const YAML::Node realms = node["realms"];
    if (realms) {
        for ( size_t irealm = 0; irealm < realms.size(); ++irealm ) {
            const YAML::Node realm_node = realms[irealm];
            // check for multi_physics realm type...
            std::string realmType = "multi_physics";
            get_if_present(realm_node, "type", realmType, realmType);
            Realm * realm = NULL;
            if ( realmType == "multi_physics" ) {
                realm = new Realm(*this, realm_node);
                std::cout << "new realm type multi physics" << std::endl;
            } else {
                //realm = new InputOutputRealm(*this, realm_node);
                std::cout << "new realm type not multi physics" << std::endl;
            }
            realm->load(realm_node);
            realmVector_.push_back(realm);
        }
    } else {
        throw std::runtime_error("parser error Realms::load");
    }
}

//! Loops through all Realms stored and initialize them
void Realms::initialize() {
    for ( size_t irealm = 0; irealm < realmVector_.size(); ++irealm ) {
        realmVector_[irealm]->initialize();
    }
}

Simulation * Realms::root() { 
    return parent()->root(); 
}

Simulation * Realms::parent() { 
    return & simulation_; 
}

Realm * Realms::find_realm(std::string realm_name) {
    RealmVector::iterator realm_iter  = std::find_if(realmVector_.begin(), realmVector_.end(), IsString(realm_name));
    if (realm_iter != realmVector_.end())
        return *realm_iter;
    else
        return 0;
}