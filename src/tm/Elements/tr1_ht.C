/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "tm/Elements/tr1_ht.h"
#include "fei2dtrlin.h"
#include "crosssection.h"
#include "gausspoint.h"
#include "gaussintegrationrule.h"
#include "floatmatrix.h"
#include "floatarray.h"
#include "intarray.h"
#include "mathfem.h"
#include "classfactory.h"


namespace oofem {
REGISTER_Element(Tr1_ht);
REGISTER_Element(Tr1_hmt);
REGISTER_Element(Tr1_mt);

FEI2dTrLin Tr1_ht :: interp(1, 2);

Tr1_ht :: Tr1_ht(int n, Domain *aDomain) :
    TransportElement(n, aDomain, HeatTransferEM), SpatialLocalizerInterface(this), ZZNodalRecoveryModelInterface(this)
{
    numberOfDofMans  = 3;
    numberOfGaussPoints = 1;
}

Tr1_hmt :: Tr1_hmt(int n, Domain *aDomain) : Tr1_ht(n, aDomain)
{
    emode = HeatMass1TransferEM;
}

Tr1_mt :: Tr1_mt(int n, Domain *aDomain) : Tr1_ht(n, aDomain)
{
    emode = Mass1TransferEM;
}


FEInterpolation *
Tr1_ht :: giveInterpolation() const { return & this->interp; }

void
Tr1_ht :: computeGaussPoints()
{
    if ( integrationRulesArray.size() == 0 ) {
        integrationRulesArray.resize( 1 );
        integrationRulesArray [ 0 ] = std::make_unique<GaussIntegrationRule>(1, this, 1, 3);
        this->giveCrossSection()->setupIntegrationPoints(* integrationRulesArray [ 0 ], numberOfGaussPoints, this);
    }
}


void
Tr1_ht :: initializeFrom(InputRecord &ir)
{
    numberOfGaussPoints = 1;
    TransportElement :: initializeFrom(ir);
}


double
Tr1_ht :: computeVolumeAround(GaussPoint *gp)
{
    double determinant = fabs( this->interp.giveTransformationJacobian( gp->giveNaturalCoordinates(), FEIElementGeometryWrapper(this) ) );
    double weight = gp->giveWeight();
    return determinant * weight * this->giveCrossSection()->give(CS_Thickness, gp);
}


double
Tr1_ht :: giveThicknessAt(const FloatArray &gcoords)
{
    return this->giveCrossSection()->give(CS_Thickness, gcoords, this, false);
}


double
Tr1_ht :: computeEdgeVolumeAround(GaussPoint *gp, int iEdge)
{
    double determinant = fabs( this->interp.edgeGiveTransformationJacobian( iEdge, gp->giveNaturalCoordinates(), FEIElementGeometryWrapper(this) ) );
    FloatArray gc;
    double thick = this->giveCrossSection()->give(CS_Thickness, gp->giveNaturalCoordinates(), NULL); // 't'
    return determinant * thick * gp->giveWeight();
}


Interface *
Tr1_ht :: giveInterface(InterfaceType interface)
{
    if ( interface == SpatialLocalizerInterfaceType ) {
        return static_cast< SpatialLocalizerInterface * >(this);
    } else if ( interface == EIPrimaryFieldInterfaceType ) {
        return static_cast< EIPrimaryFieldInterface * >(this);
    } else if ( interface == ZZNodalRecoveryModelInterfaceType ) {
        return static_cast< ZZNodalRecoveryModelInterface * >(this);
    }

    return nullptr;
}


void
Tr1_ht::printOutputAt(FILE *file, TimeStep *tStep)
// Performs end-of-step operations.
{
	//FloatArray v;
	//GaussPoint *gp;
	//fprintf(file, "element %d (%8d):\n", this->giveLabel(), number);
}

} // end namespace oofem
