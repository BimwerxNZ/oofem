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

#ifndef assemblercallback_h
#define assemblercallback_h

#include "valuemodetype.h" ///@todo We shouldn't have this for assembling vectors or matrices(!) / Mikael
#include "matresponsemode.h"
#include "chartype.h"

namespace oofem {
class IntArray;
class FloatArray;
class FloatMatrix;
class Element;
class DofManager;
class TimeStep;
class NodalLoad;
class BodyLoad;
class BoundaryLoad;
class UnknownNumberingScheme;
class SparseMtrx;
class ActiveBoundaryCondition;

/**
 * Callback class for assembling specific types of vectors.
 * Default implementations are that no contributions are considered (empty vectors on output).
 * @author Mikael Öhman
 */
class VectorAssembler
{
public:
    virtual void vectorFromElement(FloatArray &vec, Element &element, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromLoad(FloatArray &vec, Element &element, BodyLoad *load, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromBoundaryLoad(FloatArray &vec, Element &element, BoundaryLoad *load, int boundary, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromEdgeLoad(FloatArray &vec, Element &element, BoundaryLoad *load, int edge, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromNodeLoad(FloatArray &vec, DofManager &dman, NodalLoad *load, TimeStep *tStep, ValueModeType mode) const;

    /// Default implementation takes all the DOF IDs
    virtual void locationFromElement(IntArray &loc, Element &element, const UnknownNumberingScheme &s, IntArray *dofIds = nullptr) const;
    /// Default implementation takes all the DOF IDs
    virtual void locationFromElementNodes(IntArray &loc, Element &element, const IntArray &bNodes, const UnknownNumberingScheme &s, IntArray *dofIds = nullptr) const;
};

/**
 * Callback class for assembling specific types of matrices
 * @author Mikael Öhman
 */
class MatrixAssembler
{
public:
    virtual void matrixFromElement(FloatMatrix &mat, Element &element, TimeStep *tStep) const;
    virtual void matrixFromLoad(FloatMatrix &mat, Element &element, BodyLoad *load, TimeStep *tStep) const;
    virtual void matrixFromBoundaryLoad(FloatMatrix &mat, Element &element, BoundaryLoad *load, int boundary, TimeStep *tStep) const;
    virtual void matrixFromEdgeLoad(FloatMatrix &mat, Element &element, BoundaryLoad *load, int edge, TimeStep *tStep) const;
    virtual void assembleFromActiveBC(SparseMtrx &k, ActiveBoundaryCondition &bc, TimeStep* tStep, const UnknownNumberingScheme &s_r, const UnknownNumberingScheme &s_c) const;

    virtual void locationFromElement(IntArray &loc, Element &element, const UnknownNumberingScheme &s, IntArray *dofIds = nullptr) const;
    virtual void locationFromElementNodes(IntArray &loc, Element &element, const IntArray &bNodes, const UnknownNumberingScheme &s, IntArray *dofIds = nullptr) const;
};


class OldVectorAssembler : public VectorAssembler
{
private:
    CharType type;

public:
    OldVectorAssembler(CharType t);
    virtual void vectorFromElement(FloatArray& vec, Element& element, TimeStep* tStep, ValueModeType mode) const;
    virtual void vectorFromLoad(FloatArray& vec, Element& element, BodyLoad* load, TimeStep* tStep, ValueModeType mode) const;
    virtual void vectorFromBoundaryLoad(FloatArray& vec, Element& element, BoundaryLoad* load, int boundary, TimeStep* tStep, ValueModeType mode) const;
    virtual void vectorFromEdgeLoad(FloatArray& vec, Element& element, BoundaryLoad* load, int edge, TimeStep* tStep, ValueModeType mode) const;
    virtual void vectorFromNodeLoad(FloatArray& vec, DofManager& dman, NodalLoad* load, TimeStep* tStep, ValueModeType mode) const;
};

/**
 * Implementation for assembling internal forces vectors in standard monolithic, nonlinear FE-problems
 * @author Mikael Öhman
 */
class InternalForceAssembler : public VectorAssembler
{
public:
    virtual void vectorFromElement(FloatArray &vec, Element &element, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromLoad(FloatArray &vec, Element &element, BodyLoad *load, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromBoundaryLoad(FloatArray &vec, Element &element, BoundaryLoad *load, int boundary, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromEdgeLoad(FloatArray &vec, Element &element, BoundaryLoad *load, int edge, TimeStep *tStep, ValueModeType mode) const;
};

/**
 * Implementation for assembling external forces vectors in standard monolithic FE-problems
 * @author Mikael Öhman
 */
class ExternalForceAssembler : public VectorAssembler
{
public:
    virtual void vectorFromLoad(FloatArray &vec, Element &element, BodyLoad *load, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromBoundaryLoad(FloatArray &vec, Element &element, BoundaryLoad *load, int boundary, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromEdgeLoad(FloatArray &vec, Element &element, BoundaryLoad *load, int edge, TimeStep *tStep, ValueModeType mode) const;
    virtual void vectorFromNodeLoad(FloatArray &vec, DofManager &dman, NodalLoad *load, TimeStep *tStep, ValueModeType mode) const;
};

/**
 * Implementation for assembling tangent matrices in standard monolithic FE-problems
 * @author Mikael Öhman
 */
class TangentAssembler : public MatrixAssembler
{
protected:
    ///@todo This is more general than just material responses; we should make a "TangentType"
    MatResponseMode rmode;
    
public:
    TangentAssembler(MatResponseMode m): MatrixAssembler(), rmode(m) {}

    virtual void matrixFromElement(FloatMatrix &mat, Element &element, TimeStep *tStep) const;
    virtual void matrixFromLoad(FloatMatrix &mat, Element &element, BodyLoad *load, TimeStep *tStep) const;
    virtual void matrixFromBoundaryLoad(FloatMatrix &mat, Element &element, BoundaryLoad *load, int boundary, TimeStep *tStep) const;
    virtual void matrixFromEdgeLoad(FloatMatrix &mat, Element &element, BoundaryLoad *load, int edge, TimeStep *tStep) const;
    virtual void assembleFromActiveBC(SparseMtrx &k, ActiveBoundaryCondition &bc, TimeStep* tStep, const UnknownNumberingScheme &s_r, const UnknownNumberingScheme &s_c) const;
};


class OldMatrixAssembler : public MatrixAssembler
{
private:
    CharType type;

public:
    OldMatrixAssembler(CharType t) : MatrixAssembler(), type(t) {}
    virtual void matrixFromElement(FloatMatrix &mat, Element &element, TimeStep *tStep) const;
    virtual void matrixFromLoad(FloatMatrix &mat, Element &element, BodyLoad *load, TimeStep *tStep) const;
    virtual void matrixFromBoundaryLoad(FloatMatrix &mat, Element &element, BoundaryLoad *load, int boundary, TimeStep *tStep) const;
    virtual void matrixFromEdgeLoad(FloatMatrix &mat, Element &element, BoundaryLoad *load, int edge, TimeStep *tStep) const;
    virtual void assembleFromActiveBC(SparseMtrx &k, ActiveBoundaryCondition &bc, TimeStep* tStep, const UnknownNumberingScheme &s_r, const UnknownNumberingScheme &s_c) const;
};

#if 0
/**
 * Implementation for assembling the consistent mass matrix
 * @author Mikael Öhman
 */
class MassMatrixAssembler : public MatrixAssembler
{
public:
    virtual void matrixFromElement(FloatMatrix &mat, Element &element, TimeStep *tStep) const;
};
#endif

}
#endif // assemblercallback_h