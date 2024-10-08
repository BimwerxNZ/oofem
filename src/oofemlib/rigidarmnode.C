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

#include "rigidarmnode.h"
#include "slavedof.h"
#include "floatarray.h"
#include "floatmatrix.h"
#include "intarray.h"
#include "classfactory.h"
#include "domain.h"
#include "entityrenumberingscheme.h"

namespace oofem {
REGISTER_DofManager(RigidArmNode);

RigidArmNode :: RigidArmNode(int n, Domain *aDomain) : Node(n, aDomain)
{ }


void
RigidArmNode :: initializeFrom(InputRecord &ir)
{
    Node :: initializeFrom(ir);

    IR_GIVE_FIELD(ir, masterDofMngr, _IFT_RigidArmNode_master);

    IR_GIVE_FIELD(ir, masterMask, _IFT_DofManager_mastermask);
    if ( masterMask.giveSize() != this->dofidmask->giveSize() ) {
        throw ValueInputException(ir, _IFT_DofManager_mastermask, "mastermask size mismatch");
    }
}

void
RigidArmNode :: postInitialize()
{
    Node :: postInitialize();

    // auxiliary arrays
    std::map< DofIDItem, IntArray > masterDofID;
    std::map< DofIDItem, FloatArray > masterContribution;

    this->masterNode = dynamic_cast< Node * >( this->domain->giveDofManager(masterDofMngr) );
    if ( !masterNode ) {
        OOFEM_WARNING("master dofManager is not a node");
    }

    if ( this->masterNode->giveLabel() == this->giveLabel() ) {
        OOFEM_ERROR( "rigidarmnode %d is own master", this->giveLabel() );
    }

    int masterNdofs = masterNode->giveNumberOfDofs();

    IntArray masterNodes(masterNdofs);
    for ( int &nodeNum: masterNodes ) {
        nodeNum = this->masterNode->giveNumber();
    }

    this->computeMasterContribution(masterDofID, masterContribution);

    // initialize slave dofs (inside check of consistency of receiver and master dof)
    for ( Dof *dof: *this ) {
        SlaveDof *sdof = dynamic_cast< SlaveDof * >(dof);
        if ( sdof ) {
            DofIDItem id = sdof->giveDofID();
            sdof->initialize(masterNodes, masterDofID [ id ], masterContribution [ id ]);
        }
    }

#if 0
    // check if master in same mode
    if ( parallel_mode != DofManager_local ) {
        if ( ( * masterNode )->giveParallelMode() != parallel_mode ) {
            OOFEM_WARNING("mismatch in parallel mode of RigidArmNode and master", 1);
            result = 0;
        }
    }
#endif
}


int
RigidArmNode :: checkConsistency()
{
    int result;

    result = Node :: checkConsistency();

    // check if receiver has the same coordinate system as master dofManager
    /*
    if ( !this->hasSameLCS(this->masterNode) ) {
        OOFEM_WARNING("different lcs for master/slave nodes", 1);
        result = 0;
    }*/

    // check if created DOFs (dofType) compatible with mastermask
    for ( int i = 1; i <= this->giveNumberOfDofs(); i++ ) {
        if ( this->masterMask.at(i) && this->dofArray [ i - 1 ]->isPrimaryDof() ) {
            OOFEM_ERROR("incompatible mastermask and doftype data");
        }
    }

    return result;
}


void
RigidArmNode :: computeMasterContribution(std::map< DofIDItem, IntArray > &masterDofID, 
                                          std::map< DofIDItem, FloatArray > &masterContribution)
{
#if 0 // original implementation without support of different LCS in slave and master
    int k;
    IntArray R_uvw(3), uvw(3);
    FloatArray xyz(3);
    int numberOfMasterDofs = masterNode->giveNumberOfDofs();
    //IntArray countOfMasterDofs((int)masterDofID.size());

    // decode of masterMask
    uvw.at(1) = this->dofidmask->findFirstIndexOf(R_u);
    uvw.at(2) = this->dofidmask->findFirstIndexOf(R_v);
    uvw.at(3) = this->dofidmask->findFirstIndexOf(R_w);

    xyz.beDifferenceOf(*this->giveCoordinates(), *masterNode->giveCoordinates());

    if ( hasLocalCS() ) {
        // LCS is stored as global-to-local, so LCS*xyz_glob = xyz_loc
        xyz.rotatedWith(* this->localCoordinateSystem, 'n');
    }

    for ( int i = 1; i <= this->dofidmask->giveSize(); i++ ) {
        Dof *dof = this->giveDofWithID(dofidmask->at(i));
        DofIDItem id = dof->giveDofID();
        masterDofID [ id ].resize(numberOfMasterDofs);
        masterContribution [ id ].resize(numberOfMasterDofs);
        R_uvw.zero();

        switch ( masterMask.at(i) ) {
        case 0: continue;
            break;
        case 1:
            if ( id == D_u ) {
                if ( uvw.at(2) && masterMask.at( uvw.at(2) ) ) {
                    R_uvw.at(3) =  ( ( int ) R_v );
                }

                if ( uvw.at(3) && masterMask.at( uvw.at(3) ) ) {
                    R_uvw.at(2) = -( ( int ) R_w );
                }
            } else if ( id == D_v ) {
                if ( uvw.at(1) && masterMask.at( uvw.at(1) ) ) {
                    R_uvw.at(3) = -( ( int ) R_u );
                }

                if ( uvw.at(3) && masterMask.at( uvw.at(3) ) ) {
                    R_uvw.at(1) =  ( ( int ) R_w );
                }
            } else if ( id == D_w ) {
                if ( uvw.at(1) && masterMask.at( uvw.at(1) ) ) {
                    R_uvw.at(2) =  ( ( int ) R_u );
                }

                if ( uvw.at(2) && masterMask.at( uvw.at(2) ) ) {
                    R_uvw.at(1) = -( ( int ) R_v );
                }
            }

            break;
        default:
            OOFEM_ERROR("unknown value in masterMask");
        }

        //k = ++countOfMasterDofs.at(i);
        k = 1;
        masterDofID [ id ].at(k) = ( int ) id;
        masterContribution [ id ].at(k) = 1.0;

        for ( int j = 1; j <= 3; j++ ) {
            if ( R_uvw.at(j) != 0 ) {
                int sign = R_uvw.at(j) < 0 ? -1 : 1;
                //k = ++countOfMasterDofs.at(i);
                k++;
                masterDofID [ id ].at(k) = sign * R_uvw.at(j);
                masterContribution [ id ].at(k) = sign * xyz.at(j);
            }
        }
        masterDofID [ id ].resizeWithValues(k);
        masterContribution [ id ].resizeWithValues(k);
    }
#else
    // receiver lcs stored in localCoordinateSystem
    // (this defines the transformation from global to local)
    FloatArray xyz(3);
    FloatMatrix TG2L(6,6); // receiver global to receiver local
    FloatMatrix TR(6,6); // rigid arm transformation between receiver global DOFs and Master global DOFs
    FloatMatrix TMG2L(6,6); // master global to local
    FloatMatrix T(6,6); // full transformation for all dofs
    IntArray fullDofMask = {D_u, D_v, D_w, R_u, R_v, R_w};
    bool hasg2l = this->computeL2GTransformation(TG2L, fullDofMask);
    bool mhasg2l = masterNode->computeL2GTransformation(TMG2L, fullDofMask);

    xyz.beDifferenceOf(this->giveCoordinates(), masterNode->giveCoordinates());

    if (xyz.giveSize() < 3) {
      xyz.resizeWithValues(3);
    }
    
    TR.beUnitMatrix();
    TR.at(1,5) =  xyz.at(3);
    TR.at(1,6) = -xyz.at(2);
    TR.at(2,4) = -xyz.at(3);
    TR.at(2,6) =  xyz.at(1);
    TR.at(3,4) =  xyz.at(2);
    TR.at(3,5) = -xyz.at(1);

    if (hasg2l && mhasg2l) {
      FloatMatrix h; 
      h.beTProductOf(TG2L, TR);  // T transforms global master DOfs to local dofs;
      T.beProductOf(h,TMG2L);    // Add transformation to master local c.s.
    } else if (hasg2l) {
      T.beTProductOf(TG2L, TR);  // T transforms global master DOfs to local dofs;
    } else if (mhasg2l) {
      T.beProductOf(TR,TMG2L);   // Add transformation to master local c.s.
    } else {
      T = TR;
    }
    
    // assemble DOF weights for relevant dofs
    for ( int i = 1; i <= this->dofidmask->giveSize(); i++ ) {
      Dof *dof = this->giveDofWithID(dofidmask->at(i));
      DofIDItem id = dof->giveDofID();
      masterDofID [ id ] = *dofidmask;
      masterContribution [ id ].resize(dofidmask->giveSize());
      
      for (int j = 1; j <= this->dofidmask->giveSize(); j++ ) {
          if ( dofidmask->at(j) <= 6 && id <= 6 ) {
              masterContribution [ id ].at(j) = T.at(id, dofidmask->at(j));
          } else if ( dofidmask->findFirstIndexOf(id) == j ) {
              masterContribution [ id ].at(j) = 1;
          }
      }
    }

#endif

}


void RigidArmNode :: updateLocalNumbering(EntityRenumberingFunctor &f)
{
    Node::updateLocalNumbering (f);
    //update masterNode numbering
    this->masterDofMngr = f( this->masterDofMngr, ERS_DofManager );
}


} // end namespace oofem
