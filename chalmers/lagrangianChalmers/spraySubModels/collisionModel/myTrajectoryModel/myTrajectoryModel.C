/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2009 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "myTrajectoryModel.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(myTrajectoryCollisionModel, 0);

addToRunTimeSelectionTable
(
    collisionModel,
    myTrajectoryCollisionModel,
    dictionary
);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
myTrajectoryCollisionModel::myTrajectoryCollisionModel
(
    const dictionary& dict,
    spray& sm,
    Random& rndGen
)
:
    collisionModel(dict, sm, rndGen),
    coeffsDict_(dict.subDict(typeName + "Coeffs")),
    cSpace_(readScalar(coeffsDict_.lookup("cSpace"))),
    cTime_(readScalar(coeffsDict_.lookup("cTime"))),
    coalescence_(coeffsDict_.lookup("coalescence"))

{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

myTrajectoryCollisionModel::~myTrajectoryCollisionModel()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void myTrajectoryCollisionModel::collideParcels(const scalar dt) const
{
    if (spray_.size() < 2)
    {
        return;
    }

    spray::iterator secondParcel = spray_.begin();
    ++secondParcel;
    spray::iterator p1 = secondParcel;

    while (p1 != spray_.end())
    {
        spray::iterator p2 = spray_.begin();

        while (p2 != p1)
        {
#           include "myTrajectoryCM.H"

            // remove coalesced droplets
            if (p2().m() < VSMALL) 
            {
                spray::iterator tmpElmnt = p2;
                ++tmpElmnt;
                spray_.deleteParticle(p2());
                p2 = tmpElmnt;
            }
            else 
            {
                ++p2;
            }

        } // end - inner loop

        // remove coalesced droplets
        if (p1().m() < VSMALL)
        {
            spray::iterator tmpElmnt = p1;
            ++tmpElmnt;
            spray_.deleteParticle(p1());
            p1 = tmpElmnt;
        }
        else
        {
            ++p1;
        }
    } // end - outer loop

} // end


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
