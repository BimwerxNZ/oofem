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

#ifndef TR_SHELL03_h
#define TR_SHELL03_h

#include "../sm/Elements/structuralelement.h"
#include "zznodalrecoverymodel.h"
#include "../sm/ErrorEstimators/zzerrorestimator.h"
#include "../sm/Elements/Shells/cct3d.h"
#include "../sm/Elements/PlaneStress/trplanestressrotallman3d.h"
#include "spatiallocalizer.h"

#include <memory>

#define _IFT_TR_SHELL03_Name "TR_SHELL03"

///@name Additional Input fields for TR_SHELL03
//@{
#define _IFT_TR_SHELL03_FirstLocalAxis "lcs1"
#define _IFT_TR_SHELL03_macroElem "macroelem"
//@}

namespace oofem {
/**
 * This class implements an triangular three-node shell finite element, composed of
 * cct3d and trplanrot3d elements.
 * Each node has 6 degrees of freedom.
 *
 * @author Ladislav Svoboda
 */
class TR_SHELL03 : public StructuralElement, public ZZNodalRecoveryModelInterface, public NodalAveragingRecoveryModelInterface, public ZZErrorEstimatorInterface, public SpatialLocalizerInterface
{
protected:
    /// Pointer to plate element.
    std :: unique_ptr< CCTPlate3d > plate;
    /// Pointer to membrane (plane stress) element.
    std::unique_ptr< TrPlanestressRotAllman3d > membrane;
    /**
     * Element integraton rule (plate and membrane parts have their own integration rules)
     * this one used to integrate element error and perhaps can be (re)used for other putrposes.
     * Created on demand.
     */
    std :: unique_ptr< IntegrationRule > compositeIR;

    static IntArray loc_plate;
    static IntArray loc_membrane;
    bool drillType = false; // NF mod - drilling not accounted in for tria
    IntArray drillOrdering = { 6, 12, 18 };
    // 1st local axis
    FloatArray la1;
    // macro element number
    int macroElem;

public:
    /// Constructor
    TR_SHELL03(int n, Domain * d);
    /// Destructor
    virtual ~TR_SHELL03() {}

    virtual FEInterpolation *giveInterpolation() const { return plate->giveInterpolation(); }

    int computeNumberOfDofs() override { return 18; }
    void giveDofManDofIDMask(int inode, IntArray &answer) const override
    { plate->giveDofManDofIDMask(inode, answer); }
    // definition & identification
    const char *giveInputRecordName() const override { return _IFT_TR_SHELL03_Name; }
    const char *giveClassName() const override { return "TR_SHELL03"; }
    void initializeFrom(InputRecord &ir) override;

    void giveCharacteristicVector(FloatArray &answer, CharType mtrx, ValueModeType mode, TimeStep *tStep) override;
    void giveCharacteristicMatrix(FloatMatrix &answer, CharType mtrx, TimeStep *tStep) override;
    double computeVolumeAround(GaussPoint *gp) override;
    bool giveRotationMatrix(FloatMatrix &answer) override;
    int computeLoadGToLRotationMtrx(FloatMatrix &answer);

    void updateYourself(TimeStep *tStep) override;
    void updateInternalState(TimeStep *tStep) override;
    void printOutputAt(FILE *file, TimeStep *tStep) override;
    void saveContext(DataStream &stream, ContextMode mode) override;
    void restoreContext(DataStream &stream, ContextMode mode) override;
    virtual void postInitialize();
    void updateLocalNumbering(EntityRenumberingFunctor &f);
    void setCrossSection(int csIndx);
#ifdef __OOFEG
    virtual void drawRawGeometry(oofegGraphicContext &gc, TimeStep *tStep);
    virtual void drawDeformedGeometry(oofegGraphicContext &gc, TimeStep *tStep, UnknownType type);
    virtual void drawScalar(oofegGraphicContext &gc, TimeStep *tStep);
#endif
    // the membrane and plate irules are same (chacked in initializeFrom)
    int giveDefaultIntegrationRule() const override { return plate->giveDefaultIntegrationRule(); }
    IntegrationRule *giveDefaultIntegrationRulePtr() override { return plate->giveDefaultIntegrationRulePtr(); }
    Element_Geometry_Type giveGeometryType() const override { return EGT_triangle_1; }
    integrationDomain giveIntegrationDomain() const override { return _Triangle; }
    MaterialMode giveMaterialMode() override { return _Unknown; }

    virtual Interface *giveInterface(InterfaceType it);
    int giveIPValue(FloatArray &answer, GaussPoint *gp, InternalStateType type, TimeStep *tStep) override;

    virtual void NodalAveragingRecoveryMI_computeNodalValue(FloatArray &answer, int node,
                                                            InternalStateType type, TimeStep *tStep);

    virtual IntegrationRule *ZZErrorEstimatorI_giveIntegrationRule();
    virtual void ZZErrorEstimatorI_computeLocalStress(FloatArray &answer, FloatArray &sig);

    // SpatialLocalizerI
    virtual void SpatialLocalizerI_giveBBox(FloatArray &bb0, FloatArray &bb1);


    virtual int computeGlobalCoordinates(FloatArray &answer, const FloatArray &lcoords) {
        return this->plate->computeGlobalCoordinates(answer, lcoords);
    }

    virtual bool computeLocalCoordinates(FloatArray &answer, const FloatArray &gcoords) {
        return this->plate->computeLocalCoordinates(answer, gcoords);
    }

protected:
    void computeBmatrixAt(GaussPoint *, FloatMatrix &, int = 1, int = ALL_STRAINS) override
    { OOFEM_ERROR("calling of this function is not allowed"); }
    void computeNmatrixAt(const FloatArray &iLocCoord, FloatMatrix &) override
    { OOFEM_ERROR("calling of this function is not allowed"); }

    /// @todo In time delete
protected:
    virtual void computeGaussPoints()
    {
        this->membrane->computeGaussPoints();
        this->plate->computeGaussPoints();
    }
    void computeStressVector( FloatArray &answer, const FloatArray &strain, GaussPoint *gp, TimeStep *tStep ) override
    { OOFEM_ERROR( "calling of this function is not allowed" ); }
    void computeConstitutiveMatrixAt( FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep ) override
    { OOFEM_ERROR( "calling of this function is not allowed" ); }
    void computeBodyLoadVectorAt(FloatArray &answer, Load *forLoad, TimeStep *tStep, ValueModeType mode) override;
	int computeLoadLEToLRotationMatrix(FloatMatrix &answer, int iEdge, GaussPoint *gp) override;

public:
    void computeStiffnessMatrix( FloatMatrix &answer, MatResponseMode rMode, TimeStep *tStep ) override
    { OOFEM_ERROR("calling of this function is not allowed"); }
    void computeMassMatrix(FloatMatrix &answer, TimeStep *tStep) override
    { OOFEM_ERROR("calling of this function is not allowed"); }
    void giveInternalForcesVector( FloatArray &answer, TimeStep *tStep, int useUpdatedGpRecord ) override
    { OOFEM_ERROR("calling of this function is not allowed"); }
	void computeInitialStressMatrix(FloatMatrix &answer, TimeStep *tStep) override;

private:
    void giveNodeCoordinates(FloatArray& nc1, FloatArray& nc2, FloatArray& nc3);
    void giveLocalCoordinates(FloatArray &answer,const FloatArray &global);
    const FloatMatrix *computeGtoLRotationMatrix();
    virtual void computeSurfaceNMatrix(FloatMatrix &answer, int boundaryID, const FloatArray &lcoords);
    virtual void computeEdgeNMatrix(FloatMatrix &answer, int boundaryID, const FloatArray &lcoords);
    virtual double computeEdgeVolumeAround(GaussPoint *gp, int iEdge);
    virtual double computeSurfaceVolumeAround(GaussPoint *gp, int iSurf);
};
} // end namespace oofem
#endif
