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

#ifndef fei2dquadlin_h
#define fei2dquadlin_h

#include "feinterpol2d.h"

namespace oofem {
/**
 * Class representing a 2d isoparametric linear interpolation based on natural coordinates
 * for quadrilateral elements.
 */
class OOFEM_EXPORT FEI2dQuadLin : public FEInterpolation2d
{
public:
    FEI2dQuadLin(int ind1, int ind2) : FEInterpolation2d(1, ind1, ind2) { }

    integrationDomain giveIntegrationDomain() const override { return _Square; }
    Element_Geometry_Type giveGeometryType() const override { return EGT_quad_1; }
    integrationDomain giveBoundaryIntegrationDomain(int ib) const override { return _Line; }
    integrationDomain giveBoundarySurfaceIntegrationDomain(int isurf) const override { return _Square; }
    integrationDomain giveBoundaryEdgeIntegrationDomain(int iedge) const override { return _Line; }

    double giveArea(const FEICellGeometry &cellgeo) const override;

    // Bulk
    void evalN(FloatArray &answer, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    double evaldNdx(FloatMatrix &answer, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    void local2global(FloatArray &answer, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    int global2local(FloatArray &answer, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    int giveNumberOfNodes() const override { return 4; }

    bool inside(const FloatArray &lcoords) const override;

    // Edge
    void computeLocalEdgeMapping(IntArray &edgeNodes, int iedge) override;
    int giveNumberOfEdges() const override { return 4; }
    void edgeEvalN(FloatArray &answer, int iedge, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    double edgeEvalNormal(FloatArray &answer, int iedge, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    void edgeEvaldNds(FloatArray &answer, int iedge, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    void edgeLocal2global(FloatArray &answer, int iedge, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    double evalNXIntegral(int iEdge, const FEICellGeometry &cellgeo) override;

    IntegrationRule *giveIntegrationRule(int order) override;

    void evaldNdxi(FloatMatrix &answer, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;

protected:
    double edgeComputeLength(IntArray &edgeNodes, const FEICellGeometry &cellgeo);
};

/**
 * Class representing a 2d isoparametric linear interpolation based on natural coordinates
 * for quadrilateral elements in axisymmetric setting.
 */
class OOFEM_EXPORT FEI2dQuadLinAxi : public FEI2dQuadLin
{
public:
    FEI2dQuadLinAxi(int ind1, int ind2) : FEI2dQuadLin(ind1, ind2) { }

    double giveTransformationJacobian(const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    double boundaryEdgeGiveTransformationJacobian(int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    double boundaryGiveTransformationJacobian(int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
    double edgeGiveTransformationJacobian(int iedge, const FloatArray &lcoords, const FEICellGeometry &cellgeo) override;
};

} // end namespace oofem
#endif // fei2dquadlin_h
