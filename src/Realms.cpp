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

Realms::Realms() {
}

Realms::~Realms() {
}

void Realms::load(const YAML::Node & node) {
    const YAML::Node realms = node["realms"];
    if (realms) {
        for ( size_t irealm = 0; irealm < realms.size(); ++irealm ) {
            const YAML::Node realm_node = realms[irealm];
            // check for multi_physics realm type...
            std::string realmType = "multi_physics";
            get_if_present(realm_node, "type", realmType, realmType);
            Realm *realm = NULL;
            if ( realmType == "multi_physics" ) {
                realm = new Realm(*this, realm_node);
                std::cout << "new ralm type multi physics" << std::endl;
            } else {
                //realm = new InputOutputRealm(*this, realm_node);
                std::cout << "new ralm type not multi physics" << std::endl;
            }
            realm->load(realm_node);
            realmVector_.push_back(realm);
        }
    } else {
        throw std::runtime_error("parser error Realms::load");
    }
}

void Realms::initialize() {
    for ( size_t irealm = 0; irealm < realmVector_.size(); ++irealm ) {
        realmVector_[irealm]->initialize();
    }
}

