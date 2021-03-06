/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "master_element/MasterElementFactory.h"
#include "master_element/MasterElement.h"

//#include "master_element/Hex8CVFEM.h"
//#include "master_element/Hex27CVFEM.h"
//#include "master_element/Pyr5CVFEM.h"
//#include "master_element/Wed6CVFEM.h"
//#include "master_element/Quad43DCVFEM.h"
//#include "master_element/Quad42DCVFEM.h"
//#include "master_element/Quad92DCVFEM.h"
#include "master_element/Tet4CVFEM.h"
#include "master_element/Tri32DCVFEM.h"
#include "master_element/Tri3DSCS.h"
#include "master_element/Tri2DSCV.h"
#include "master_element/Edge2DSCS.h"
//#include "master_element/MasterElementHO.h"

#include "HOFlowEnv.h"
#include "hoflow_make_unique.h"

#include <stk_util/util/ReportHandler.hpp>
#include <stk_topology/topology.hpp>

#include <cmath>
#include <iostream>
#include <memory>

std::unique_ptr<MasterElement> create_surface_master_element(stk::topology topo) {
    if (topo.is_super_topology()) {
        // super topologies uses different master element type
        return nullptr;
    }

    switch ( topo.value() ) {

//        case stk::topology::HEX_8:
//            return make_unique<HexSCS>();
//
//        case stk::topology::HEX_27:
//            return make_unique<Hex27SCS>();

        case stk::topology::TET_4:
            return make_unique<TetSCS>();

//        case stk::topology::PYRAMID_5:
//            return make_unique<PyrSCS>();
//
//        case stk::topology::WEDGE_6:
//            return make_unique<WedSCS>();
//
//        case stk::topology::QUAD_4:
//            return make_unique<Quad3DSCS>();
//
//        case stk::topology::QUAD_9:
//            return make_unique<Quad93DSCS>();

        case stk::topology::TRI_3:
            return make_unique<Tri3DSCS>();

//        case stk::topology::QUAD_4_2D:
//            return make_unique<Quad42DSCS>();
//
//        case stk::topology::QUAD_9_2D:
//            return make_unique<Quad92DSCS>();

        case stk::topology::TRI_3_2D:
            return make_unique<Tri32DSCS>();

        case stk::topology::LINE_2:
            return make_unique<Edge2DSCS>();

//        case stk::topology::LINE_3:
//            return make_unique<Edge32DSCS>();
//
//        case stk::topology::SHELL_QUAD_4:
//            HOFlowEnv::self().hoflowOutputP0() << "SHELL_QUAD_4 only supported for io surface transfer applications" << std::endl;
//            return make_unique<Quad3DSCS>();

        case stk::topology::SHELL_TRI_3:
            HOFlowEnv::self().hoflowOutputP0() << "SHELL_TRI_3 only supported for io surface transfer applications" << std::endl;
            return make_unique<Tri3DSCS>();

        case stk::topology::BEAM_2:
            HOFlowEnv::self().hoflowOutputP0() << "BEAM_2 is only supported for io surface transfer applications" << std::endl;
            return make_unique<Edge2DSCS>();

        default:
            HOFlowEnv::self().hoflowOutputP0() << "sorry, we only support TET_4 and TRI_3 surface elements" << std::endl;
            HOFlowEnv::self().hoflowOutputP0() << "your type is " << topo.value() << std::endl;
            break;
    }
    return nullptr;
}
//--------------------------------------------------------------------------
std::unique_ptr<MasterElement> create_volume_master_element(stk::topology topo) {
    if (topo.is_super_topology()) {
        // super topologies uses different master element type
        return nullptr;
    }

    switch ( topo.value() ) {

//        case stk::topology::HEX_8:
//            return make_unique<HexSCV>();
//
//        case stk::topology::HEX_27:
//            return make_unique<Hex27SCV>();

        case stk::topology::TET_4:
            return make_unique<TetSCV>();

//        case stk::topology::PYRAMID_5:
//            return make_unique<PyrSCV>();
//
//        case stk::topology::WEDGE_6:
//            return make_unique<WedSCV>();
//
//        case stk::topology::QUAD_4_2D:
//            return make_unique<Quad42DSCV>();
//
//        case stk::topology::QUAD_9_2D:
//            return make_unique<Quad92DSCV>();

        case stk::topology::TRI_3_2D:
            return make_unique<Tri32DSCV>();

        default:
            HOFlowEnv::self().hoflowOutputP0() << "sorry, we only support tet4 volume elements" << std::endl;
            HOFlowEnv::self().hoflowOutputP0() << "your type is " << topo.value() << std::endl;
            break;
    }
    return nullptr;
}

std::unique_ptr<MasterElement> create_surface_master_element(stk::topology topo, int dimension, std::string quadType) {
    if (!topo.is_super_topology()) {
        // regular topologies uses different master element type
        return nullptr;
    }

//    ThrowRequire(dimension > 0 && dimension < 4);
//
//    auto desc = ElementDescription::create(dimension, topo);
//
//    auto basis = (topo.is_superelement()) ?
//        LagrangeBasis(desc->inverseNodeMap, desc->nodeLocs1D)
//      : LagrangeBasis(desc->inverseNodeMapBC, desc->nodeLocs1D);
//
//    auto quad = TensorProductQuadratureRule(quadType, desc->polyOrder);
//
//    if (topo.is_superedge()) {
//      ThrowRequire(desc->baseTopo == stk::topology::QUAD_4_2D);
//      return make_unique<HigherOrderEdge2DSCS>(*desc, basis, quad);
//    }
//
//    if (topo.is_superface()) {
//      ThrowRequire(desc->baseTopo == stk::topology::HEX_8);
//      return make_unique<HigherOrderQuad3DSCS>(*desc, basis, quad);
//    }
//
//    if (topo.is_superelement() && desc->baseTopo == stk::topology::QUAD_4_2D) {
//      return make_unique<HigherOrderQuad2DSCS>(*desc, basis, quad);
//    }
//
//    if (topo.is_superelement() && desc->baseTopo == stk::topology::HEX_8) {
//      return make_unique<HigherOrderHexSCS>(*desc, basis, quad);
//    }

    return nullptr;
}

std::unique_ptr<MasterElement> create_volume_master_element(stk::topology topo, int dimension, std::string quadType) {
  if (!topo.is_super_topology()) {
    // regular topologies uses different master element type
    return nullptr;
  }

  ThrowRequire(dimension > 0 && dimension < 4);
//
//  auto desc = ElementDescription::create(dimension, topo);
//  auto basis = LagrangeBasis(desc->inverseNodeMap, desc->nodeLocs1D);
//  auto quad = TensorProductQuadratureRule(quadType, desc->polyOrder);

//  switch (desc->baseTopo.value()) {
//    case stk::topology::QUADRILATERAL_4_2D:
//      return make_unique<HigherOrderQuad2DSCV>(*desc, basis, quad);
//    case stk::topology::HEXAHEDRON_8:
//      return make_unique<HigherOrderHexSCV>(*desc, basis, quad);
//    default:
//      HOFlowEnv::self().hoflowOutputP0() << "High order elements only support base quad4 and hex8 meshes" << std::endl;
//      break;
//  }
  return nullptr;
}

std::map<stk::topology, std::unique_ptr<MasterElement>> MasterElementRepo::surfaceMeMap_;

MasterElement * MasterElementRepo::get_surface_master_element(const stk::topology & theTopo, int dimension, std::string quadType) {
    auto it = surfaceMeMap_.find(theTopo);
    if (it == surfaceMeMap_.end()) {
        if (!theTopo.is_super_topology()) {
            surfaceMeMap_[theTopo] = create_surface_master_element(theTopo);
        } else {
            surfaceMeMap_[theTopo] = create_surface_master_element(theTopo, dimension, quadType);
        }
    }
    MasterElement* theElem = surfaceMeMap_.at(theTopo).get();
    ThrowRequire(theElem != nullptr);
    
    return theElem;
}

std::map<stk::topology, std::unique_ptr<MasterElement>> MasterElementRepo::volumeMeMap_;

MasterElement * MasterElementRepo::get_volume_master_element(const stk::topology& theTopo, int dimension, std::string quadType) {
    auto it = volumeMeMap_.find(theTopo);
    if (it == volumeMeMap_.end()) {
        if (!theTopo.is_super_topology()) {
            volumeMeMap_[theTopo] = create_volume_master_element(theTopo);
        } else {
            volumeMeMap_[theTopo] = create_volume_master_element(theTopo, dimension, quadType);
        }
    }
    MasterElement * theElem = volumeMeMap_.at(theTopo).get();
    ThrowRequire(theElem != nullptr);
    return theElem;
}

void MasterElementRepo::clear() {
    surfaceMeMap_.clear();
    volumeMeMap_.clear();
}