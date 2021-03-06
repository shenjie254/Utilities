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

#include "BasicSprayParcel.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::BasicSprayParcel<ThermoType>::BasicSprayParcel
(
    SprayCloud<BasicSprayParcel<ThermoType> >& owner,
    const vector& position,
    const label cellI
)
:
    SprayParcel<BasicSprayParcel<ThermoType> >(owner, position, cellI)
{}

template<class ThermoType>
Foam::BasicSprayParcel<ThermoType>::BasicSprayParcel
(
    SprayCloud<BasicSprayParcel<ThermoType> >& owner,
    const vector& position,
    const label cellI,
    const label typeId,
    const scalar nParticle0,
    const scalar d0,
    const vector& U0,
    const scalarField& Y0,
    const scalar& dInit,
    const scalar& liquidCore,
    const scalar& KHindex,
    const scalar& y,
    const scalar& yDot,
    const scalar& tc,
    const scalar& ms,
    const scalar& injector,
    const scalar& tMom,
    const scalar& user,
    const typename SprayParcel<BasicSprayParcel<ThermoType> >::
        constantProperties& constProps
)
:
    SprayParcel<BasicSprayParcel<ThermoType> >
    (
        owner,
        position,
        cellI,
        typeId,
        nParticle0,
        d0,
        U0,
        Y0,
        dInit,
        liquidCore,
        KHindex,
        y,
        yDot,
        tc,
        ms,
        injector,
        tMom,
        user,
        constProps
    )
{}

template<class ThermoType>
Foam::BasicSprayParcel<ThermoType>::BasicSprayParcel
(
    const Cloud<BasicSprayParcel<ThermoType> >& cloud,
    Istream& is,
    bool readFields
)
:
    SprayParcel<BasicSprayParcel<ThermoType> >(cloud, is, readFields)
{}


template<class ThermoType>
Foam::BasicSprayParcel<ThermoType>::BasicSprayParcel
(
    const BasicSprayParcel<ThermoType>& p
)
:
    SprayParcel<BasicSprayParcel>(p)
{}


// * * * * * * * * * * * * * * * *  Destructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::BasicSprayParcel<ThermoType>::~BasicSprayParcel()
{}



// ************************************************************************* //
