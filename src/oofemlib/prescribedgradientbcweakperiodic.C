/*
 * prescribedgradientbcweakperiodic.C
 *
 *  Created on: May 19, 2014
 *      Author: svennine
 */

#include "prescribedgradientbcweakperiodic.h"

#include "classfactory.h"

namespace oofem {
REGISTER_BoundaryCondition(PrescribedGradientBCWeakPeriodic);

PrescribedGradientBCWeakPeriodic :: PrescribedGradientBCWeakPeriodic(int n, Domain *d) :
    PrescribedGradientBCWeak(n, d)
{}

PrescribedGradientBCWeakPeriodic :: ~PrescribedGradientBCWeakPeriodic() {}

void PrescribedGradientBCWeakPeriodic :: initializeFrom(InputRecord &ir)
{
    PrescribedGradientBCWeak :: initializeFrom(ir);
    mMeshIsPeriodic = true;
}

void PrescribedGradientBCWeakPeriodic :: postInitialize()
{
    bool enforceCornerPeriodicity = true;
    int numSides = 2;
    createTractionMesh(enforceCornerPeriodicity, numSides);
}

void PrescribedGradientBCWeakPeriodic :: giveBoundaryCoordVector(FloatArray &oX, const FloatArray &iPos) const
{
    FloatArray xMinus;
    giveMirroredPointOnGammaMinus(xMinus, iPos);

    oX = {
        iPos [ 0 ] - xMinus [ 0 ], iPos [ 1 ] - xMinus [ 1 ]
    };
}

void PrescribedGradientBCWeakPeriodic :: checkIfCorner(bool &oIsCorner, bool &oDuplicatable, const FloatArray &iPos, const double &iNodeDistTol) const
{
    oIsCorner = false;
    oDuplicatable = false;

    FloatArray cornerPos = mLC;
    if ( distance(iPos, cornerPos) < iNodeDistTol ) {
        oIsCorner = true;
    }

    cornerPos = {
        mUC [ 0 ], mLC [ 1 ]
    };
    if ( distance(iPos, cornerPos) < iNodeDistTol ) {
        oIsCorner = true;
    }

    cornerPos = {
        mUC [ 0 ], mUC [ 1 ]
    };
    if ( distance(iPos, cornerPos) < iNodeDistTol ) {
        oIsCorner = true;
        if ( mTractionInterpOrder == 1 ) {
            oDuplicatable = true;
        }
    }

    cornerPos = {
        mLC [ 0 ], mUC [ 1 ]
    };
    if ( distance(iPos, cornerPos) < iNodeDistTol ) {
        oIsCorner = true;
    }
}
} /* namespace oofem */
