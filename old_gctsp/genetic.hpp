/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * genetic.hpp
 * This file is part of AG-GCTSP
 *
 * Copyright (C) 2012 - Carlos Contreras Bolton <ccontreras.bolton@gmail.com>
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

#ifndef _GENETIC_H
#define	_GENETIC_H

#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <sstream>

#include <boost/random/discrete_distribution.hpp>

#include "crossover.hpp"
#include "mutation.hpp"
#include "selection.hpp"
#include "initpop.hpp"
#include "tsp.hpp"

class AGenetic : public Util {
public:
    double optimo;
    double tiempo; /* Tiempo transcurrido por el AG */
    struct individuos *pop; /* Inidividuos actuales */
    struct individuos *newpop; /* nuevos Inidividuos  */
    struct individuos bestfit; /* El mejor Inidividuo  */
    AGenetic( unsigned sizePop, float pcross, float pmut, unsigned seed, string nom_instance,
              double optimo, string ruta_resultados, int hebras, unsigned ejecucion );
    AGenetic( unsigned seed, string path_instance, string ruta_resultados, int hebras );
    virtual ~AGenetic( );
    vector <unsigned> evolution( unsigned nGen );
    vector <unsigned> ILS( unsigned nGen );
    void cargarInstancias( );
private:
    unsigned sizeChrm; /* tamaño del cromosoma binario */
    unsigned sizePop; /* tamaño de la poblacion */
    int gen; /* actual generation number */
    unsigned maxgen; /* maximum generation number */
    unsigned seed; /* valor de la semilla */
    float pcross; /* probability of crossover */
    float pmut; /* probability of mutation */
    int ncross; /* numero de cruzamientos*/
    int nmutation; /* numero de mutaciones*/
    double max; /* maximum fitness of population */
    double min; /* minumum fitness of population */
    double med; /* medium  fitness of population */
    double varianza; /* Varianza de la poblacion */
    double lowerBound;
    float aptitud; /* fitness del individuo */
    unsigned ejecucion;
    string nom_instance;
    string ruta_resultados; /* Ruta completa del directorio donde quedarán los archivos resultados */
    int hebras;
    vector<unsigned> fijo;
    double tiempoGen; /* Tiempo de cada generación */
    struct individuos *tempold; /* Inidividuos temporales  */
    /* Para porcentajes de participación de cada operador*/
    boost::random::discrete_distribution<> distSel;
    boost::random::discrete_distribution<> distMut;
    boost::random::discrete_distribution<> distCross;
    boost::random::discrete_distribution<> dist2daMut;
    boost::random::discrete_distribution<> dist2daCross;
    boost::array <double, 3> porcentage0;
    boost::array <double, 16> porcentage11;
    boost::array <double, 16> porcentage12;
    boost::array <double, 16> porcentage21;
    boost::array <double, 16> porcentage22;
    node *grafo;
    node *depot;
    int nSuppliers;
    //Contenedor con la información de los "suppliers" (objetos)
    set < node * > suppliers;
    //Contenedor con la información de los "customers" (objetos)
    set < node * > customers;
    //Asociaciones entre customers y suppliers (ids)
    map < int, set <int> > associations;

    TSP *tsp;
    Crossover *cross;
    Mutation *mut;
    Selection *sel;
    Initpop *initpop;
    // int rnd(unsigned low, unsigned high);
    // bool flip(float prob);

    //Solo para validar operadores
    int validar;
    void validarOp( );
    //operador de seleccion
    unsigned select( );
    //operador de cruzamiento
    void crossover( vector <unsigned> &child1, vector <unsigned> &child2, int n, int m );
    //operador de mutación
    void mutation( vector <unsigned> &child1, int n ); // , vector <unsigned> &child2);

    //operador de elitismo
    void elitism( );

    //operador de poblacion inicial
    void initPop( );

    void statistics( struct individuos *pop );
    void funcionObjetivo( struct individuos *indiv );
    void genera_resultados_generacion( );
    void genera_reporte( );
    void genera_archivo_chrm( );
    void genera_resultados_layout( );
    void barra_progreso( float time );


    double evaluar(int who);
    void removal(int who);
    //función auxiliares para reparar cromosomas
    void reparar(vector <unsigned> &child1, vector <unsigned> &childNew1);
    // idema arriba pero sin depot
    void reparar2(vector <unsigned> &child1, vector <unsigned> &childNew1);
};



#endif	/* _GENETIC_H */
