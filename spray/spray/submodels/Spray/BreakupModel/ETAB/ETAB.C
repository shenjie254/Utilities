/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2009-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "ETAB.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class CloudType>
Foam::ETAB<CloudType>::ETAB
(
    const dictionary& dict,
    CloudType& owner
)
:
    BreakupModel<CloudType>(dict, owner, typeName),
    coeffsDict_(dict.subDict(typeName + "Coeffs")),
    Cmu_(readScalar(coeffsDict_.lookup("Cmu"))),
    Comega_(readScalar(coeffsDict_.lookup("Comega"))),
    k1_(readScalar(coeffsDict_.lookup("k1"))),
    k2_(readScalar(coeffsDict_.lookup("k2"))),
    WeCrit_(readScalar(coeffsDict_.lookup("WeCrit"))),
    WeTransition_(readScalar(coeffsDict_.lookup("WeTransition"))),
    AWe_(0.0)
{
    scalar k21 = k2_/k1_;
    AWe_ = (k21*sqrt(WeTransition_) - 1.0)/pow(WeTransition_, 4.0);

    if (!BreakupModel<CloudType>::solveOscillationEq_)
    {
        Info << "Warning: solveOscillationEq is set to " << BreakupModel<CloudType>::solveOscillationEq_ 
             << endl
             << " Setting it to true in order for the ETAB model to work." << endl;
        BreakupModel<CloudType>::solveOscillationEq_ = true;
    }

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template <class CloudType>
Foam::ETAB<CloudType>::~ETAB()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
bool Foam::ETAB<CloudType>::update
(
    const scalar& dt,
    const vector& g,
    scalar& d,
    scalar& tc,
    scalar& ms,
    scalar& nParticle,
    scalar& KHindex,
    scalar& y,
    scalar& yDot,
    const scalar& d0,
    const scalar& rho,
    const scalar& mu,
    const scalar& sigma,
    const vector& U,
    const scalar& rhoc,
    const scalar& muc,
    const vector& Urel,
    const scalar& Urmag,
    const scalar& tMom,
    const scalar& averageParcelMass,
    scalar& dChild,
    scalar& massChild,
    Random& rndGen
) const
{
    scalar r  = 0.5*d;
    scalar r2 = r*r;
    scalar r3 = r*r2;

    scalar semiMass = nParticle*pow(d, 3);

    // inverse of characteristic viscous damping time
    scalar rtd = 0.5*Cmu_*mu/(rho*r2);

    // oscillation frequency (squared)
    scalar omega2 = Comega_*sigma/(rho*r3) - rtd*rtd;

    if (omega2 > 0)
    {
        scalar omega = sqrt(omega2);
        scalar romega = 1.0/omega;

        scalar We = rhoc*pow(Urmag, 2.0)*r/sigma;
        scalar Wetmp = We/WeCrit_;

        scalar y1 = y - Wetmp;
        scalar y2 = yDot*romega;

        scalar a = sqrt(y1*y1 + y2*y2);

        // scotty we may have break-up
        if (a+Wetmp > 1.0)
        {
            scalar phic = y1/a;

            // constrain phic within -1 to 1
            phic = max(min(phic, 1), -1);

            scalar phit = acos(phic);
            scalar phi = phit;
            scalar quad = -y2/a;
            if (quad < 0)
            {
                phi = 2*mathematicalConstant::pi - phit;
            }

            scalar tb = 0;

            if (mag(y) < 1.0)
            {
                scalar theta = acos((1.0 - Wetmp)/a);

                if (theta < phi)
                {
                    if (2*mathematicalConstant::pi-theta >= phi)
                    {
                        theta = -theta;
                    }
                    theta += 2*mathematicalConstant::pi;
                }
                tb = (theta-phi)*romega;

                // breakup occurs
                if (dt > tb)
                {
                    y = 1.0;
                    yDot = -a*omega*sin(omega*tb + phi);
                }
            }

            // update droplet size
            if (dt > tb)
            {
                scalar sqrtWe = AWe_*pow(We, 4.0) + 1.0;
                scalar Kbr = k1_*omega*sqrtWe;

                if (We > WeTransition_)
                {
                    sqrtWe = sqrt(We);
                    Kbr =k2_*omega*sqrtWe;
                }

                scalar rWetmp = 1.0/Wetmp;
                scalar cosdtbu = max(-1.0, min(1.0, 1.0-rWetmp));
                scalar dtbu = romega*acos(cosdtbu);
                scalar decay = exp(-Kbr*dtbu);

                scalar rNew = decay*r;
                if (rNew < r)
                {
                    d = 2.0*rNew;
                    y = 0.0;
                    yDot = 0.0;
                }
            }
        }
    }
    else
    {
        // reset droplet distortion parameters
        y = 0;
        yDot = 0;
    }

    // update the nParticle count to conserve mass
    nParticle = semiMass/pow(d, 3);

    // Do not add child parcel
    return false;
}


// ************************************************************************* //
