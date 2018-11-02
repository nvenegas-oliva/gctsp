/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/*
 * selection.hpp
 * This file is part of AG-GCTSP
 *
 * Copyright (C) 2013 - Carlos Contreras Bolton <ccontreras.bolton@gmail.com>
 *
 * VCP-PGB is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * VCP-PGB is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with VCP-PGB; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "selection.hpp"

Selection::Selection( unsigned npop, unsigned seed )
{
    sizePop = npop;
    generator.seed( seed );
}

Selection::~Selection( )
{

}

template<template <typename> class P = std::less >
struct compara_pair_second
{

    template<class T1, class T2> bool operator()( const std::pair<T1, T2>& left,
            const std::pair<T1, T2>& right )
    {
        return P<T2>( )( left.second, right.second );
    }
};

/**
 *
 * Selección de Torneo K
 *
 **/
unsigned Selection::TorneoK( individuos *pop, int k )
{
    unsigned i;
    vector<pair <unsigned, double > > torneo;

    for ( int j = 0; j < k; j++ ) {
        i = rnd( 0, sizePop - 1 );
        pair <int, double> ind( i, pop[i].fitness );
        torneo.push_back( ind );
    }
    sort( torneo.begin( ), torneo.end( ), compara_pair_second<std::less>( ) );

    return torneo[0].first;
}

/**
 *
 * roulette-wheel selection que considera variables del tipo float
 *
 **/
unsigned Selection::Ruleta( individuos *pop )
{
    // int i;
    // double sumfitness = 0.0;
    // for(int j = 0; j < sizePop; j++)
    //   sumfitness += (((double) 1.0) /(double) pop[j].fitness);

    // double pick = generate_canonical<double, numeric_limits<double>::digits>(generator);
    // double sum = 0.0;

    // if(sumfitness != 0.0)
    //   for(i = 0; (sum < pick) && (i < sizePop); i++)
    //     sum += (double) (((double) 1.0) /(double) pop[i].fitness)/((double) sumfitness);
    // else
    //   i = rnd(0, sizePop - 1);

    // return i-1;
    return true;
}
