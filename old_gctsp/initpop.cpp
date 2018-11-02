/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * initpop.cpp
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
#include "initpop.hpp"

Initpop::Initpop( unsigned nChrm, unsigned valor, unsigned seed, map < int, set <int> > mapa )
{
    associations = mapa;
    valorPosible = valor;
    sizeChrm = nChrm;
    generator.seed( seed );
}

Initpop::~Initpop( )
{

}

/**
 *
 * Población Inicial con la Nearest Neighbor Heuristic (NNH)
 * (D. Rosenkrantz, R. Stearns and P. Lewis., 1977)
 *
 **/
// void Initpop::NNH(vector <unsigned> &chrm, TSP *tsp1)
// {
//   tsp1->vecinoMasCercano(rnd(0, sizeChrm - 1), chrm);
// }

/**
 *
 * Poblacion inicial Aleatoria
 *
 **/
void Initpop::Random( vector <unsigned> &tour )
{
    for ( int j = 0; j < sizeChrm; j++ )
        tour.push_back( j + 1 );


    shuffle( tour.begin( ), tour.end( ), generator );
}

/**
 *
 * Población Inicial con la Nearest Neighbor Heuristic (NNH)
 * (D. Rosenkrantz, R. Stearns and P. Lewis., 1977)
 *
 **/
void Initpop::NNH( vector <unsigned> &chrm, TSP *tsp1)
{
    tsp1->vecinoMasCercano( rnd( 0, sizeChrm * 0.2 ), chrm);
    //tsp1->vecinoCercanoAlDepot (associations , 0, rnd( 0, sizeChrm - 1) );
}

/**
 * Nearest Neighbor Center
 *
 **/
void Initpop::NNC( vector <unsigned> &chrm, TSP *tsp1)
{
    tsp1->vecinoCercanoAlDepot (chrm, associations , rnd( 0, sizeChrm - 1) );
}
