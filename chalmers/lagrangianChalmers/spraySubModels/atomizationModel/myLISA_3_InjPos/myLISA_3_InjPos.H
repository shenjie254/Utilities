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

Class
    Foam::myLISA_3_InjPos

Description
    Primary Breakup Model for pressure swirl atomizers.

    Accurate description in
    @verbatim
    P.K. Senecal, D.P. Shmidt, I. Nouar, C.J. Rutland, R.D. Reitz, M. Corradini
    "Modeling high-speed viscous liquid sheet atomization"
    International Journal of Multiphase Flow 25 (1999) pags. 1073-1097
    @endverbatim

    and

    @verbatim
    D.P. Schmidt, I. Nouar, P.K. Senecal, C.J. Rutland, J.K. Martin, R.D. Reitz
    "Pressure-Swirl Atomization in the Near Field"
    SAE Techical Paper Series 1999-01-0496
    @endverbatim


\*---------------------------------------------------------------------------*/

#ifndef myLISA_3_InjPos_H
#define myLISA_3_InjPos_H

#include "atomizationModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class myLISA_3_InjPos Declaration
\*---------------------------------------------------------------------------*/

class myLISA_3_InjPos
:
    public atomizationModel
{
private:

    // Private data

        dictionary coeffsDict_;
        Random& rndGen_;
        scalar Cl_;
        scalar cTau_;
        scalar Q_;
        scalar J_;
        scalar lisaExp_;

public:

    //- Runtime type information
    TypeName("myLISA_3_InjPos");


    // Constructors

        //- Construct from components
        myLISA_3_InjPos
        (
            const dictionary& dict,
            spray& sm
        );


    // Destructor

        ~myLISA_3_InjPos();


    // Member Operators

        void atomizeParcel
        (
            parcel& parcel,
            const scalar deltaT,
            const vector& vel,
            const liquidMixture& fuels
        ) const;


};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
