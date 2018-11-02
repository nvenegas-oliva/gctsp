/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * auxiliares.cpp
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

#include "auxiliares.hpp"
#include "genetic.hpp"

Aux::Aux( )
{

}

Aux::~Aux( )
{

}

/**********************************************************/
/*
 * Ejemplo de permutaciones en orden lexicografico en C,
 * usando algoritmo de R. Sedgewick (ver referencia).
 *
 * Autor: manuel.freire@uam.es & pablo.haya@uam.es
 * (C) 2007-08 Escuela Politecnica Superior, UAM
 */

/*
 * OJO: las permutaciones llegan *invertidas*, y se debe
 * ignorar P[0] y llegar hasta P[N] (en lugar de P[N-1])
 */
void Aux::process( int* P, int N, int* total )
{
    int i;
    vector<int> p;

    for ( i = N; i > 0; i-- ) {
        // printf("%d ", P[i]);
        p.push_back( P[i] );
    }
    permutacion.push_back( p );
    // printf("\n");
    ( *total )++;
}

/* esto seguro que sabes lo que hace ... */
void Aux::swap( int *x, int *y )
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

/* ==== Comienzo de la magia ====
 * Extraido de R. Sedgewick, "Permutations Generation Methods"
 * ACM Computing Surveys, Vol. 9, No. 2, p. 154, Junio 1977
 */

/* invierte el orden del array de enteros P */
void Aux::reverse( int *P, int N )
{
    int i = 1;
    while ( i < ( N + 1 - i ) ) {
        swap( &P[i], &P[N + 1 - i] );
        i++;
    }
}

int B( int N, int c )
{
    return ( ( N % 2 ) != 0 ? 1 : c );
}

/* permutaciones en orden lexicografico; cuenta tambien el total
 * OJO: el elemento '0' del array P *no* se procesa; y P[N] *sÃ­* cuenta
 */
void Aux::lexperms( int *P, int N, int *total )
{
    int i;
    int c[N];
    for ( i = N; i > 1; i-- )
        c[i] = 1;
    i = 2;

    process( P, N, total );
    do {
        if ( c[i] < i ) {
            swap( &P[i], &P[c[i]] );
            reverse( P, i - 1 ); /* inversion parcial! */
            process( P, N, total );
            c[i]++;
            i = 2;
        } else {
            c[i] = 1;
            i++;
        }
    } while ( i <= N );
}

/* ==== Final de la magia ==== */
/**********************************************************/


/* ==== Acá comienzan las funciones implementadas para el GCTSP ==== */

void recorreVector( vector <unsigned> v ){
    for(    vector <unsigned>::iterator iter = v.begin();
            iter != v.end();
            iter++)
        {
            cout << *iter << " " ;
        }
    cout << endl;
}



void intercambiaVector( std::vector<unsigned> &destino,
						std::vector<unsigned> &origen )
{
    //Limpiamos el vector de destino
    destino.clear( );
    for( vector<unsigned>::iterator iter = origen.begin( );
         iter != origen.end( );
         iter++ )
        {
            destino.push_back( *iter );
        }
}


bool validarP( map < int, set <int > > *associations,
               vector<unsigned> cromosoma )
{
    set <int> *covers;
    for ( vector<unsigned>::iterator iter = cromosoma.begin( );
          iter != cromosoma.end( );
          iter++ )
        {
            map<int, set<int>>::iterator it;
            set < int > vec;
            //Contando las clientes asociados al supplierTemp.
            it = associations->find( *iter );

            vec = it->second;
            for ( set <int>::iterator iter = vec.begin( ); iter != vec.end( ); iter++ )
                covers->insert( *iter );


            if ( covers->size( ) >= P )
                {
                    // cout << "P es " << P << " y suma es " << covers.size( ) << endl;
                    return true;
                }
        }
    return false;
}


void genera_conjunto( set < unsigned > *out , vector < unsigned > child1 )
{
    set < unsigned >::iterator iter;
    for( unsigned int i = 0 ; i < child1.size() ; i++ )
        {
            iter = out->find( child1.at(i) );
            //Encontre el elemento en el cromosoma creado, lo elimino del conjunto out
            if( iter != out->end() )
                {
                    out->erase( *iter );
                }
        }
}

bool agrega_gen( set < unsigned > &conjunto , vector < unsigned > &child1 )
{
    if( conjunto.size() == 0 )
        {
            return true;
        }
    //Agrego un elemento de conjunto a child1 y luego lo borro de conjunto
    //Esto puede ser random o de forma ordenada
    set <unsigned>::iterator iter= conjunto.begin();
    child1.push_back( *iter );
    return false;
}

/**
 * Función que genera un set con todos los customers cubiertos por el
 * cromosoma (hasta su P) menos el que se desea reemplazar
 *
 * @param covers: set con los todos los customers cubiertos
 * @param associations: mapa con los customers por supliers
 * @param cromosoma: cromosoma (con las ciudades que solo cubren hasta P)
 * @param P: número de customers a cubrir
 *
 * @return newP: retorna el nuevo valor que cubre este cromosoma
 *
 */

int conjuntoConCustomersAll( set < int > *covers , map < int, set <int > > *associations, vector < unsigned > cromosoma, int P )
{
    set < unsigned >::iterator iter;
    int newP = 0;
    // cout << "que xuxa" << P << endl;
    for ( vector<unsigned>::iterator iter = cromosoma.begin( );
          iter != cromosoma.end( );
          iter++ )
        {
            map<int, set<int>>::iterator it;
            set < int > vec;
            // Contando las clientes asociados al supplierTemp.
            it = associations->find( *iter );
            vec = it->second;
            for ( set <int>::iterator i = vec.begin( ); i != vec.end( ); i++ )
                covers->insert( *i );
            // Se actualiza hasta donde cubre ahora
            newP++;
            // cout << "xxx " <<  *iter <<  " "<< covers->size( ) << " "<< P << " "<< newP  << endl;
            if ( covers->size( ) >= P )
                return newP;
        }

    // Su nuevo número para cubrir no cumple, así que se deja el antiguo
    return -1;
}

/**
 * Función que genera un set con todos los customers cubiertos por el
 * cromosoma (hasta su P) menos el que se desea reemplazar
 *
 * @param covers: set con los todos los customers cubiertos
 * @param associations: mapa con los customers por supliers
 * @param cromosoma: cromosoma (con las ciudades que solo cubren hasta P)
 * @param reemplazar: ciudad de la cual no se quieren agregar sus customers
 *
 */

void conjuntoConCustomers( set < int > *covers , map < int, set <int > > *associations, vector < unsigned > cromosoma, int reemplazar, int n)
{
    set < unsigned >::iterator iter;
    int contar = 0; // para recorrer hasta n no más
    for ( vector<unsigned>::iterator iter = cromosoma.begin( );
          iter != cromosoma.end( );
          iter++ )
        {
            map<int, set<int>>::iterator it;
            set < int > vec;
            //Contando las clientes asociados al supplierTemp.
            contar++;
            if(*iter != reemplazar)
                {
                    it = associations->find( *iter );

                    vec = it->second;
                    for ( set <int>::iterator i = vec.begin( ); i != vec.end( ); i++ )
                        covers->insert( *i );
                }
            if ( contar == n)
                return;
        }
}

/**
 * Función que genera un set con todos los customers cubiertos por el
 * cromosoma (hasta su P) menos el que se desea reemplazar
 *
 * @param covers: set con los todos los customers cubiertos
 * @param associations: mapa con los customers por supliers
 * @param ciudad: ciudad de la cual se quieren agregar sus customers
 *
 */

void agregar_customers_alConjunto( set < int > *covers , map < int, set <int > > *associations, int ciudad)
{
    map<int, set<int>>::iterator it = associations->find( ciudad );
    set < int > vec = it->second;
    for ( set <int>::iterator iter = vec.begin( ); iter != vec.end( ); iter++ )
        covers->insert( *iter );
}
