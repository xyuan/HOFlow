/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef MASTERELEMENTFACTORY_H
#define MASTERELEMENTFACTORY_H

#include <string>
#include <map>
#include <memory>

namespace stk { struct topology; }

class MasterElement;

struct MasterElementRepo
{
public:
  static MasterElement*
  get_surface_master_element(
    const stk::topology& theTopo,
    int dimension = 0,
    std::string quadType = "GaussLegendre");

  static MasterElement*
  get_volume_master_element(
    const stk::topology& theTopo,
    int dimension = 0,
    std::string quadType = "GaussLegendre");

  static void clear();
private:
  MasterElementRepo() = default;
  static std::map<stk::topology, std::unique_ptr<MasterElement>> surfaceMeMap_;
  static std::map<stk::topology, std::unique_ptr<MasterElement>> volumeMeMap_;
};

#endif /* MASTERELEMENTFACTORY_H */

