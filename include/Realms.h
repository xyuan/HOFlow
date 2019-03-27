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

/** Container class that stores one or multiple realms.
 *
 * A Realm is a representation of a physical domain on which
 * some physical equations has to be solved. Mulitple realms
 * can interact with each other via transfers.
 *
 * @note Transfers are not implemented -> multiple realms
 * can not interact with each other
 */
class Realms {
public:
    /** Initialize some variables*/
    Realms(Simulation & sim);
    
    /** Deletes all realms stored in realmVector_*/
    ~Realms();
    
    /** Creates objects for each realm specified in the input file*/
    void load(const YAML::Node & node);
    
    /** Stuff that has to be done before initialization
     * 
     * Calls the breadboard function of each realm stored in realmVector_
     * @note Not sure if this is really needed, may be able to put
     * these things in the initailize method
     */
    void breadboard();
    
    /** Calls the initialize method for each realm  stored in realmVector_*/
    void initialize();
    
    Simulation * root();
    Simulation * parent();
    
    /** Helper struct to find a realm based on its name stored in it*/
    struct IsString {
        IsString(std::string & str) : str_(str) {}
        std::string & str_;
        bool operator()(Realm * realm) { return realm->name_ == str_; }
    };
    
    /** Searches for a realm in the realmVector_ variable and returns a pointer to it*/
    Realm * find_realm(std::string realm_name);
    
    /** Return the number of realms*/
    size_t size() { return realmVector_.size(); }
    
    Simulation & simulation_;
    
    /** Every realm created is stored in here*/
    RealmVector realmVector_;
};

#endif /* REALMS_H */

