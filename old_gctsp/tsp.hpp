/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * tsp.hpp
 * This file is part of VCP-PGB
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
#ifndef _TSP_H
#define	_TSP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <set>
#include "math.h"

// #include "node.h"

#include <list>
#include <cmath>


using namespace std;

#define Infinite 9999999

class TSP {
public:

    typedef struct ruta {
        float x;
        float y;
        int NombreCiudad;
    } ruta;

    // Para usar con el sort
    template<template <typename> class P = std::less >
    struct compare_pair_second {

        template<class T1, class T2> bool operator()(const std::pair<T1, T2>& left,
                                                     const std::pair<T1, T2>& right) {
            return P<T2>()(left.second, right.second);
        }
    };
    ruta *pt;
    int Numciudades; /* Numero de ciudades */
    int bitsCiudad;
    float matrixCosto;
    int optimoSolucion;
    string ruta_instancia;
    string ruta_resultados;

    TSP( string ruta_instancia, string ruta_resultados );
    TSP( int Numciudades );
    virtual ~TSP( );
    void leerInstancia( vector <vector <float> > pt, int np );
    float calculaDistancia( float x1, float y1, float x2, float y2 );
    float distancia2Ciudades( int i, int j );
    float evaluar( vector <unsigned> chrm );
    float evaluar( vector <unsigned> chrm, int nCity);
    void vecinoMasCercano( int inicio, vector <unsigned> &chrm);
    float getOptimo( );
    void vecinoCercanoAlDepot(vector <unsigned> &ruta,  map < int, set <int> > &associations , int n_rand );

};
#endif	/* _TSP_H */
