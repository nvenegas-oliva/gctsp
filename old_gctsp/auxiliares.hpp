	/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/*
 * auxiliares.hpp
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

#ifndef _AUXILIAR_H
#define	_AUXILIAR_H

#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <cstdio>
#include <vector>
#include <map>
#include <set>

using namespace std;

extern int P;

class Aux {
public:
    vector<vector<int> > permutacion;

    Aux( );
    virtual ~Aux( );
    void lexperms( int *P, int N, int *total );
private:
    void process( int* P, int N, int* total );
    void swap( int *x, int *y );
    void reverse( int *P, int N );

};

#endif	/* _AUXILIAR_H */

//Intercambia el valor de dos vectores: (vDestino <- vFuente)
void intercambiaVector( std::vector<unsigned> &destino,
						std::vector<unsigned> &origen );

//Valida que el cromosoma de suppliers cumpla la cantidad P de customers.
bool validarP( map < int, set <int > > *associations,
               vector<unsigned> cromosoma);

void recorreVector( vector <unsigned> v );

//out es un contenedor con todos los genes de los padres de child1
//chid1 es el cromosoma creado a partir del cruzamiento
//la funcion genera el conjunto out - chil1 y lo deja en out
void genera_conjunto( set < unsigned > *out , vector < unsigned > child1 );


//Mueve un gen desde conjunto a child1
bool agrega_gen( set < unsigned > &conjunto , vector < unsigned > &child1 );

int conjuntoConCustomersAll( set < int > *covers , map < int, set <int > > *associations, vector < unsigned > cromosoma, int P );
void conjuntoConCustomers( set < int > *covers , map < int, set <int > > *associations, vector < unsigned > cromosoma, int reemplazar, int n);

void agregar_customers_alConjunto( set < int > *covers , map < int, set <int > > *associations, int ciudad);
