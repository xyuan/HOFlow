/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef ALGTRAITS_H
#define ALGTRAITS_H

#include <stk_topology/topology.hpp>

// limited supported now (P=1 3D elements)
struct AlgTraitsHex8 {
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 8;
  static constexpr int numScsIp_ = 12;
  static constexpr int numScvIp_ = 8;
  static constexpr int numGp_ = 8; // for FEM
  static constexpr stk::topology::topology_t topo_ = stk::topology::HEX_8;
};

struct AlgTraitsHex27 {
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 27;
  static constexpr int numScsIp_ = 216;
  static constexpr int numScvIp_ = 216;
  static constexpr int numGp_ = 27; // for FEM (not supported)
  static constexpr stk::topology::topology_t topo_ = stk::topology::HEX_27;
};

struct AlgTraitsTet4 {
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 4;
  static constexpr int numScsIp_ = 6;
  static constexpr int numScvIp_ = 4;
  static constexpr int numGp_ = 4; // for FEM (not supported)
  static constexpr stk::topology::topology_t topo_ = stk::topology::TET_4;
};

struct AlgTraitsPyr5 {
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 5;
  static constexpr int numScsIp_ = 12;
  static constexpr int numScvIp_ = 5;
  static constexpr int numGp_ = 5; // for FEM (not supported)
  static constexpr stk::topology::topology_t topo_ = stk::topology::PYRAMID_5;
};

struct AlgTraitsWed6 {
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 6;
  static constexpr int numScsIp_ = 9;
  static constexpr int numScvIp_ = 6;
  static constexpr int numGp_ = 6; // for FEM (not supported)
  static constexpr stk::topology::topology_t topo_ = stk::topology::WEDGE_6;
};

struct AlgTraitsQuad4_2D {
  static constexpr int nDim_ = 2;
  static constexpr int nodesPerElement_ = 4;
  static constexpr int numScsIp_ = 4;
  static constexpr int numScvIp_ = 4;
  static constexpr int numGp_ = 4; // for FEM (not supported)
  static constexpr stk::topology::topology_t topo_ = stk::topology::QUAD_4_2D;
};

struct AlgTraitsQuad9_2D {
  static constexpr int nDim_ = 2;
  static constexpr int nodesPerElement_ = 9;
  static constexpr int numScsIp_ = 24;
  static constexpr int numScvIp_ = 36;
  static constexpr int numGp_ = 9; // for FEM (not supported)
  static constexpr stk::topology::topology_t topo_ = stk::topology::QUAD_9_2D;
};

struct AlgTraitsTri3_2D {
  static constexpr int nDim_ = 2;
  static constexpr int nodesPerElement_ = 3;
  static constexpr int numScsIp_ = 3;
  static constexpr int numScvIp_ = 3;
  static constexpr int numGp_ = 3; // for FEM (not supported)
  static constexpr stk::topology::topology_t topo_ = stk::topology::TRI_3_2D;
};

template <int p> constexpr int nGL() { return (p % 2 == 0) ? p / 2 + 1 : (p + 1) / 2; }

template <int p>
struct AlgTraitsQuadGL_2D {
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = (p+1) * (p+1);
  static constexpr int numScsIp_ = 2 * p * (p + 1) * nGL<p>();
  static constexpr int numScvIp_ = nodesPerElement_ * nGL<p>() * nGL<p>();
  static constexpr int numGp_ = nodesPerElement_; // for FEM (not supported)

  static constexpr stk::topology::topology_t topo_ = static_cast<stk::topology::topology_t>(
    nodesPerElement_ + stk::topology::SUPERELEMENT_START
  );
  static constexpr stk::topology::topology_t baseTopo_ = stk::topology::QUAD_4_2D;
};

template <int p>
struct AlgTraitsHexGL {
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = (p+1)*(p+1)*(p+1);
  static constexpr int numScsIp_ = 3 * p * (p+1) * (p+1) * nGL<p>() * nGL<p>();
  static constexpr int numScvIp_ = nodesPerElement_ * nGL<p>() * nGL<p>() * nGL<p>();
  static constexpr int numGp_ = nodesPerElement_; // for FEM (not supported)

  static constexpr stk::topology::topology_t topo_ = static_cast<stk::topology::topology_t>(
    nodesPerElement_ + stk::topology::SUPERELEMENT_START
  );
  static constexpr stk::topology::topology_t baseTopo_ = stk::topology::HEX_8;
};

struct AlgTraitsEdge_3D
{
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 2;
  static constexpr int numScsIp_ = 1;
  static constexpr int numScvIp_ = 2;
  static constexpr stk::topology::topology_t topo_ = stk::topology::LINE_2;
};

//-------------------------------------------------------------------------------------------

struct AlgTraitsQuad4
{
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 4;
  static constexpr int nodesPerFace_ = nodesPerElement_;
  static constexpr int numScsIp_ = 4;
  static constexpr int numFaceIp_ = numScsIp_;
  static constexpr stk::topology::topology_t topo_ = stk::topology::QUAD_4;
};

struct AlgTraitsQuad9
{
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 9;
  static constexpr int nodesPerFace_ = nodesPerElement_;
  static constexpr int numScsIp_ = 36;
  static constexpr int numFaceIp_ = numScsIp_;
  static constexpr stk::topology::topology_t topo_ = stk::topology::QUAD_9;
};

struct AlgTraitsTri3
{
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = 3;
  static constexpr int nodesPerFace_ = nodesPerElement_;
  static constexpr int numScsIp_ = 3;
  static constexpr int numFaceIp_ = numScsIp_;
  static constexpr stk::topology::topology_t topo_ = stk::topology::TRI_3;
};

struct AlgTraitsEdge_2D
{
  static constexpr int nDim_ = 2;
  static constexpr int nodesPerElement_ = 2;
  static constexpr int nodesPerFace_ = nodesPerElement_;
  static constexpr int numScsIp_ = 2;
  static constexpr int numFaceIp_ = numScsIp_;
  static constexpr stk::topology::topology_t topo_ = stk::topology::LINE_2;
};


struct AlgTraitsEdge3_2D
{
  static constexpr int nDim_ = 2;
  static constexpr int nodesPerElement_ = 3;
  static constexpr int nodesPerFace_ = nodesPerElement_;
  static constexpr int numScsIp_ = 6;
  static constexpr int numFaceIp_ = numScsIp_;
  static constexpr stk::topology::topology_t topo_ = stk::topology::LINE_3;
};

template <int p>
struct AlgTraitsQuadGL
{
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = (p+1)*(p+1);
  static constexpr int nodesPerFace_ = nodesPerElement_;
  static constexpr int numScsIp_ = nGL<p>()*nGL<p>()*nodesPerElement_;
  static constexpr int numFaceIp_ = numScsIp_;
  static constexpr stk::topology::topology_t topo_ = static_cast<stk::topology::topology_t>(
    nodesPerElement_ + stk::topology::SUPERFACE_START
  );
  static constexpr stk::topology::topology_t baseTopo_ = stk::topology::QUAD_4;
};

template <int p>
struct AlgTraitsEdgeGL
{
  static constexpr int nDim_ = 3;
  static constexpr int nodesPerElement_ = (p+1);
  static constexpr int nodesPerFace_ = nodesPerElement_;
  static constexpr int numScsIp_ = nGL<p>()*nodesPerElement_;
  static constexpr int numFaceIp_ = numScsIp_;
  static constexpr stk::topology::topology_t topo_ = static_cast<stk::topology::topology_t>(
    nodesPerElement_ + stk::topology::SUPEREDGE_START
  );
  static constexpr stk::topology::topology_t baseTopo_ = stk::topology::LINE_2;
};

//-------------------------------------------------------------------------------------------

template <typename AlgTraitsFace, typename AlgTraitsElem>
struct AlgTraitsFaceElem
{
  using FaceTraits = AlgTraitsFace;
  using ElemTraits = AlgTraitsElem;

  static constexpr int nDim_ = ElemTraits::nDim_;
  static_assert( nDim_ == FaceTraits::nDim_, "inconsistent dimension specification");

  static constexpr int nodesPerElement_ = ElemTraits::nodesPerElement_;
  static constexpr int nodesPerFace_ = FaceTraits::nodesPerElement_;

  static constexpr int numScsIp_ = ElemTraits::numScsIp_;
  static constexpr int numScvIp_ = ElemTraits::numScvIp_;

  static constexpr int numFaceIp_ = FaceTraits::numScsIp_;

  static constexpr stk::topology::topology_t elemTopo_ = ElemTraits::topo_;
  static constexpr stk::topology::topology_t faceTopo_ = FaceTraits::topo_;
};

using AlgTraitsEdge2DTri32D = AlgTraitsFaceElem<AlgTraitsEdge_2D, AlgTraitsTri3_2D>;
using AlgTraitsEdge2DQuad42D = AlgTraitsFaceElem<AlgTraitsEdge_2D, AlgTraitsQuad4_2D>;

using AlgTraitsTri3Tet4 = AlgTraitsFaceElem<AlgTraitsTri3, AlgTraitsTet4>;
using AlgTraitsTri3Pyr5 = AlgTraitsFaceElem<AlgTraitsTri3, AlgTraitsPyr5>;
using AlgTraitsTri3Wed6 = AlgTraitsFaceElem<AlgTraitsTri3, AlgTraitsWed6>;

using AlgTraitsQuad4Hex8 = AlgTraitsFaceElem<AlgTraitsQuad4, AlgTraitsHex8>;
using AlgTraitsQuad4Pyr5 = AlgTraitsFaceElem<AlgTraitsQuad4, AlgTraitsPyr5>;
using AlgTraitsQuad4Wed6 = AlgTraitsFaceElem<AlgTraitsQuad4, AlgTraitsWed6>;

using AlgTraitsEdge32DQuad92D = AlgTraitsFaceElem<AlgTraitsEdge3_2D, AlgTraitsQuad9_2D>;
using AlgTraitsQuad9Hex27 = AlgTraitsFaceElem<AlgTraitsQuad9, AlgTraitsHex27>;

template <int p> using AlgTraitsEdgePQuadPGL = AlgTraitsFaceElem<AlgTraitsEdgeGL<p>, AlgTraitsQuadGL_2D<p>>;
template <int p> using AlgTraitsQuadPHexPGL = AlgTraitsFaceElem<AlgTraitsQuadGL<p>, AlgTraitsHexGL<p>>;

#endif /* ALGTRAITS_H */

