/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * genetic.hpp
 * This file is part of VCP-PGB
 *
 * Copyright (C) 2010 - Carlos Contreras Bolton <ccontreras.bolton@gmail.com>
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

#ifndef _CROSSOVER_H
#define	_CROSSOVER_H

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include "util.hpp"
#include "selection.hpp"
#include "tsp.hpp"
#include "auxiliares.hpp"

using namespace std;

class Crossover : public Util {
public:
    unsigned sizeChrm;
    unsigned sizePop;
    unsigned seed;
    float pcross; /* probability of crossover */
    map < int, set <int> > associations;
    Crossover( int sizeChrm, int nPop, unsigned seed, map < int, set <int> > associations );
    virtual ~Crossover( );
    // Funciones auxiliares
    void setSizeChrm( unsigned nChrm );
    void auxPutDepotBegin(vector <unsigned> &child1);
    bool existenDesMarcados( vector <bool> &marca );
    // Funciones operadores
    bool PMX( vector <unsigned> &child1, vector <unsigned> &child2 );                       // 0
    bool OX1( vector <unsigned> &child1, vector <unsigned> &child2 );                       // 1
    bool OX2( vector <unsigned> &child1, vector <unsigned> child2 );                        // 2
    bool MOX( vector <unsigned> &child1, vector <unsigned> child2 );                        // 3
    bool POS( vector <unsigned> &child1, vector <unsigned> child2 );                        // 4
    bool CX( vector <unsigned> &child1, vector <unsigned> &child2 );                        // 5
    bool DPX( vector <unsigned> &child1, vector <unsigned> child2, TSP *tsp1 );             // 6
    bool AP( vector <unsigned> &child1, vector <unsigned> &child2 );                        // 7
    bool MPX( vector <unsigned> &child1, vector <unsigned> child2 );                        // 8
    bool HX( vector <unsigned> &child1, vector <unsigned> child2, TSP *tsp1 );              // 9
    bool IO( vector <unsigned> &child1, individuos *pop, TSP *tsp1 );                       // 10
    bool MIO( vector <unsigned> &child1, individuos *pop, TSP *tsp1, int maxgen, int gen ); // 11
    bool VR( vector <unsigned> &child1, vector <unsigned> child2, individuos *pop );        // 12
    bool adhoc( vector <unsigned> &child1, vector <unsigned> child2, TSP *tsp1 );           // 13
    bool ER( vector <unsigned> &child1, vector <unsigned> child2 );                         // 14
    bool GSTX( vector <unsigned> &child11, vector <unsigned> child2 );                      // 15
    bool adhoc2( vector <unsigned> &child1, vector <unsigned> child2, TSP *tsp1 );           // 13
private:

};

#endif	/* _CROSSOVER_H */
