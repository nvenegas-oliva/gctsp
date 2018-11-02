/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * mutation.hpp
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

#ifndef _MUTATION_H
#define	_MUTATION_H

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstring>
#include "util.hpp"
#include "auxiliares.hpp"
#include "tsp.hpp"

using namespace std;

// LKH
extern "C" int lkh_dll(int argc, char *argv[], int *MejorTour, float **coordenadas, int n);

class Mutation : public Util {
public:
    int sizeChrm;
    float pmut; /* probability of mutation */
    vector<unsigned> fijo;
    map < int, set <int> > associations; // Mapa con los customers por supliers
    Mutation( int largo, float pmuta, unsigned seed, map < int, set <int> > mapa);
    virtual ~Mutation( );
    //Funciones auxiliares
    void auxPutDepotBegin(vector <unsigned> &child1);
    void setSizeChrm( unsigned nChrm );
    //normales
    bool EM2(vector <unsigned> &child1, TSP *tsp1, int n);
    bool EM( vector <unsigned> &child1 );                         // 0
    bool SIM( vector <unsigned> &child1 );                        // 1
    bool SM( vector <unsigned> &child1 );                         // 2
    bool DM( vector <unsigned> &child1 );                         // 3
    bool IVM( vector <unsigned> &child1 );                        // 4
    bool ISM( vector <unsigned> &child1, int n );                 // 5
    bool DBM( vector <unsigned> &child1 );                        // 6
    //new
    bool LS1( vector <unsigned> &child1, TSP *tsp1, int n );      // 7
    bool LS2( vector <unsigned> &child1, TSP *tsp1, int n );      // 8
    bool LS3( vector <unsigned> &child1, TSP *tsp1, int n );      // 9
    bool LS4( vector <unsigned> &child1, TSP *tsp1, int n );      // 10
    bool LKH( vector <unsigned> &child1, TSP *tsp1, int n );      // 11
    //golosos
    bool GSM( vector <unsigned> &child1, TSP *tsp1 );             // x
    bool HM( vector <unsigned> &child1, TSP *tsp1 );              // x
    bool NJ( vector <unsigned> &child1, individuos *pop, TSP *tsp1, unsigned sizePop ); // x
    bool twoOPT( vector <unsigned> &child1, TSP *tsp1 );          // 12
    bool SHMO( vector <unsigned> &child1, TSP *tsp1, int n );     // 13
    bool DBM2( vector <unsigned> &child1, TSP *tsp1, int n );            // 14
    bool threeOPT( vector <unsigned> &child1, TSP *tsp1, int n ); // 15

    //no lo usaré
    bool GSTM( vector <unsigned> &child1, TSP *tsp1 );
private:
    /* data */
    vector<unsigned> tipo3( vector <unsigned> &child1, TSP *tsp1,
                            unsigned c, int c_i, unsigned c_prime, int c_prime_i );

    vector<unsigned> tipo4( vector <unsigned> &child1, TSP *tsp1,
                            unsigned c, int c_i, unsigned c_prime, int c_prime_i );
};

#endif	/* _MUTATION_H */
