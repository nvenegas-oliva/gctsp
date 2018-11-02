/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * mutation.cpp
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

#include "mutation.hpp"

Mutation::Mutation( int largo, float pmuta, unsigned seed, map < int, set <int> > mapa)
{
    associations = mapa;
    sizeChrm = largo;
    pmut = pmuta;
    generator.seed( seed );
    // Para usar para el orden de recorrer
    for(int i = 0; i < sizeChrm; i++)
        fijo.push_back(i);
}

Mutation::~Mutation( )
{

}

/**
 * Función que mueve todos los clientes (de forma circular) hasta colocar el
 * depot en la posición
 *
 */

void Mutation::auxPutDepotBegin(vector <unsigned> &child1)
{
    vector<unsigned> child1Aux = child1;

    // cout << "in auxPutDepotBegin" << endl;
    // for ( int i = 0; i < child1.size(); i++ )
    //     cout << child1[i] << " ";
    // cout << endl;

    int j = 0;
    // Search depot position
    for ( int i = 0; i < child1.size(); i++ )
        if ( child1[i] == 0 ) {
            j = i - 1; // Para poder poner el j++ después
            break;
        }
    // Move customer until Depot put on position 0
    for ( int i = 0; i < child1.size(); i++ ) {
        j = (j == child1.size() - 1) ? 0 : j+1;
        child1[i] = child1Aux[j];
    }

    // for ( int i = 0; i < child1.size(); i++ )
    //     cout << child1[i] << " ";
    // cout << endl;
    // exit( 0 );
}

void Mutation::setSizeChrm( unsigned nChrm )
{
    sizeChrm = nChrm;
}

/**
 *
 * Exchange mutation (EM)
 *
 * Banzhaf (1990). The “Molecular” Traveling Salesman. Biological Cybernetics 64: 7–14.
 *
 **/
bool Mutation::EM( vector <unsigned> &child1 )
{
    int i, j, aux;
    i = rnd( 0, sizeChrm - 1 );
    j = rnd( 0, sizeChrm - 1 );
    aux = child1[i];
    child1[i] = child1[j];
    child1[j] = aux;

    return true;
}


bool Mutation::EM2(vector <unsigned> &child1, TSP *tsp1, int n)
{
  int i, j, aux;
  double current = tsp1->evaluar(child1, n);
  int k = 0;

  do {
    do
      {
        i = rnd(0, n);
        j = rnd(n, sizeChrm - 1);
      }
    while(i == j);
    aux = child1[i];
    child1[i] = child1[j];
    child1[j] = aux;

    double newSol = tsp1->evaluar(child1);
    if(newSol < current)
      {
        // cout << "lalal " <<  current << " > " << newSol << endl;
        newSol = current;
      }
    else
      {
        aux = child1[i];
        child1[i] = child1[j];
        child1[j] = aux;
      }
    k++;
  }
  while(k < n);

  return true;
}

/**
 *
 * Scramble mutation (SM)
 *
 * Syswerda (1991). Schedule Optimization Using Genetic Algorithms. In Davis, L. (ed.)
 * Handbook of Genetic Algorithms, 332–349. New York: Van Nostrand Reinhold.
 *
 **/

bool Mutation::SM( vector <unsigned> &child1 )
{
    int begin, end;

    do {
        begin = rnd( 0, sizeChrm - 2 );
        end = rnd( begin, sizeChrm - 1 );
    } while ( begin == end );

    shuffle( child1.begin( ) + begin, child1.begin( ) + end, generator );

    return true;
}

/**
 *
 * Simple Inversion mutation (SIM)
 *
 * Holland (1975). Adaptation in Natural and Artificial Systems. Ann Arbor:
 * University of Michigan Press.
 *
 * Grefenstette (1987). (Ed). Genetic Algorithms and Their Applications:
 * Proceedings of the Second International Conference. Hillsdale, New Jersey:
 * Lawrence Erlbaum.
 *
 **/
bool Mutation::SIM( vector <unsigned> &child1 )
{
    int i, j, begin, end;
    vector <unsigned> subtour = child1;

    do {
        begin = rnd( 0, sizeChrm - 1 );
        end = rnd( 0, sizeChrm - 1 );
        if ( begin > end ) {
            i = begin;
            end = i;
            begin = end;
        }
    } while ( begin == end );

    for ( i = begin, j = end; i <= end; i++ ) {
        child1[i] = subtour[j];
        j--;
    }
    return true;
}

/**
 *
 * Displacement mutation (DM)
 *
 * Michalewicz (1992). Genetic Algorithms + Data Structures = Evolution
 * Programs. Berlin Heidelberg: Springer Verlag.
 *
 **/
bool Mutation::DM( vector <unsigned> &child1 )
{
    int i, j, begin, end;

    do {
        begin = rnd( 0, sizeChrm - 1 );
        end = rnd( 0, sizeChrm - 1 );
        if ( begin > end ) {
            i = begin;
            end = i;
            begin = end;
        }
    } while ( begin == end || ( begin == 0 && end == sizeChrm - 1 ) );

    vector <unsigned> subtour( end - begin + 1 );
    for ( i = begin, j = 0; j < subtour.size( ); j++ ) {
        subtour[j] = child1[i];
        child1.erase( child1.begin( ) + i );
    }

    i = rnd( 0, child1.size( ) - 1 );
    child1.insert( child1.begin( ) + i + 1, subtour.begin( ), subtour.end( ) );

    return true;
}

/**
 *
 * Inversion mutation (IVM)
 *
 * Fogel (1990). A Parallel Processing Approach to a Multiple Traveling
 * Salesman Problem Using Evolutionary Programming. In Canter, L. (ed.)
 * Proceedings on the Fourth Annual Parallel Processing Symposium,
 * 318–326. Fullterton, CA.
 *
 * Fogel (1993). Applying Evolutionary Programming to Selected Traveling
 * Salesman Problems. Cybernetics and Systems 24: 27–36.
 *
 **/
bool Mutation::IVM( vector <unsigned> &child1 )
{
    int i, j, begin, end;

    do {
        begin = rnd( 0, sizeChrm - 1 );
        end = rnd( 0, sizeChrm - 1 );
        if ( begin > end ) {
            i = begin;
            end = i;
            begin = end;
        }
    } while ( begin == end || ( begin == 0 && end == sizeChrm - 1 ) );

    vector <unsigned> subtour( end - begin + 1 );
    for ( i = begin, j = 0; j < subtour.size( ); j++ ) {
        subtour[j] = child1[i];
        child1.erase( child1.begin( ) + i );
    }

    i = rnd( 0, child1.size( ) - 1 );
    reverse( subtour.begin( ), subtour.end( ) );
    child1.insert( child1.begin( ) + i + 1, subtour.begin( ), subtour.end( ) );

    return true;
}

/**
 *
 * Insertion mutation (ISM)
 *
 * Fogel (1988). An Evolutionary Approach to the Traveling Salesman
 * Problem. Biological Cybernetics 60: 139–144.
 *
 * Michalewicz (1992). Genetic Algorithms + Data Structures = Evolution
 * Programs. Berlin Heidelberg: Springer Verlag.
 *
 **/
bool Mutation::ISM( vector <unsigned> &child1, int n )
{
    int i, j, aux;
    // Se quita un ciudad de las ciudades de la solución
    i = rnd( 0, --n );
    aux = child1[i];
    child1.erase( child1.begin( ) + i );
    j = rnd( n, child1.size( ) );
    // Se coloca en las ciudades que están fuera
    child1.insert( child1.begin( ) + j, aux );
    return true;
}

/**
 *
 * Greedy-swap mutation (GSM)
 *
 * Louis & Tang (1999). Interactive genetic algorithms for the
 * traveling salesman problem. In Proceedings of the 1999 genetic and
 * evolutionary computing conference (GECCO 1999) (pp. 385–392).
 *
 **/
bool Mutation::GSM( vector <unsigned> &child1, TSP *tsp1 )
{
    int i, j, k = 0, aux1, aux2;

    do {
        do {
            i = rnd( 0, sizeChrm - 1 );
            j = rnd( 0, sizeChrm - 1 );
        } while ( i == j );

        unsigned imas = ( i == child1.size( ) - 1 ) ? 0 : i + 1;
        unsigned jmas = ( j == child1.size( ) - 1 ) ? 0 : j + 1;

        unsigned imenos = ( i == 0 ) ? child1.size( ) - 1 : i - 1;
        unsigned jmenos = ( j == 0 ) ? child1.size( ) - 1 : j - 1;

        if ( tsp1->distancia2Ciudades( child1[imenos], child1[i] ) + tsp1->distancia2Ciudades( child1[i], child1[imas] ) +
             tsp1->distancia2Ciudades( child1[jmenos], child1[j] ) + tsp1->distancia2Ciudades( child1[j], child1[jmas] )
             >
             tsp1->distancia2Ciudades( child1[imenos], child1[j] ) + tsp1->distancia2Ciudades( child1[j], child1[imas] ) +
             tsp1->distancia2Ciudades( child1[jmenos], child1[i] ) + tsp1->distancia2Ciudades( child1[i], child1[jmas] ) ) {
            aux1 = child1[i];
            aux2 = child1[j];

            child1[i] = aux2;
            child1[j] = aux1;
            break;
        }
        k++;
    } while ( k < sizeChrm );

    return true;
}

/**
 *
 * Heuristic mutation (HM)
 *
 * Gen & Cheng (1997). “Genetic Algorithm and Engineering design”, John
 * wiley and sons, New  York,  118-127.
 *
 **/
bool Mutation::HM( vector <unsigned> &child1, TSP *tsp1 )
{
    int i, j, n, total;
    vector<int> indice;
    int temp;
    map<int, int> city;
    Aux *aux = new Aux( );
    vector<int>::iterator it;
    // vector<unsigned> child_min;
    n = rnd( 3, 7 + 1 );
    //n = 6+1;
    for ( int k = 0; k < n; k++ )
        indice.push_back( -1 );

    for ( int k = 0; k < n; k++ ) {
        do {
            temp = rnd( 1, sizeChrm - 1 );
            it = find( indice.begin( ), indice.begin( ) + k, temp );
        } while ( *it == temp );

        indice[k] = temp;
        city.insert( pair<int, int>( indice[k], child1[indice[k]] ) );
    }

    total = 0;
    aux->lexperms( &indice[0], n - 1, &total );
    float actual, menor = Infinite;
    int min = -1;
    for ( i = 0; i < aux->permutacion.size( ); i++ ) {
        actual = 0;
        for ( j = 0; j < aux->permutacion[i].size( ); j++ ) {
            int pos = indice[j + 1];
            unsigned posMas1 = ( pos == child1.size( ) - 1 ) ? 0 : pos + 1;
            unsigned posMenos1 = ( pos == 0 ) ? child1.size( ) - 1 : pos - 1;

            actual = tsp1->distancia2Ciudades( child1[posMenos1], child1[pos] ) +
                tsp1->distancia2Ciudades( child1[pos], child1[posMas1] );
        }

        if ( actual < menor ) {
            menor = actual;
            min = i;
        }
    }
    for ( j = 0; j < aux->permutacion[min].size( ); j++ )
        child1[indice[j + 1]] = city[aux->permutacion[min][j]];

    delete aux;
    return true;
}

/**
 *
 * Greedy Sub Tour Mutation (GSTM)
 *
 * Albayrak & Allahverdi (2011). Development a new mutation operator to solve
 * the Traveling Salesman Problem by aid of Genetic Algorithms. Expert
 * Systems with Applications, 38(3), 1313–1320.
 *
 **/
bool Mutation::GSTM( vector <unsigned> &child1, TSP *tsp1 )
{
    float Prc, Pcp, Pl;
    int R1, R2, Lmin, Lmax, NLmax;
    Prc = 0.5;
    Pcp = 0.8;
    Pl = 0.2;
    Lmin = 2;
    Lmax = sqrt( sizeChrm );
    NLmax = 5;

    do {
        R1 = rnd( 0, sizeChrm - 2 );
        R2 = rnd( R1 + 1, sizeChrm - 1 );
    } while ( ( R2 - R1 < Lmin ) || ( R2 - R1 > Lmax ) );
    //Reconexión
    if ( flip( Prc ) ) {
        int i, j;
        vector <unsigned> subtour( R2 - R1 + 1 );
        for ( i = R1, j = 0; j < subtour.size( ); j++ ) {
            subtour[j] = child1[i];
            child1.erase( child1.begin( ) + i );
        }

        vector <unsigned> auxChild1 = child1;
        float costo, min = Infinite;
        for ( i = 0; i < auxChild1.size( ) - 1; i++ ) {
            auxChild1.insert( auxChild1.begin( ) + i + 1, subtour.begin( ), subtour.end( ) );
            costo = tsp1->evaluar( auxChild1 );

            if ( min > costo ) {
                j = i;
                min = costo;
            }
            auxChild1 = child1;
        }

        child1.insert( child1.begin( ) + j + 1, subtour.begin( ), subtour.end( ) );
    } else {
        if ( flip( Pcp ) ) //Distorción
            {
                if ( flip( Prc ) ) {
                    vector <unsigned> subtour( R2 - R1 + 1 );
                    for ( int i = R1, j = 0; j < subtour.size( ); j++, i++ )
                        subtour[j] = child1[i];

                    for ( int j = R1; subtour.size( ) > 0; j++ ) {
                        if ( flip( Pl ) ) {
                            int city = rnd( 0, subtour.size( ) - 1 );
                            int i = find( child1.begin( ), child1.end( ), subtour[city] ) - child1.begin( );
                            child1.erase( child1.begin( ) + i );
                            child1.insert( child1.begin( ) + j, subtour[city] );
                            subtour.erase( subtour.begin( ) + city );
                        } else {
                            int last = subtour.size( ) - 1;
                            int i = find( child1.begin( ), child1.end( ), subtour[last] ) - child1.begin( );
                            child1.erase( child1.begin( ) + i );
                            child1.insert( child1.begin( ) + j, subtour[last] );
                            subtour.erase( subtour.begin( ) + last );
                        }
                    }
                }
            } else //Rotación
            {
                // vecino R1
                vector<pair <int, int> > distR1;
                vector<unsigned> child11 = child1;
                for ( int i = 0; i < sizeChrm; i++ )
                    if ( child1[R1] != i ) {
                        pair <int, int> city( i, tsp1->distancia2Ciudades( child1[R1], i ) );
                        distR1.push_back( city );
                    }
                sort( distR1.begin( ), distR1.end( ), compare_pair_second<std::less>( ) );

                int j, NLR1, R1mas1, NLR1mas1;
                bool flag;
                do {
                    flag = false;
                    j = rnd( 0, NLmax - 1 );
                    NLR1 = find( child1.begin( ), child1.end( ), distR1[j].first ) - child1.begin( );
                    if ( R1 != sizeChrm - 1 ) {
                        if ( R1 + 1 == NLR1 )
                            flag = true;
                    } else
                        if ( NLR1 == 0 )
                            flag = true;

                    if ( R1 != 0 ) {
                        if ( R1 - 1 == NLR1 )
                            flag = true;
                    } else
                        if ( NLR1 == sizeChrm - 1 )
                            flag = true;

                    if ( R1 == NLR1 )
                        flag = true;

                } while ( flag );

                // cout << R1 << " aca1 " << NLR1 << endl;
                if ( R1 < NLR1 ) {
                    if ( NLR1 == child1.size( ) - 1 ) {
                        R1--;
                        NLR1--;
                        R1mas1 = R1 + 1;
                        NLR1mas1 = NLR1 + 1;
                        int aux = child1[0];
                        child1.erase( child1.begin( ) );
                        child1.insert( child1.end( ), aux );
                    } else {
                        R1mas1 = R1 + 1;
                        NLR1mas1 = NLR1 + 1;
                    }
                    if ( R1 == NLR1 )
                        flag = true;
                } else {
                    if ( NLR1 == 0 ) {
                        R1++;
                        NLR1++;
                        R1mas1 = R1 - 1;
                        NLR1mas1 = NLR1 - 1;
                        int end = child1.size( ) - 1;
                        int aux = child1[end];
                        child1.erase( child1.begin( ) + end );
                        child1.insert( child1.begin( ), aux );
                    } else {
                        R1mas1 = R1 - 1;
                        NLR1mas1 = NLR1 - 1;
                    }
                }
                float gainR1 = tsp1->distancia2Ciudades( child1[R1], child1[R1mas1] )
                    + tsp1->distancia2Ciudades( child1[NLR1], child1[NLR1mas1] )
                    - ( tsp1->distancia2Ciudades( child1[R1], child1[NLR1] )
                        + tsp1->distancia2Ciudades( child1[R1mas1], child1[NLR1mas1] ) );

                // cout << "\tlalalallalalaa" << endl;
                //vecino R2
                vector<pair <int, int> > distR2;
                for ( int i = 0; i < sizeChrm; i++ )
                    if ( child11[R1] != i ) {
                        pair <int, int> city( i, tsp1->distancia2Ciudades( child11[R1], i ) );
                        distR2.push_back( city );
                    }
                sort( distR2.begin( ), distR2.end( ), compare_pair_second<std::less>( ) );

                int R2mas1, NLR2mas1, NLR2;

                do {
                    flag = false;
                    j = rnd( 0, NLmax - 1 );
                    NLR2 = find( child11.begin( ), child11.end( ), distR2[j].first ) - child11.begin( );
                    if ( R2 != sizeChrm - 1 ) {
                        if ( R2 + 1 == NLR2 )
                            flag = true;
                    } else
                        if ( NLR2 == 0 )
                            flag = true;

                    if ( R2 != 0 ) {
                        if ( R2 - 1 == NLR2 )
                            flag = true;
                    } else
                        if ( NLR2 == sizeChrm - 1 )
                            flag = true;
                    if ( R2 == NLR2 )
                        flag = true;
                    // cout << R2 << " " << NLR2 << endl;
                } while ( flag );

                // cout << R2 << " aca " << NLR2 << endl;
                if ( R2 < NLR2 ) {

                    if ( NLR2 == child11.size( ) - 1 ) {
                        R2--;
                        NLR2--;
                        R2mas1 = R2 + 1;
                        NLR2mas1 = NLR2 + 1;
                        int aux = child11[0];
                        child11.erase( child11.begin( ) );
                        child11.insert( child11.end( ), aux );
                    } else {
                        R2mas1 = R2 + 1;
                        NLR2mas1 = NLR2 + 1;
                    }
                } else {
                    // cout << "llalala wea " << R2 << " " << NLR2 << endl;
                    if ( NLR2 == 0 ) {
                        R2++;
                        NLR2++;
                        R2mas1 = R2 - 1;
                        NLR2mas1 = NLR2 - 1;
                        int end = child11.size( ) - 1;
                        int aux = child11[end];
                        child11.erase( child11.begin( ) + end );
                        child11.insert( child11.begin( ), aux );
                    } else {
                        R2mas1 = R2 - 1;
                        NLR2mas1 = NLR2 - 1;
                    }
                }
                // cout << R2 << " " << NLR2 << endl;
                // cout << R2mas1 << " " << NLR2mas1 << endl;
                float gainR2 = tsp1->distancia2Ciudades( child11[R2], child11[R2mas1] )
                    + tsp1->distancia2Ciudades( child11[NLR2], child11[NLR2mas1] )
                    - ( tsp1->distancia2Ciudades( child11[R2], child11[NLR2] )
                        + tsp1->distancia2Ciudades( child11[R2mas1], child11[NLR2mas1] ) );

                int x, y;

                if ( gainR1 > gainR2 ) {
                    x = R1mas1;
                    y = NLR1;
                    // cout << "gain1 "<< gainR1 << endl;
                    // cout << "R1 es " << R1 << " ";
                    // cout << x << " " << y << endl;
                    // cout << "el otro "<< gainR2 << endl;
                } else {
                    x = R2mas1;
                    y = NLR2;
                    // child1 = child11;
                    // cout << "gain2 " << gainR2 << endl;
                    // cout << "R1 es " << R2 << " ";
                    // cout << x << " " << y << endl;
                    // cout << "el otro "<< gainR1 << endl;
                }
                // for(int i = 0; i < child1.size(); i++)
                //   cout << child1[i] << " ";
                // cout << endl;

                if ( x > y )
                    reverse( child1.begin( ) + y, child1.begin( ) + x + 1 );
                else
                    reverse( child1.begin( ) + x, child1.begin( ) + y + 1 );

                // cout << tsp1->evaluar(child1)<< endl;

                // for(int i = 0; i < child1.size(); i++)
                //   cout << child1[i] << " ";
                // cout << endl;

                // x = R1mas1;
                // y = NLR1;
                // cout << endl;
                // for(int i = 0; i < child11.size(); i++)
                //   cout << child11[i] << " ";
                // cout << endl;
                // cout << "R1 es " << R1 << " ";
                // cout << x << " " << y << endl;
                // if(x > y)
                //   reverse(child11.begin() + y, child11.begin() + x + 1);
                // else
                //   reverse(child11.begin() + x, child11.begin() + y + 1);

                // cout << tsp1->evaluar(child11) << " "<< gainR1 << endl;

                // for(int i = 0; i < child11.size(); i++)
                //   cout << child11[i] << " ";
                // cout << endl;




                // exit(0);
            }
    }
    return true;
}

/**
 *
 * 2-opt
 *
 * Croes, G. A. (1958). A Method for Solving Traveling-Salesman
 * Problems. Operations Research, 6(6), 791–812.
 *
 **/
bool Mutation::DBM( vector <unsigned> &child1 )
{
    int i, j, k, l;
    bool flag = false;
    do {
        i = rnd( 0, sizeChrm - 7 );
        j = i + 2;
        k = rnd( 0, sizeChrm - 3 );
        l = k + 2;

        if ( j + 1 < k )
            flag = false;

    } while ( flag );

    int aux = child1[( i + 1 ) % sizeChrm];
    child1[( i + 1 ) % sizeChrm] = child1[( k + 1 ) % sizeChrm];
    child1[( k + 1 ) % sizeChrm] = aux;

    aux = child1[j];
    child1[j] = child1[l];
    child1[l] = aux;

    return true;
}

/**
 *
 * 2-opt
 *
 * Croes, G. A. (1958). A Method for Solving Traveling-Salesman
 * Problems. Operations Research, 6(6), 791–812.
 *
 **/
bool Mutation::twoOPT( vector <unsigned> &child1, TSP *tsp1 )
{
    float gainActual, gainCandidato, diff;
    int Imas1, Jmas1, Ci = -1, Cj = -1;

    // (largo del cromosoma hasta P + 1 (depot)) - 1 (así está implementado
    // el n, en la versión anterior)
    int n = child1.size( ) - 1;
        float valor = tsp1->evaluar( child1 );
    float min = valor;

    for ( int i = 0; i < child1.size( ); i++ ) {
        for ( int j = i + 2; j < child1.size( ) - 2; j++ ) {
            Imas1 = ( i == n ) ? 0 : i + 1;
            Jmas1 = ( j == n ) ? 0 : j + 1;

            gainActual = tsp1->distancia2Ciudades( child1[i], child1[Imas1] )
                + tsp1->distancia2Ciudades( child1[j], child1[Jmas1] );

            gainCandidato = tsp1->distancia2Ciudades( child1[i], child1[j] )
                + tsp1->distancia2Ciudades( child1[Imas1], child1[Jmas1] );

            diff = valor - gainActual + gainCandidato;

            if ( min > diff ) {
                Ci = i;
                Cj = j;
                min = diff;

                // //Pruebas 2-OPT con break
                // reverse(child1.begin() + Ci + 1, child1.begin() + Cj + 1);
                // return true;
                // //
            }
        }
    }


    if ( Ci != -1 && Cj != -1 )
        reverse( child1.begin( ) + Ci + 1, child1.begin( ) + Cj + 1 );
    else
        reverse( child1.begin( ) + rnd( 0, n ) + 1, child1.begin( ) + rnd( 0, n ) + 1 );

    // Si el depot no está al comienzo lo movemos al comienzo
    if ( child1[0] != 0 ) {
        auxPutDepotBegin(child1);
    }


    return true;
}

/**
 *
 * Self-adaptive Hybrid Mutation Operator (SHMO)
 *
 * Wang, Cui, Wang & Chen (2005). A Novel Ant Colony System Based on Minimum
 * 1-Tree and Hybrid Mutation for TSP. In L. Wang, K. Chen, & Y. Ong (Eds.),
 * Advances in Natural Computation (Vol. 3611, pp. 1269–1278). Springer
 * Berlin Heidelberg.
 *
 **/

bool Mutation::SHMO( vector <unsigned> &child1, TSP *tsp1, int n )
{
    float valor = tsp1->evaluar( child1 );
    int Imenos1, Imas1, Jmenos1, Jmas1, gainActual, gainCandidato, diff;
    float min = valor;
    // Se trabaja con n - 1
    n = n - 1;
    if ( n < 4 )
        return false;
    for ( int i = 0; i < n - 1; i++ )
        for ( int j = i + 2; j < n + 1; j++ ) {
            if ( j - i == n )
                break;

            //invertir
            Imas1 = ( i == n ) ? 0 : i + 1;
            Jmas1 = ( j == n ) ? 0 : j + 1;

            gainActual = tsp1->distancia2Ciudades( child1[i], child1[Imas1] )
                + tsp1->distancia2Ciudades( child1[j], child1[Jmas1] );

            gainCandidato = tsp1->distancia2Ciudades( child1[i], child1[j] )
                + tsp1->distancia2Ciudades( child1[Imas1], child1[Jmas1] );

            diff = valor - gainActual + gainCandidato;

            if ( min > diff ) // Si es menor, se hace swap
                {
                    reverse( child1.begin( ) + i + 1, child1.begin( ) + j + 1 );
                    valor = min = diff;
                    // continue;
                }

            // insert
            Imenos1 = ( i == 0 ) ? n : i - 1;
            Jmas1 = ( j == n ) ? 0 : j + 1;
            Jmenos1 = ( j == 0 ) ? n : j - 1;

            gainActual = tsp1->distancia2Ciudades( child1[i], child1[Imenos1] )
                + tsp1->distancia2Ciudades( child1[j], child1[Jmas1] )
                + tsp1->distancia2Ciudades( child1[j], child1[Jmenos1] );

            gainCandidato = tsp1->distancia2Ciudades( child1[j], child1[Imenos1] )
                + tsp1->distancia2Ciudades( child1[i], child1[j] )
                + tsp1->distancia2Ciudades( child1[Jmas1], child1[Jmenos1] );


            diff = valor - gainActual + gainCandidato;

            if ( min > diff ) // Si es menor, se hace swap
                {
                    int nuevo = child1[j];
                    child1.erase( child1.begin( ) + j );
                    child1.insert( child1.begin( ) + i, nuevo );
                    valor = min = diff;
                    // continue;
                }

            //swap
            Imas1 = ( i == n ) ? 0 : i + 1;
            Imenos1 = ( i == 0 ) ? n : i - 1;
            Jmas1 = ( j == n ) ? 0 : j + 1;
            Jmenos1 = ( j == 0 ) ? n : j - 1;

            if ( ( ( ( i % n ) + 1 ) == j ) || ( ( ( i % n ) + 2 ) == j ) ||
                 ( ( ( i % n ) - 1 ) == j ) || ( ( ( i % n ) - 2 ) == j ) ) {
                gainActual = tsp1->distancia2Ciudades( child1[i], child1[Imenos1] )
                    + tsp1->distancia2Ciudades( child1[j], child1[Jmas1] );

                gainCandidato = tsp1->distancia2Ciudades( child1[j], child1[Imenos1] )
                    + tsp1->distancia2Ciudades( child1[i], child1[Jmas1] );
                //continue;
            } else if ( i == 0 && j == n ) {
                gainActual = tsp1->distancia2Ciudades( child1[i], child1[Imas1] )
                    + tsp1->distancia2Ciudades( child1[j], child1[Jmenos1] );

                gainCandidato = tsp1->distancia2Ciudades( child1[j], child1[Imas1] )
                    + tsp1->distancia2Ciudades( child1[i], child1[Jmenos1] );
                // continue;
            } else {
                gainActual = tsp1->distancia2Ciudades( child1[i], child1[Imas1] )
                    + tsp1->distancia2Ciudades( child1[i], child1[Imenos1] )
                    + tsp1->distancia2Ciudades( child1[j], child1[Jmas1] )
                    + tsp1->distancia2Ciudades( child1[j], child1[Jmenos1] );

                gainCandidato = tsp1->distancia2Ciudades( child1[j], child1[Imas1] )
                    + tsp1->distancia2Ciudades( child1[j], child1[Imenos1] )
                    + tsp1->distancia2Ciudades( child1[i], child1[Jmas1] )
                    + tsp1->distancia2Ciudades( child1[i], child1[Jmenos1] );
                // continue;
            }

            diff = valor - gainActual + gainCandidato;

            if ( min > diff ) {
                int aux = child1[i];
                child1[i] = child1[j];
                child1[j] = aux;
                valor = min = diff;
                // continue;
            }
        }

    // Si el depot no está al comienzo lo movemos al comienzo
    if ( child1[0] != 0 ) {
        auxPutDepotBegin(child1);
    }
    return true;
}

/**
 *
 * Double Bridge move (DBM)
 *
 * Martin, Otto & Felten (1991). Large-Step Markov Chains for the Traveling
 * Salesman Problem. Complex Systems, 5, 299-326.
 *
 * Martin, Otto, & Felten (1992). Large-step Markov chains for the TSP
 * incorporating local search heuristics. Operations Research Letters, 11(4),
 * 219-224.
 *
 **/

bool Mutation::DBM2( vector <unsigned> &child1, TSP *tsp1, int m )
{
    int i, j, k, l;
    int I, J, K, L;
    i = j = k = l = I = J = K = L = -1;
    int ciudadRespaldo = -1;
    bool cambiar = false, flag = true;
    float actual = 0, nuevo = 0;

    if(m < 8)
        return false;
    for ( int n = 0; n < 10; n++ ) {
        do {
            i = rnd( 0, m - 7 );
            j = rnd( i, m - 5 );
            k = rnd( j, m - 3 );
            l = rnd( k, m - 1 );

            if ( ( i < j ) && ( j < k ) && ( k < l ) && ( i + 1 < j ) && ( j + 1 < k ) && ( k + 1 < l ) && ( l + 1 < m ) )
                flag = false;

        } while ( flag );

        flag = true;

        //costo actual
        for ( int x = i; x <= j; x++ )
            actual += tsp1->distancia2Ciudades( child1[x], child1[x + 1] );

        for ( int x = k; x <= l; x++ )
            actual += tsp1->distancia2Ciudades( child1[x], child1[( x + 1 ) % m] );

        //costo nuevo
        nuevo = tsp1->distancia2Ciudades( child1[i], child1[k + 1] );
        for ( int x = k + 1; x < l; x++ )
            nuevo += tsp1->distancia2Ciudades( child1[x], child1[x + 1] );
        nuevo += tsp1->distancia2Ciudades( child1[l], child1[j + 1] );

        nuevo += tsp1->distancia2Ciudades( child1[k], child1[i + 1] );
        for ( int x = i + 1; x < j; x++ )
            nuevo += tsp1->distancia2Ciudades( child1[x], child1[x + 1] );
        nuevo += tsp1->distancia2Ciudades( child1[j], child1[( l + 1 ) % m] );

        if ( actual > nuevo ) {
            I = i;
            J = j;
            K = k;
            L = l;
            ciudadRespaldo = child1[k];
            cambiar = true;
        }
    }

    if ( cambiar ) {
        vector<unsigned> subtour1( child1.begin( ) + I + 1, child1.begin( ) + J + 1 );
        vector<unsigned> subtour2( child1.begin( ) + K + 1, child1.begin( ) + L + 1 );

        child1.erase( child1.begin( ) + I + 1, child1.begin( ) + J + 1 );
        child1.insert( child1.begin( ) + I + 1, subtour2.begin( ), subtour2.end( ) );

        int largo = L - K;
        int newk = -1;

        for ( int x = I + largo; x < child1.size( ); x++ )
            if ( child1[x] == ciudadRespaldo )
                newk = x;

        child1.erase( child1.begin( ) + newk + 1, child1.begin( ) + newk + largo + 1 );
        child1.insert( child1.begin( ) + newk + 1, subtour1.begin( ), subtour1.end( ) );

    } else {
        flag = true;
        do {
            I = rnd( 0, m - 7 );
            J = rnd( I, m - 5 );
            K = rnd( J, m - 3 );
            L = rnd( K, m - 1 );

            if ( ( I < J ) && ( J < K ) && ( K < L ) && ( I + 1 < J ) && ( J + 1 < K ) && ( K + 1 < L ) && ( L + 1 < m ) )
                flag = false;

        } while ( flag );
        ciudadRespaldo = child1[K];
        vector<unsigned> subtour1( child1.begin( ) + I + 1, child1.begin( ) + J + 1 );
        vector<unsigned> subtour2( child1.begin( ) + K + 1, child1.begin( ) + L + 1 );

        child1.erase( child1.begin( ) + I + 1, child1.begin( ) + J + 1 );
        child1.insert( child1.begin( ) + I + 1, subtour2.begin( ), subtour2.end( ) );

        int largo = L - K;
        int newk = -1;

        for ( int x = I + largo; x < child1.size( ); x++ )
            if ( child1[x] == ciudadRespaldo )
                newk = x;

        child1.erase( child1.begin( ) + newk + 1, child1.begin( ) + newk + largo + 1 );
        child1.insert( child1.begin( ) + newk + 1, subtour1.begin( ), subtour1.end( ) );

    }
    // Si el depot no está al comienzo lo movemos al comienzo
    if ( child1[0] != 0 ) {
        auxPutDepotBegin(child1);
    }
    return true;
}

/**
 *
 * 3-opt
 *
 * Croes, G. A. (1958). A Method for Solving Traveling-Salesman
 * Problems. Operations Research, 6(6), 791–812.
 *
 **/
bool Mutation::threeOPT( vector <unsigned> &child1, TSP *tsp1, int n )
{
    float gainActual, gainCandidato, diff;
    int Imas1, Jmas1, Kmas1, Ci = -1, Cj = -1, Ck = -1, min, op = -1;
    float valor = tsp1->evaluar( child1 );
    /* int n = child1.size() - 1; */
    min = valor;

    //Requisito mínimo para aplicar 3-opt
    if ( n < 6 )
        return false;

    for ( int i = 0; i < n - 3; i++ ) {
        for ( int j = i + 2; j < n - 1; j++ ) {
            //for(int k = j + 2; k < n + 1; k++)
            for ( int K = 0; j+2 < n && K < 10; K++ ) {

                int k = rnd( j + 2, n-1);

                Imas1 = ( i == n-1 ) ? 0 : i + 1;
                Jmas1 = ( j == n-1 ) ? 0 : j + 1;
                Kmas1 = ( k == n-1 ) ? 0 : k + 1;

                //operación 1
                gainActual = tsp1->distancia2Ciudades( child1[i], child1[Imas1] )
                                   + tsp1->distancia2Ciudades( child1[j], child1[Jmas1] )
                    + tsp1->distancia2Ciudades( child1[k], child1[Kmas1] );

                gainCandidato = tsp1->distancia2Ciudades( child1[i], child1[j] )
                    + tsp1->distancia2Ciudades( child1[Imas1], child1[k] )
                    + tsp1->distancia2Ciudades( child1[Jmas1], child1[Kmas1] );

                diff = valor - gainActual + gainCandidato;

                if ( min > diff ) {
                    op = 0;
                    Ci = i;
                    Cj = j;
                    Ck = k;
                    min = diff;
                }

                //operación 2
                gainCandidato = tsp1->distancia2Ciudades( child1[i], child1[Jmas1] )
                    + tsp1->distancia2Ciudades( child1[k], child1[Imas1] )
                    + tsp1->distancia2Ciudades( child1[j], child1[Kmas1] );

                diff = valor - gainActual + gainCandidato;

                if ( min > diff ) {
                    op = 1;
                    Ci = i;
                    Cj = j;
                    Ck = k;
                    min = diff;
                }

                //operación 3
                gainCandidato = tsp1->distancia2Ciudades( child1[i], child1[k] )
                    + tsp1->distancia2Ciudades( child1[Jmas1], child1[Imas1] )
                    + tsp1->distancia2Ciudades( child1[j], child1[Kmas1] );

                diff = valor - gainActual + gainCandidato;

                if ( min > diff ) {
                    op = 2;
                    Ci = i;
                    Cj = j;
                    Ck = k;
                    min = diff;
                }

                //operación 4
                gainCandidato = tsp1->distancia2Ciudades( child1[i], child1[Jmas1] )
                    + tsp1->distancia2Ciudades( child1[k], child1[j] )
                    + tsp1->distancia2Ciudades( child1[Imas1], child1[Kmas1] );

                diff = valor - gainActual + gainCandidato;

                if ( min > diff ) {
                    //cout << i << " " << j << " " << k << " " << diff << endl;
                    op = 3;
                    Ci = i;
                    Cj = j;
                    Ck = k;
                    min = diff;
                }
            }
        }
    }

    // probando 3-opt con break
    if ( Ci != -1 && Cj != -1 ) {
        switch ( op )
            {
            case 0:
                reverse( child1.begin( ) + Ci + 1, child1.begin( ) + Cj + 1 );
                reverse( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                break;

            case 1:
                {
                    vector<unsigned> subtour( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.erase( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.insert( child1.begin( ) + Ci + 1, subtour.begin( ), subtour.end( ) );
                    break;
                }
            case 2:
                {
                    vector<unsigned> subtour( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.erase( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    reverse( subtour.begin( ), subtour.end( ) );
                    child1.insert( child1.begin( ) + Ci + 1, subtour.begin( ), subtour.end( ) );
                    break;
                }

            case 3:
                {
                    vector<unsigned> subtour( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.erase( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    reverse( child1.begin( ) + Ci + 1, child1.begin( ) + Cj + 1 );
                    child1.insert( child1.begin( ) + Ci + 1, subtour.begin( ), subtour.end( ) );
                    break;
                }

            default:
                break;
            }
    } else {
        Ci = rnd( 0, n - 5 );
        Cj = rnd( Ci + 2, n - 3 );
        Ck = rnd( Cj + 2, n-1);
        op = rnd( 0, 3 );

        switch ( op )
            {
            case 0:
                {
                    reverse( child1.begin( ) + Ci + 1, child1.begin( ) + Cj + 1 );
                    reverse( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    break;
                }
            case 1:
                {
                    vector<unsigned> subtour( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.erase( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.insert( child1.begin( ) + Ci + 1, subtour.begin( ), subtour.end( ) );
                    break;
                }
            case 2:
                {
                    vector<unsigned> subtour( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.erase( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    reverse( subtour.begin( ), subtour.end( ) );
                    child1.insert( child1.begin( ) + Ci + 1, subtour.begin( ), subtour.end( ) );
                    break;
                }

            case 3:
                {
                    vector<unsigned> subtour( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    child1.erase( child1.begin( ) + Cj + 1, child1.begin( ) + Ck + 1 );
                    reverse( child1.begin( ) + Ci + 1, child1.begin( ) + Cj + 1 );
                    child1.insert( child1.begin( ) + Ci + 1, subtour.begin( ), subtour.end( ) );
                    break;
                }
            default:
                break;

            }

    }


    // Si el depot no está al comienzo lo movemos al comienzo
    if ( child1[0] != 0 ) {
        auxPutDepotBegin(child1);
    }
    return true;
}

vector<unsigned> Mutation::tipo3( vector <unsigned> &child1, TSP *tsp1,
                                  unsigned c, int c_i, unsigned c_prime, int c_prime_i )

{
    bool flag = true;
    vector<unsigned> subtourR, subtourS;
    float max = -1 * Infinite;
    int R = -1, R_mas_1 = -1, S = -1, S_mas_1 = -1;
    vector<unsigned> ruta;

    //Type IV
    if ( c_i > c_prime_i ) //c' ..... c
        {
            flag = false;
            for ( unsigned i = 0; i < child1.size( ); i++ ) {
                if ( flag ) {
                    subtourR.push_back( child1[i] );
                    if ( child1[i] == c )
                        flag = false;
                } else {
                    if ( child1[i] == c_prime ) {
                        flag = true;
                        subtourR.push_back( child1[i] );
                    } else
                        subtourS.push_back( child1[i] );
                }
            }
            for ( unsigned s = 0; s < subtourS.size( ); s++ ) {
                for ( unsigned r = 0; r < subtourR.size( ); r++ ) {
                    unsigned r_mas_1 = ( r == subtourR.size( ) - 1 ) ? 0 : r + 1;
                    unsigned s_mas_1 = ( s == subtourS.size( ) - 1 ) ? 0 : s + 1;
                    float gain =
                        tsp1->distancia2Ciudades( child1[s], child1[s_mas_1] ) + tsp1->distancia2Ciudades( child1[r], child1[r_mas_1] ) -
                        tsp1->distancia2Ciudades( child1[s], child1[r_mas_1] ) - tsp1->distancia2Ciudades( child1[r], child1[s_mas_1] );

                    if ( gain > max ) {
                        max = gain;
                        R = r;
                        R_mas_1 = r_mas_1;
                        S = s;
                        S_mas_1 = s_mas_1;
                    }
                }
            }
            // cout << "S: ";
            // for(unsigned i = 0; i < subtourS.size(); i++)
            // 	cout << subtourS[i] << " ";
            // cout << endl;

            // cout << "R: ";
            // for(unsigned i = 0; i < subtourR.size(); i++)
            // 	cout << subtourR[i] << " ";
            // cout << endl;



            // cout << max << " " << S << " " << S_mas_1 << " " << R << " " << R_mas_1 << endl;
            // //construir ruta
            for ( unsigned s = 0; s < subtourS.size( ); s++ ) {
                if ( s != S_mas_1 )
                    ruta.push_back( subtourS[s] );
                else {
                    for ( unsigned r = R_mas_1;; ) {
                        ( r == subtourR.size( ) ) ? r = 0 : r = r;
                        if ( r == R ) {
                            ruta.push_back( subtourR[r] );
                            ruta.push_back( subtourS[s] );
                            break;
                        } else
                            ruta.push_back( subtourR[r] );
                        r++;
                    }
                }
            }

        } else //c ..... c'
        {
            for ( unsigned i = 0; i < child1.size( ); i++ ) {
                if ( flag ) {
                    if ( child1[i] == c ) {
                        flag = false;
                        subtourS.push_back( child1[i] );
                    } else
                        subtourR.push_back( child1[i] );
                } else {

                    subtourS.push_back( child1[i] );
                    if ( child1[i] == c_prime )
                        flag = true;
                }
            }
            for ( unsigned r = 0; r < subtourR.size( ); r++ ) {
                for ( unsigned s = 0; s < subtourS.size( ); s++ ) {
                    unsigned r_mas_1 = ( r == subtourR.size( ) - 1 ) ? 0 : r + 1;
                    unsigned s_mas_1 = ( s == subtourS.size( ) - 1 ) ? 0 : s + 1;
                    float gain =
                        tsp1->distancia2Ciudades( child1[r], child1[r_mas_1] ) + tsp1->distancia2Ciudades( child1[s], child1[s_mas_1] ) -
                        tsp1->distancia2Ciudades( child1[r], child1[s_mas_1] ) - tsp1->distancia2Ciudades( child1[s], child1[r_mas_1] );

                    if ( gain > max ) {
                        max = gain;
                        R = r;
                        R_mas_1 = r_mas_1;
                        S = s;
                        S_mas_1 = s_mas_1;
                    }
                }
            }

            // cout << "R: ";
            // for(unsigned i = 0; i < subtourR.size(); i++)
            // 	cout << subtourR[i] << " ";
            // cout << endl;

            // cout << "S: ";
            // for(unsigned i = 0; i < subtourS.size(); i++)
            // 	cout << subtourS[i] << " ";
            // cout << endl;


            // cout << max << " " << R << " " << R_mas_1 << " " << S << " " << S_mas_1 << endl;
            // cout << "lalala"<< endl;
            // //construir ruta
            for ( unsigned r = 0; r < subtourR.size( ); r++ ) {
                if ( r != R_mas_1 )
                    ruta.push_back( subtourR[r] );
                else {
                    for ( unsigned s = S_mas_1;; ) {
                        ( s == subtourS.size( ) ) ? s = 0 : s = s;
                        if ( s == S ) {
                            ruta.push_back( subtourS[s] );
                            ruta.push_back( subtourR[r] );
                            break;
                        } else
                            ruta.push_back( subtourS[s] );
                        s++;
                    }
                }
            }

        }

    // cout << "N: ";
    // for(unsigned i = 0; i < ruta.size(); i++)
    //   cout << ruta[i] << " ";
    // cout << endl;

    // printf("\n");
    //   // exit(0);
    return ruta;

}

vector<unsigned> Mutation::tipo4( vector <unsigned> &child1, TSP *tsp1,
                                  unsigned c, int c_i, unsigned c_prime, int c_prime_i )
{
    bool flag = true;
    vector<unsigned> subtourR, subtourS;
    float max = -1 * Infinite;
    int R = -1, R_mas_1 = -1, S = -1, S_mas_1 = -1;
    vector<unsigned> ruta;

    //Type IV
    if ( c_i > c_prime_i ) //c' ..... c
        {
            flag = false;
            for ( unsigned i = 0; i < child1.size( ); i++ ) {
                if ( flag ) {
                    if ( child1[i] == c ) {
                        flag = false;
                        subtourS.push_back( child1[i] );
                    } else
                        subtourR.push_back( child1[i] );
                } else {
                    subtourS.push_back( child1[i] );
                    if ( child1[i] == c_prime )
                        flag = true;
                }
            }
            for ( unsigned s = 0; s < subtourS.size( ); s++ ) {
                for ( unsigned r = 0; r < subtourR.size( ); r++ ) {
                    unsigned r_mas_1 = ( r == subtourR.size( ) - 1 ) ? 0 : r + 1;
                    unsigned s_mas_1 = ( s == subtourS.size( ) - 1 ) ? 0 : s + 1;
                    float gain =
                        tsp1->distancia2Ciudades( child1[s], child1[s_mas_1] ) + tsp1->distancia2Ciudades( child1[r], child1[r_mas_1] ) -
                        tsp1->distancia2Ciudades( child1[s], child1[r_mas_1] ) - tsp1->distancia2Ciudades( child1[r], child1[s_mas_1] );

                    if ( gain > max ) {
                        max = gain;
                        R = r;
                        R_mas_1 = r_mas_1;
                        S = s;
                        S_mas_1 = s_mas_1;
                    }
                }
            }
            // cout << "S: ";
            // for(unsigned i = 0; i < subtourS.size(); i++)
            // 	cout << subtourS[i] << " ";
            // cout << endl;

            // cout << "R: ";
            // for(unsigned i = 0; i < subtourR.size(); i++)
            // 	cout << subtourR[i] << " ";
            // cout << endl;



            // cout << max << " " << S << " " << S_mas_1 << " " << R << " " << R_mas_1 << endl;
            //construir ruta
            for ( unsigned s = 0; s < subtourS.size( ); s++ ) {
                if ( s != S_mas_1 )
                    ruta.push_back( subtourS[s] );
                else {
                    for ( unsigned r = R_mas_1;; ) {
                        ( r == subtourR.size( ) ) ? r = 0 : r = r;
                        if ( r == R ) {
                            ruta.push_back( subtourR[r] );
                            ruta.push_back( subtourS[s] );
                            break;
                        } else
                            ruta.push_back( subtourR[r] );
                        r++;
                    }
                }
            }

        } else //c ..... c'
        {
            for ( unsigned i = 0; i < child1.size( ); i++ ) {
                if ( flag ) {
                    subtourR.push_back( child1[i] );
                    if ( child1[i] == c )
                        flag = false;
                } else {

                    if ( child1[i] == c_prime ) {
                        flag = true;
                        subtourR.push_back( child1[i] );
                    } else
                        subtourS.push_back( child1[i] );
                }
            }
            for ( unsigned r = 0; r < subtourR.size( ); r++ ) {
                for ( unsigned s = 0; s < subtourS.size( ); s++ ) {
                    unsigned r_mas_1 = ( r == subtourR.size( ) - 1 ) ? 0 : r + 1;
                    unsigned s_mas_1 = ( s == subtourS.size( ) - 1 ) ? 0 : s + 1;
                    float gain =
                        tsp1->distancia2Ciudades( child1[r], child1[r_mas_1] ) + tsp1->distancia2Ciudades( child1[s], child1[s_mas_1] ) -
                        tsp1->distancia2Ciudades( child1[r], child1[s_mas_1] ) - tsp1->distancia2Ciudades( child1[s], child1[r_mas_1] );

                    if ( gain > max ) {
                        max = gain;
                        R = r;
                        R_mas_1 = r_mas_1;
                        S = s;
                        S_mas_1 = s_mas_1;
                    }
                }
            }

            // cout << "R: ";
            // for(unsigned i = 0; i < subtourR.size(); i++)
            // 	cout << subtourR[i] << " ";
            // cout << endl;

            // cout << "S: ";
            // for(unsigned i = 0; i < subtourS.size(); i++)
            // 	cout << subtourS[i] << " ";
            // cout << endl;


            // cout << max << " " << R << " " << R_mas_1 << " " << S << " " << S_mas_1 << endl;

            //construir ruta
            for ( unsigned r = 0; r < subtourR.size( ); r++ ) {
                if ( r != R_mas_1 )
                    ruta.push_back( subtourR[r] );
                else {
                    for ( unsigned s = S_mas_1;; ) {
                        ( s == subtourS.size( ) ) ? s = 0 : s = s;
                        if ( s == S ) {
                            ruta.push_back( subtourS[s] );
                            ruta.push_back( subtourR[r] );
                            break;
                        } else
                            ruta.push_back( subtourS[s] );
                        s++;
                    }
                }
            }

        }

    // cout << "N: ";
    // for(unsigned i = 0; i < ruta.size(); i++)
    //   cout << ruta[i] << " ";
    // cout << endl;

    // printf("\n");
    return ruta;
    // exit(0);
}

/**
 *
 * Neighbor-Join (NJ)
 *
 * Solving Traveling Salesman Problems by Combining Global and Local Search
 * Mechanisms
 *
 **/
bool Mutation::NJ( vector <unsigned> &child1, individuos *pop, TSP *tsp1, unsigned sizePop )
{
    // printf("\n\n\n");
    // for(int i = 0; i < child1.size(); i++)
    //   cout << child1[i] << " ";
    // cout << endl;

    float probabilidad = 0.5;
    unsigned c, c_prime;


    unsigned NLmax = 3; //ciudades vecinas

    unsigned L = 5; // Número de iteraciones
    for ( unsigned l = 0; l < L; l++ ) {
        vector<unsigned> type1 = child1, type2 = child1, type3, type4;
        //seleccionar c
        unsigned random = rnd( 0, sizeChrm - 1 );
        c = child1[random];
        int c_i = find( child1.begin( ), child1.end( ), c ) - child1.begin( );

        //selecionar c'
        if ( flip( probabilidad ) ) {
            do {
                unsigned j = rnd( 0, sizePop - 1 );
                int k = find( pop[j].chrm.begin( ), pop[j].chrm.end( ), c ) - pop[j].chrm.begin( );

                c_prime = ( k == sizeChrm - 1 ) ? pop[j].chrm[0] : pop[j].chrm[k + 1];
                // j = find(child1.begin(), child1.end(), c_prim) - child1.begin();

            } while ( c_prime == c );
        } else {
            vector<pair <int, float> > distR1;
            vector<unsigned> child11 = child1;
            for ( unsigned i = 0; i < sizeChrm; i++ )
                if ( child1[c_i] != i ) {
                    pair <int, float> city( i, tsp1->distancia2Ciudades( child1[c_i], i ) );
                    distR1.push_back( city );
                }
            sort( distR1.begin( ), distR1.end( ), compare_pair_second<std::less>( ) );
            unsigned j = rnd( 0, NLmax - 1 );
            c_prime = find( child1.begin( ), child1.end( ), distR1[j].first ) - child1.begin( );
        }

        // cout << "c= " << c << " c'= " << c_prime << endl;


        int c_prime_i = find( child1.begin( ), child1.end( ), c_prime ) - child1.begin( );

        if ( abs( c_prime_i - c_i ) < 2 ||
             ( c_prime_i == 0 && c_i == child1.size( ) - 1 ) ||
             ( c_i == 0 && c_prime_i == child1.size( ) - 1 ) ) {
        } else {
            //Tipo 1: c es fijo
            if ( c_i > c_prime_i ) //c' ..... c
                {
                    // cout << "reverse" << endl;
                    reverse( type1.begin( ) + c_prime_i, type1.begin( ) + c_i );
                    // for(int i = 0; i < type1.size(); i++)
                    // 	cout << type1[i] << " ";
                    // cout << endl;
                } else {
                // cout << "reverse" << endl;
                reverse( type1.begin( ) + c_i + 1, type1.begin( ) + c_prime_i + 1 );
                // for(int i = 0; i < type1.size(); i++)
                // 	cout << type1[i] << " ";
                // cout << endl;

            }
            //Tipo 2: c' es fijo
            if ( c_i > c_prime_i ) //c' ..... c
                {
                    // cout << "reverse" << endl;
                    reverse( type2.begin( ) + c_prime_i + 1, type2.begin( ) + c_i + 1 );
                    // for(int i = 0; i < type2.size(); i++)
                    // 	cout << type2[i] << " ";
                    // cout << endl;
                } else {
                // cout << "reverse" << endl;
                reverse( type2.begin( ) + c_i, type2.begin( ) + c_prime_i );
                // for(int i = 0; i < type2.size(); i++)
                // 	cout << type2[i] << " ";
                // cout << endl;

            }
            type3 = tipo3( child1, tsp1, c, c_i, c_prime, c_prime_i );
            type4 = tipo4( child1, tsp1, c, c_i, c_prime, c_prime_i );

            float valorT[4];

            valorT[0] = tsp1->evaluar( type1 );
            valorT[1] = tsp1->evaluar( type2 );
            valorT[2] = tsp1->evaluar( type3 );
            valorT[3] = tsp1->evaluar( type4 );

            // cout << valor << endl;

            // for(unsigned i = 0; i < 4; i++)
            //   cout << valorT[i] << endl;

            int best = -1;
            float min = Infinite;
            // int valor = tsp1->evaluar(child1);
            // unsigned min = valor;
            for ( unsigned i = 0; i < 4; i++ )
                if ( min > valorT[i] ) {
                    best = i;
                    min = valorT[i];
                }

            // if(best != -1)
            // {
            switch ( best )
                {
                case 0:
                    child1 = type1;
                    break;
                case 1:
                    child1 = type2;
                    break;
                case 2:
                    child1 = type3;
                    break;
                case 3:
                    child1 = type4;
                    break;
                }
            // }

            // cout << valor << endl;
            // int final = tsp1->evaluar(child1);
            // cout << final<<" final\n";
            // for(unsigned i = 0; i < 4; i++)
            //   cout << valorT[i] << endl;

            // exit(0);
        }
    }

    return true;
}

/**
 * Operador de mutación basado en una local search
 *
 */

bool Mutation::LS1( vector <unsigned> &child1, TSP *tsp1, int n )
{
    set < int > coversAux; // set con los todos los customers cubiertos
    int newP = conjuntoConCustomersAll(&coversAux, &associations, child1, P); // sin la ciudad que se desea reemplazar
    if(newP != -1) // Se actualiza
        n = newP;

    set < int > coversOriginal;//coversOriginal.clear();
    int reemplazar = rnd( 0, n - 1); // Se escoge una ciudad al azar
    int ciudad = child1[reemplazar]; // ciudad candidata a cambiar
    conjuntoConCustomers(&coversOriginal, &associations, child1, ciudad, n); // sin la ciudad que se desea reemplazar
    // Si se cumple mejor quitar la ciudad
    if(coversOriginal.size() >= P)
    {
        child1.erase( child1.begin( ) + reemplazar );
        child1.push_back(ciudad); // La agregamos al final
        return true;
    }
    // cout << "quedo con " << coversOriginal.size() << endl;
    int ciudad_candidata_pos = -1;
    int ciudad_ant = ( reemplazar > 0 ) ?  child1[reemplazar - 1] : 0; //Si reemplazar es 0 la ciudad anterior es el deposito
    int ciudad_sig = ( reemplazar < child1.size() - 1) ?  child1[reemplazar + 1] : 0; //Si reemplazar es largoTotal la ciudad siguiente es el deposito
    float costo_original = tsp1->distancia2Ciudades(ciudad_ant, ciudad) + tsp1->distancia2Ciudades(ciudad, ciudad_sig);
    // intenta cambiar la ciudad el azar por cualquiera que no esté en la ruta
	for(int i = n; i < child1.size(); i++)
        {
            set < int > covers(coversOriginal.begin(), coversOriginal.end());
            int ciudad_nueva = child1[i];
            agregar_customers_alConjunto(&covers, &associations, ciudad_nueva);
            float costo_nuevo = tsp1->distancia2Ciudades(ciudad_ant, ciudad_nueva) + tsp1->distancia2Ciudades(ciudad_nueva, ciudad_sig);
            // Si es menor y además cumple el P, es una ciudad candidata
            if(covers.size() >= P && costo_nuevo < costo_original)
                {
                    ciudad_candidata_pos = i;
                    costo_original = costo_nuevo;
                }
        }
    // Si existe una ciudad mejor se reemplaza
    if(ciudad_candidata_pos != -1)
        {
            int aux = child1[reemplazar];
            child1[reemplazar] = child1[ciudad_candidata_pos];
            child1[ciudad_candidata_pos] = aux;
            return true;
        }
    else if ( flip( 0.5 ) ) // cambiar por una cualquiera con un porcentaje bajo
        {
            reemplazar = rnd(0, n);
            ciudad_candidata_pos = rnd(n, child1.size() - 1);
            int aux = child1[reemplazar];
            child1[reemplazar] = child1[ciudad_candidata_pos];
            child1[ciudad_candidata_pos] = aux;
            return true;
        }
    return false;
}


/**
 * Operador de mutación basado en una local search con mayor intensidad.
 *
 */

bool Mutation::LS2( vector <unsigned> &child1, TSP *tsp1, int n )
{
    set < int > coversAux; // set con los todos los customers cubiertos
    int newP = conjuntoConCustomersAll(&coversAux, &associations, child1, P); // sin la ciudad que se desea reemplazar
    if(newP != -1) // Se actualiza el n
        n = newP;

    // se aleatoriza el orden de recorrer el cromosoma
    vector<unsigned> random(fijo.begin(), fijo.begin() + n);
    // int rand = rnd(0, 9);
    // if(rand % 3 != 0)
    random_shuffle(random.begin(), random.end());

    bool flag = false; // para verificar si hay cambio
    //Se recorrerá para cada ciudad, y se modificará hasta que se cumpla P.
    for( int j = 0 ; j < n ; j++ )
    {
        // Para recorrer de forma desordenada
    	int reemplazar = random[j]; // Se escoge una ciudad
    	int ciudad = child1[reemplazar]; // ciudad candidata a cambiar
        set < int > coversOriginal;
        conjuntoConCustomers(&coversOriginal, &associations, child1, ciudad, n); // sin la ciudad que se desea reemplazar
    	// Si se cumple mejor quitar la ciudad
	    if(coversOriginal.size() >= P)
	    {
            child1.erase( child1.begin( ) + reemplazar );
            child1.push_back(ciudad); //La agregamos al final
            n--;
            continue;
	    }

	    int ciudad_candidata_pos = -1;
	    int ciudad_ant = ( reemplazar > 0 ) ? child1[reemplazar - 1] : 0; //Si reemplazar es 0 la ciudad anterior es el deposito
	    int ciudad_sig = ( reemplazar < child1.size() - 1) ? child1[reemplazar + 1] : 0;//Si reemplazar es largoTotal la ciudad siguiente es el deposito
	    float costo_original = tsp1->distancia2Ciudades(ciudad_ant, ciudad) + tsp1->distancia2Ciudades(ciudad, ciudad_sig);
	    // intenta cambiar la ciudad el azar por cualquiera que no esté en la ruta
	    for(int i = n; i < child1.size(); i++)
	    {
	        set < int > covers(coversOriginal.begin(), coversOriginal.end());
	        int ciudad_nueva = child1[i];
	        agregar_customers_alConjunto(&covers, &associations, ciudad_nueva);
	        float costo_nuevo = tsp1->distancia2Ciudades(ciudad_ant, ciudad_nueva) + tsp1->distancia2Ciudades(ciudad_nueva, ciudad_sig);
	        // Si es menor y además cumple el P, es una ciudad candidata
            if ( flip( 0.5 ) )
                {
                    if(covers.size() >= P && costo_nuevo < costo_original)
                        {
                            ciudad_candidata_pos = i;
                            costo_original = costo_nuevo;
                            break;
                        }
                }
	    }

	     // Si existe una ciudad mejor se reemplaza
	    if(ciudad_candidata_pos != -1)
	    {
	        int aux = child1[reemplazar];
	        child1[reemplazar] = child1[ciudad_candidata_pos];
	        child1[ciudad_candidata_pos] = aux;
            flag = true;
	    }
    }
    if(flag)
        return true;

    return false;
}

/**
 * Operador de mutación basado en una local search TODA intensidad posible 1313. Le dicen el Nico.
 *
 */

bool Mutation::LS3( vector <unsigned> &child1, TSP *tsp1, int n )
{
    // for(int i = 0; i < child1.size(); i++)
    //     cout << child1[i] << " ";
    // cout << endl;

    set < int > coversAux; // set con los todos los customers cubiertos

    int newP = conjuntoConCustomersAll(&coversAux, &associations, child1, P); // sin la ciudad que se desea reemplazar
    if(newP != -1) // Se actualiza el n
        n = newP;

    // cout << "Costo Original es "  << tsp1->evaluar(child1, n) << endl;
    // cout << endl;
    // se aleatoriza el orden de recorrer el cromosoma
    vector<unsigned> random(fijo.begin(), fijo.begin() + n);
    int rand = rnd(0, 9);
    if(rand % 3 != 0)
        random_shuffle(random.begin(), random.end());

    bool flag = false; // para verificar si hay cambio
    //Se recorrerá para cada ciudad, y se modificará hasta que se cumpla P.
    for( int j = 0 ; j < n ; j++ )
        {
            // Para recorrer de forma desordenada
            int reemplazar = random[j]; // Se escoge una ciudad
            int ciudad = child1[reemplazar]; // ciudad candidata a cambiar
            set < int > coversOriginal;
            conjuntoConCustomers(&coversOriginal, &associations, child1, ciudad, n); // sin la ciudad que se desea reemplazar
            // Si se cumple mejor quitar la ciudad
            if(coversOriginal.size() >= P)
                {
                    // cout << "remove " << ciudad << endl;
                    // cout << "n is " << n << endl;
                    child1.erase( child1.begin( ) + reemplazar );
                    child1.push_back(ciudad); //La agregamos al final
                    n--;
                    // for(int i = 0; i < child1.size(); i++)
                    //     cout << child1[i] << " ";
                    // cout << endl;
                    // cout << "Costo es "  << tsp1->evaluar(child1, n) << endl;
                    continue;
                }

            int ciudad_candidata_pos = -1;
            int ciudad_ant = ( reemplazar > 0 ) ? child1[reemplazar - 1] : 0; //Si reemplazar es 0 la ciudad anterior es el deposito
            int ciudad_sig = ( reemplazar < child1.size() - 1) ? child1[reemplazar + 1] : 0;//Si reemplazar es largoTotal la ciudad siguiente es el deposito
            float costo_original = tsp1->distancia2Ciudades(ciudad_ant, ciudad) + tsp1->distancia2Ciudades(ciudad, ciudad_sig);
            // intenta cambiar la ciudad el azar por cualquiera que no esté en la ruta
            for(int i = n; i < child1.size(); i++)
                {
                    set < int > covers(coversOriginal.begin(), coversOriginal.end());
                    int ciudad_nueva = child1[i];
                    agregar_customers_alConjunto(&covers, &associations, ciudad_nueva);
                    float costo_nuevo = tsp1->distancia2Ciudades(ciudad_ant, ciudad_nueva) + tsp1->distancia2Ciudades(ciudad_nueva, ciudad_sig);
                    // Si es menor y además cumple el P, es una ciudad candidata
                    if(covers.size() >= P && costo_nuevo < costo_original)
                        {
                            ciudad_candidata_pos = i;
                            costo_original = costo_nuevo;
                        }
                }

            // Si existe una ciudad mejor se reemplaza
            if(ciudad_candidata_pos != -1)
                {
                    // cout << "change"<< endl;
                    // cout << "n is " << n << endl;
                    // cout << child1[ciudad_candidata_pos] << endl;
                    // cout << child1[reemplazar] << endl;
                    int aux = child1[reemplazar];
                    child1[reemplazar] = child1[ciudad_candidata_pos];
                    child1[ciudad_candidata_pos] = aux;
                    flag = true;

                    // for(int i = 0; i < child1.size(); i++)
                    //     cout << child1[i] << " ";
                    // cout << endl;
                    //cout << "Costo es "  << tsp1->evaluar(child1, n) << endl;
                }
        }


    // for(int i = 0; i < child1.size(); i++)
    //     cout << child1[i] << " ";
    // cout << endl;
    // cout << "Costo es "  << tsp1->evaluar(child1, n) << endl;


    // exit(0);

    if(flag)
        return true;
    else if ( flip( 0.5 ) ) // cambiar por una cualquiera con un porcentaje bajo
        {

            int reemplazar = rnd(0, n-1);
            int ciudad_candidata_pos = rnd(n-1, child1.size() - 1);
            int aux = child1[reemplazar];
            child1[reemplazar] = child1[ciudad_candidata_pos];
            child1[ciudad_candidata_pos] = aux;
            return true;
        }
    return false;
}


bool Mutation::LS4( vector <unsigned> &child1, TSP *tsp1, int n )
{
    set < int > coversOriginal; // set con los todos los customers cubiertos
    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, child1, P); // sin la ciudad que se desea reemplazar
    if(newP != -1) // Se actualiza el n
        n = newP;

    // se aleatoriza el orden de recorrer el cromosoma
    vector<unsigned> random(fijo.begin(), fijo.begin() + n);
    int rand = rnd(0, 9);
    if(rand != 0)
        random_shuffle(random.begin(), random.end());

    //Se recorrerá para cada ciudad, y se modificará hasta que se cumpla P.
    for( int j = 0 ; j < n ; j++ )
    {
        // Para recorrer de forma desordenada
    	int reemplazar = random[j]; // Se escoge una ciudad
    	int ciudad = child1[reemplazar]; // ciudad candidata a cambiar
        set < int > covers;
        conjuntoConCustomers(&covers, &associations, child1, ciudad, n); // sin la ciudad que se desea reemplazar
    	// Si se cumple mejor quitar la ciudad
	    if(covers.size() >= P)
	    {
            child1.erase( child1.begin( ) + reemplazar );
            child1.push_back(ciudad); //La agregamos al final;
            n--;
	    }

    }
    return true;
}

//acuerdate de ajustar el N para los otros operadores 2-opt, 3-opt, etc
bool Mutation::LKH( vector <unsigned> &child1, TSP *tsp1, int n )
{
    // Si es menos de 3 ciudades no se puede hacer LKH
    if(n < 3)
        return false;

    // Formato para usar la biblioteca de LKH
    stringstream stream2;
    stringstream stream3;
    stream2 << "Giant_Parameters" << 1 <<".txt";
    stream3 << "Giant_Tour" << 1 <<".txt";
    string parametro2 = stream2.str();
    string parametro3 = stream3.str();
    ofstream Parameters((parametro2).c_str());
    Parameters << "PROBLEM_FILE = lala.tsp" << endl;
    Parameters << "TRACE_LEVEL = 0" << endl;
    Parameters << "GAIN23 = NO" << endl;
    Parameters << "RUNS = 1" << endl;
    Parameters << "OUTPUT_TOUR_FILE " << parametro3 << endl;
    Parameters.close();
    char dll_exe[] = "lkh_dll";
    char dll_par[1024];
    strcpy(dll_par, ((parametro2).c_str()));
    char *dll_argv[] = { dll_exe , dll_par };

    float **coord;
    coord = (float **) malloc(n * sizeof(float *));
    for(int i = 0; i < n; i++)
        coord[i] = (float *) malloc(2 * sizeof(float));

    // Acá se guarda la ruta del LKH
    vector<int> tour(n, 0);
    int ciudad;

    // Agregar Depot
    coord[0][0] = tsp1->pt[0].x;
    coord[0][1] = tsp1->pt[0].y;
    // Generar la entrada de las coordenadas para LKH
    for(unsigned i = 0; i < n - 1; i++)
        {
            ciudad = child1[i];
            coord[i+1][0] = tsp1->pt[ciudad].x;
            coord[i+1][1] = tsp1->pt[ciudad].y;
        }

    lkh_dll(2 , dll_argv, &tour[0], coord, n);

    // copias de tour y child
    vector<int> child1Aux(tour.begin(), tour.end());
    vector<unsigned> child2Aux(child1.begin(), child1.end());

    // Mueve la ruta para que comience desde el depot (0)
    // Search depot position
    int j = 0;
    for ( int i = 0; i < tour.size(); i++ )
        if ( tour[i] == 1 ) {
            j = i - 1; // Para poder poner el j++ después
            break;
        }
    // Move customer until Depot put on position 0
    for ( int i = 0; i < tour.size(); i++ ) {
        j = (j == tour.size() - 1) ? 0 : j+1;
        tour[i] = child1Aux[j];
    }

    // Copiar al Child1
    for(int i = 1; i < n; i++)
        child1[i-1] = child2Aux[tour[i]-2];

    for(int i = 0; i < n; i++)
        free(coord[i]);
    free(coord);

    return true;
}