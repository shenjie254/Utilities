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

#include "parcel.H"
#include "IOstreams.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::parcel::parcel
(
    const Cloud<parcel>& cloud,
    Istream& is,
    bool readFields
)
:
    Particle<parcel>(cloud, is, readFields),

    liquidComponents_
    (
        (cloud.pMesh().lookupObject<dictionary>("thermophysicalProperties"))
       .lookup("liquidComponents")
    ),
    X_(liquidComponents_.size(), 0.0),

    tMom_(GREAT)
{

    label nX = X_.size();

    if (readFields)
    {
        if (is.format() == IOstream::ASCII)
        {
            d_ = readScalar(is);
//---------------------------------CH 101101-------------------------------//
            d0_ = readScalar(is);
//-----------------------------------END-----------------------------------//
            T_ = readScalar(is);
            m_ = readScalar(is);
//---------------------------------CH 101202-------------------------------//
            pIndKH_ = readScalar(is);
//-----------------------------------END-----------------------------------//
            y_ = readScalar(is);
            yDot_ = readScalar(is);
            ct_ = readScalar(is);
            ms_ = readScalar(is);
            tTurb_ = readScalar(is);
            liquidCore_ = readScalar(is);
            injector_ = readScalar(is);
            is >> U_;
            is >> Uturb_;
            is >> n_;
            for (label j=0; j<nX; j++)
            {
                X_[j] = readScalar(is);
            }
        }
        else
        {
            is.read
            (
                reinterpret_cast<char*>(&d_),
                sizeof(d_) + sizeof(T_) + sizeof(m_) + sizeof(y_)
              + sizeof(yDot_) + sizeof(ct_) + sizeof(ms_) + sizeof(tTurb_)
              + sizeof(liquidCore_) + sizeof(injector_)
              + sizeof(U_) + sizeof(Uturb_) + sizeof(n_)
//---------------------------------CH 101101-------------------------------//
              +  sizeof(d0_) + sizeof(pIndKH_)
//-----------------------------------END-----------------------------------//
            );

            is.read
            (
                reinterpret_cast<char*>(X_.begin()),
                X_.size()*sizeof(scalar)
            );
        }
    }

    // Check state of Istream
    is.check("parcel::parcel(Istream&)");
}


void Foam::parcel::readFields
(
    Cloud<parcel>& c
)
{
    if (!c.size())
    {
        return;
    }

    Particle<parcel>::readFields(c);

    IOField<scalar> d(c.fieldIOobject("d", IOobject::MUST_READ));
    c.checkFieldIOobject(c, d);
//------------------------------CH 101101----------------------------------//
    IOField<scalar> d0(c.fieldIOobject("d0", IOobject::MUST_READ));
    c.checkFieldIOobject(c, d0);
//----------------------------------END------------------------------------//
    IOField<scalar> T(c.fieldIOobject("T", IOobject::MUST_READ));
    c.checkFieldIOobject(c, T);

    IOField<scalar> m(c.fieldIOobject("m", IOobject::MUST_READ));
    c.checkFieldIOobject(c, m);
//------------------------------CH 101202----------------------------------//
    IOField<scalar> pIndKH(c.fieldIOobject("pIndKH", IOobject::MUST_READ));
    c.checkFieldIOobject(c, pIndKH);
//----------------------------------END------------------------------------//
    IOField<scalar> y(c.fieldIOobject("y", IOobject::MUST_READ));
    c.checkFieldIOobject(c, y);

    IOField<scalar> yDot(c.fieldIOobject("yDot", IOobject::MUST_READ));
    c.checkFieldIOobject(c, yDot);

    IOField<scalar> ct(c.fieldIOobject("ct", IOobject::MUST_READ));
    c.checkFieldIOobject(c, ct);

    IOField<scalar> ms(c.fieldIOobject("ms", IOobject::MUST_READ));
    c.checkFieldIOobject(c, ms);

    IOField<scalar> tTurb(c.fieldIOobject("tTurb", IOobject::MUST_READ));
    c.checkFieldIOobject(c, tTurb);

    IOField<scalar> liquidCore
    (
        c.fieldIOobject("liquidCore", IOobject::MUST_READ)
    );
    c.checkFieldIOobject(c, liquidCore);

    IOField<scalar> injector(c.fieldIOobject("injector", IOobject::MUST_READ));
    c.checkFieldIOobject(c, injector);

    IOField<vector> U(c.fieldIOobject("U", IOobject::MUST_READ));
    c.checkFieldIOobject(c, U);

    IOField<vector> Uturb(c.fieldIOobject("Uturb", IOobject::MUST_READ));
    c.checkFieldIOobject(c, Uturb);

    IOField<vector> n(c.fieldIOobject("n", IOobject::MUST_READ));
    c.checkFieldIOobject(c, n);

    label i = 0;
    forAllIter(Cloud<parcel>, c, iter)
    {
        parcel& p = iter();

        p.d_ = d[i];
//------------------------------CH 101101----------------------------------//
        p.d0_ = d0[i];
//----------------------------------END------------------------------------//
        p.T_ = T[i];
        p.m_ = m[i];
//------------------------------CH 101202----------------------------------//
        p.pIndKH_ = pIndKH[i];
//----------------------------------END------------------------------------//
        p.y_ = y[i];
        p.yDot_ = yDot[i];
        p.ct_ = ct[i];
        p.ms_ = ms[i];
        p.tTurb_ = tTurb[i];
        p.liquidCore_ = liquidCore[i];
        p.injector_ = injector[i];

        p.U_ = U[i];
        p.Uturb_ = Uturb[i];
        p.n_ = n[i];

        i++;
    }

    // read the liquid molar fractions
    if (c.size())
    {
        Cloud<parcel>::const_iterator iter = c.begin();
        const parcel& p0 = iter();

        label nX = p0.X().size();
        const List<word>& names = p0.liquidNames();

        for (label j=0; j<nX; j++)
        {
            IOField<scalar> X(c.fieldIOobject(names[j], IOobject::MUST_READ));

            label i = 0;
            forAllIter(Cloud<parcel>, c, iter)
            {
                parcel& p = iter();
                p.X_[j] = X[i++];
            }
        }
    }
}


void Foam::parcel::writeFields
(
    const Cloud<parcel>& c
)
{
    Particle<parcel>::writeFields(c);

    label np = c.size();

    IOField<scalar> d(c.fieldIOobject("d", IOobject::NO_READ), np);
//------------------------------CH 101101----------------------------------//
    IOField<scalar> d0(c.fieldIOobject("d0", IOobject::NO_READ), np);
//----------------------------------END------------------------------------//
    IOField<scalar> T(c.fieldIOobject("T", IOobject::NO_READ), np);
    IOField<scalar> m(c.fieldIOobject("m", IOobject::NO_READ), np);
//------------------------------CH 101202----------------------------------//
    IOField<scalar> pIndKH(c.fieldIOobject("pIndKH", IOobject::NO_READ), np);
//----------------------------------END------------------------------------//
    IOField<scalar> y(c.fieldIOobject("y", IOobject::NO_READ), np);
    IOField<scalar> yDot(c.fieldIOobject("yDot", IOobject::NO_READ), np);
    IOField<scalar> ct(c.fieldIOobject("ct", IOobject::NO_READ), np);
    IOField<scalar> ms(c.fieldIOobject("ms", IOobject::NO_READ), np);
    IOField<scalar> tTurb(c.fieldIOobject("tTurb", IOobject::NO_READ), np);
    IOField<scalar> liquidCore
    (
        c.fieldIOobject("liquidCore", IOobject::NO_READ),
        np
    );
    IOField<scalar> injector
    (
        c.fieldIOobject("injector", IOobject::NO_READ),
        np
    );
    IOField<vector> U(c.fieldIOobject("U", IOobject::NO_READ), np);
    IOField<vector> Uturb(c.fieldIOobject("Uturb", IOobject::NO_READ), np);
    IOField<vector> n(c.fieldIOobject("n", IOobject::NO_READ), np);

    label i = 0;
    forAllConstIter(Cloud<parcel>, c, iter)
    {
        const parcel& p = iter();

        d[i] = p.d_;
//------------------------------CH 101101----------------------------------//
        d0[i] = p.d0_;
//----------------------------------END------------------------------------//
        T[i] = p.T_;
        m[i] = p.m_;
//------------------------------CH 101202----------------------------------//
        pIndKH[i] = p.pIndKH_;
//----------------------------------END------------------------------------//
        y[i] = p.y_;
        yDot[i] = p.yDot_;
        ct[i] = p.ct_;
        ms[i] = p.ms_;
        tTurb[i] = p.tTurb_;
        liquidCore[i] = p.liquidCore_;
        injector[i] = p.injector_;

        U[i] = p.U_;
        Uturb[i] = p.Uturb_;
        n[i] = p.n_;

        i++;
    }

    d.write();
//------------------------------CH 101101----------------------------------//
    d0.write();
//----------------------------------END------------------------------------//
    T.write();
    m.write();
//------------------------------CH 101202----------------------------------//
    pIndKH.write();
//----------------------------------END------------------------------------//
    y.write();
    yDot.write();
    ct.write();
    ms.write();
    tTurb.write();
    liquidCore.write();
    injector.write();

    U.write();
    Uturb.write();
    n.write();

    // write the liquid molar fractions
    if (np > 0)
    {
        Cloud<parcel>::const_iterator iter = c.begin();
        const parcel& p0 = iter();

        label nX = p0.X().size();
        const List<word>& names = p0.liquidNames();

        for (label j=0; j<nX; j++)
        {
            IOField<scalar> X(c.fieldIOobject(names[j], IOobject::NO_READ), np);

            label i = 0;
            forAllConstIter(Cloud<parcel>, c, iter)
            {
                const parcel& p = iter();
                X[i++] = p.X()[j];
            }

            X.write();
        }
    }
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

Foam::Ostream& Foam::operator<<(Ostream& os, const parcel& p)
{

    if (os.format() == IOstream::ASCII)
    {
        os  << static_cast<const Particle<parcel>&>(p)
            << token::SPACE << p.d_
//------------------------------CH 101101----------------------------------//
            << token::SPACE << p.d0_
//----------------------------------END------------------------------------//
            << token::SPACE << p.T_
            << token::SPACE << p.m_
//------------------------------CH 101202----------------------------------//
            << token::SPACE << p.pIndKH_
//----------------------------------END------------------------------------//
            << token::SPACE << p.y_
            << token::SPACE << p.yDot_
            << token::SPACE << p.ct_
            << token::SPACE << p.ms_
            << token::SPACE << p.tTurb_
            << token::SPACE << p.liquidCore_
            << token::SPACE << p.injector_
            << token::SPACE << p.U_
            << token::SPACE << p.Uturb_
            << token::SPACE << p.n_
            << token::SPACE << p.X_;
    }
    else
    {
        os  << static_cast<const Particle<parcel>&>(p);
        os.write
        (
            reinterpret_cast<const char*>(&p.d_),
            sizeof(p.d_) + sizeof(p.T_) + sizeof(p.m_) + sizeof(p.y_)
          + sizeof(p.yDot_) + sizeof(p.ct_) + sizeof(p.ms_) + sizeof(p.tTurb_)
          + sizeof(p.liquidCore_) + sizeof(p.injector_)
          + sizeof(p.U_) + sizeof(p.Uturb_) + sizeof(p.n_)
//------------------------------CH 101202----------------------------------//
          + sizeof(p.d0_) + sizeof(p.pIndKH_) 
//----------------------------------END------------------------------------//
        );

        os.write
        (
            reinterpret_cast<const char*>(p.X_.begin()),
            p.X_.size()*sizeof(scalar)
        );
    }

    // Check state of Ostream
    os.check("Ostream& operator<<(Ostream&, const parcel&)");

    return os;
}


// ************************************************************************* //
