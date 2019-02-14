/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#include "MaterialPropertyData.h"

MaterialPropertyData::MaterialPropertyData() :
    type_(MaterialPropertyType_END),
    constValue_(0.0)
{
}

MaterialPropertyData::~MaterialPropertyData() {
}

