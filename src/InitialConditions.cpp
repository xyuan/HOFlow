/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "InitialConditions.h"

InitialConditions::InitialConditions(Realm & realm) :
    realm_(realm)
{}

InitialConditions::~InitialConditions() {
    for ( size_t j_initial_condition = 0; j_initial_condition < initialConditionVector_.size(); ++j_initial_condition ) {
        delete initialConditionVector_[j_initial_condition];
    }
}

void InitialConditions::load(YAML::Node node) {
    /*InitialCondition tmp_initial_condition(*this);
   
    if(node["initial_conditions"]) {
        const YAML::Node initial_conditions = node["initial_conditions"];
        for ( size_t j_initial_condition = 0; j_initial_condition < initial_conditions.size(); ++j_initial_condition ) {
            const YAML::Node initial_condition_node = initial_conditions[j_initial_condition];
            InitialCondition* ic = tmp_initial_condition.load(initial_condition_node);
            initialConditionVector_.push_back(ic);
        }
    }
    else {
        throw std::runtime_error("parser error InitialConditions::load");
    }

    return this;*/
}

size_t InitialConditions::size() {
    return initialConditionVector_.size();
}

Realm *InitialConditions::parent() { 
    return &realm_; 
}

