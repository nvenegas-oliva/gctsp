/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/*
 * genetic.cpp
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
#include <omp.h>
#include "genetic.hpp"

/**
 *
 * Constructor de la clase AGenetic para La evolucion Genetica
 *
 **/
AGenetic::AGenetic( unsigned sizePop, float pcross, float pmut, unsigned seed,
                    string nom_instance, double optimo, string ruta_resultados,
                    int hebras, unsigned ejecucion )
{
    this->sizePop = sizePop;
    this->pcross = pcross;
    this->pmut = pmut;
    this->seed = seed;
    this->ruta_resultados = ruta_resultados;
    this->nom_instance = nom_instance;
    this->hebras = hebras;
    this->ejecucion = ejecucion;
    this->optimo = optimo;

    //inicializa variables
    ncross = 0;
    nmutation = 0;
    bestfit.fitness = 9999999999999.9999;

    //srand(seed);
    generator.seed( seed );

    // Inicializar poblacion
    pop = new individuos[sizePop];
    newpop = new individuos[sizePop];
    hebras = 1;

    //Carga de archivo
    string instance = "instancias/" + nom_instance;

    // cout << instance << endl;
    grafo = new node( );
    depot = new node( );

    P = grafo->load( instance.c_str( ), &suppliers, &customers, &associations, depot );
    nSuppliers = suppliers.size( );

    vector<vector <float> > pt ( suppliers.size() + 1, vector<float>( 2, 0 ));
    grafo->getSuppliers( suppliers, depot->getX( ), depot->getY( ), pt);

    // cout << "P parameter: "<< P << endl;
    // cout << "Suppliers: " << endl;
    // grafo->show_container( suppliers );
    // cout << "Customers: " << endl;
    // grafo->show_container( customers );
    // grafo->show_associations( associations );
    // cout << "\n\tdepot CERO.getX() =" << depot->getX() << endl;
    // cout << "\tdepot CERO.getY() =" << depot->getY() << endl;

    // exit(0);

    // Inicializar problema TsP
    tsp = new TSP( nSuppliers );

    tsp->leerInstancia( pt, nSuppliers );

    //grafo = new Grafo;//*[hebras];

    //for(int i = 0; i < hebras; i++)
    //{
    // grafo = new Grafo(optimo);
    // grafo->Inicializa("instancias/" + nom_instance);
    //}

    sizeChrm = nSuppliers; // Se incluye el depot

    size_t loc = nom_instance.rfind( "/" );

    if ( loc != string::npos ) {
        loc++;
        nom_instance = nom_instance.substr( loc );
    } else
        nom_instance = nom_instance;
    this->nom_instance = nom_instance;

    // Inicializador de operadores
    initpop = new Initpop( sizeChrm, nSuppliers, seed, associations );
    sel = new Selection( sizePop, seed );
    cross = new Crossover( sizeChrm, sizePop, seed, associations );
    mut = new Mutation( sizeChrm, pmut, seed, associations );

    boost::array <double, 3> probability0 = {
        {
            0.20, // 0  Random_LKH
            0.35, // 1  NNH
            0.45  // 2  NNC
        }
    };
    boost::array <double, 16> probability1 = {
        {
            0.0000, // 0  PMX
            0.0144, // 1  OX1
            0.0337, // 2  OX2
            0.0337, // 3  MOX
            0.0000, // 4  POS
            0.0000, // 5  CX
            0.2837, // 6  DPX
            0.0577, // 7  AP
            0.0000, // 8  MPX
            0.1691, // 9  HX
            0.0000, // 10 IO
            0.0240, // 11 MIO
            0.0096, // 12 VR
            0.1000, // 13 adhoc
            0.1683, // 14 ER
            0.1058  // 15 GSTX
        }
    };
    boost::array <double, 16> probability2 = {
        {
            0.0000, // 0  EM
            0.0559, // 1  SIM
            0.0335, // 2  SM
            0.0000, // 3  DM
            0.0168, // 4  IVM
            0.0000, // 5  ISM
            0.0391, // 6  DBM
            0.0000, // 7  LS1
            0.1620, // 8  LS2
            0.2184, // 9  LS3
            0.1000, // 10 LS4
            0.3408, // 11 LKH
            0.0223, // 12 2opt
            0.0000, // 13 SHMO
            0.0000, // 14 DBM2
            0.0112, // 15 3opt
        }
    };
    // 2da etapa de cruzamiento
    boost::array <double, 16> probability3 = {
        {
            0.1027, // 0  PMX
            0.0000, // 1  OX1
            0.0000, // 2  OX2
            0.1164, // 3  MOX
            0.0000, // 4  POS
            0.0000, // 5  CX
            0.0000, // 6  DPX
            0.0068, // 7  AP
            0.0137, // 8  MPX
            0.0069, // 9  HX
            0.0000, // 10 IO
            0.2630, // 11 MIO
            0.0000, // 12 VR
            0.1000, // 13 adhoc
            0.0205, // 14 ER
            0.3700  // 15 GSTX
        }
    };
    // 2da etapa de mutación
    boost::array <double, 16> probability4 = {
        {
            0.0413, // 0  EM
            0.1268, // 1  SIM
            0.0000, // 2  SM
            0.0472, // 3  DM
            0.0914, // 4  IVM
            0.0472, // 5  ISM
            0.0650, // 6  DBM
            0.0236, // 7  LS1
            0.0472, // 8  LS2
            0.1416, // 9  LS3
            0.0000, // 10 LS4
            0.1681, // 11 LKH
            0.1800, // 12 2opt
            0.0177, // 13 SHMO
            0.0000, // 14 DBM2
            0.0029, // 15 3opt
        }
    };

    boost::random::discrete_distribution<> dist1( probability1.begin( ), probability1.end( ) );
    boost::random::discrete_distribution<> dist2( probability2.begin( ), probability2.end( ) );

    distCross = dist1;
    distMut = dist2;

    boost::random::discrete_distribution<> dist3( probability3.begin( ), probability3.end( ) );
    boost::random::discrete_distribution<> dist4( probability4.begin( ), probability4.end( ) );

    dist2daCross = dist3;
    dist2daMut = dist4;

    boost::random::discrete_distribution<> dist0( probability0.begin( ), probability0.end( ) );

    distSel = dist0;

    porcentage0 = probability0;
    porcentage11 = probability1;
    porcentage12 = probability3;
    porcentage21 = probability2;
    porcentage22 = probability4;
    // INICIO - SOLO PARA VALIDAR !!!
    // Solo para validar Operadores
    // int sum = 0;
    // for(int j = 0; j < sizeChrm; j++)
    //     sum += j+1;

    // validar = sum;
    // cout << endl <<  " validar es " << endl << validar << endl;
    // FIN - SOLO PARA VALIDAR !!!
}

/**
 *
 * Constructor de la clase AGenetic para la Evaluacion de un solo individuo
 * NO OPERATIVO ARREGLAR
 **/
AGenetic::AGenetic( unsigned seed, string nom_instance,
                    string ruta_resultados, int hebras )
{
    this->seed = seed;
    this->nom_instance = nom_instance;
    this->ruta_resultados = ruta_resultados;
    this->hebras = hebras;
    bestfit.fitness = 9999999999999.9999;
    pop = new individuos[1];
    newpop = new individuos[1];

    // srand(seed);
    generator.seed( seed );

}

/**
 *
 * Destructor de la clase AGenetic
 *
 **/
AGenetic::~AGenetic( )
{
    delete [] pop;
    delete [] newpop;

    delete initpop;
    delete sel;
    delete cross;
    delete mut;

    delete depot;

    for ( set < node * >::iterator iter = suppliers.begin( ); iter != suppliers.end( ); iter++ )
        delete *iter;

    for ( set < node * >::iterator iter = customers.begin( ); iter != customers.end( ); iter++ )
        delete *iter;

    delete grafo;

    delete tsp;

    //liberamos memoria del dtp
    //for(unsigned i = 0; i < hebras; i++)
    //delete grafo;
    // delete grafo;
    // delete depot;
}

/**
 *
 * Funcion que carga instancias
 *
 **/
void AGenetic::cargarInstancias( )
{

}

void AGenetic::validarOp( )
{
    int sum = 0;
    for ( int i = 0; i < sizePop; i++ ) {
        sum = 0;
        for ( int j = 0; j < sizeChrm; j++ )
            sum += pop[i].chrm[j];


        if ( sum != validar ) {
            cout << "El operador genera una infactibilidad" << endl;
            cout << "La suma es " << validar << "y da " << sum << endl;
            cout << "Este es el cromosoma" << endl;
            recorreVector(pop[i].chrm);
            exit( 0 );
        }
    }

}

int comparaFitness( const void *Indi, const void *Indj )
{
    struct AGenetic::individuos *i = ( struct AGenetic::individuos * ) Indi;
    struct AGenetic::individuos *j = ( struct AGenetic::individuos * ) Indj;

    if ( i->fitness > j->fitness )
        return 1;
    if ( i->fitness < j->fitness )
        return -1;
    return 0;
}

/**
 * Función que realiza el operador de elitismo, que consiste en tomar los X% mejores individuos de
 * la generación anterior y reemplazarlos por los peores de la nueva generación.
 *
 */

void AGenetic::elitism( )
{
    struct individuos *popAux = pop;
    struct individuos *newpopAux = newpop;
    qsort( popAux, sizePop, sizeof (struct individuos ), comparaFitness );
    qsort( newpopAux, sizePop, sizeof (struct individuos ), comparaFitness );
    float lastIndividual = -1.0;
    int j = sizePop - 1;
    for ( int i = 0; i < sizePop; i++ )  {
        if( lastIndividual < pop[i].fitness )  {
            lastIndividual = pop[i].fitness;
            // cout << newpop[j].fitness;
            newpop[j].chrm = pop[i].chrm;
            newpop[j].fitness = pop[i].fitness;
            newpop[j].ruta = pop[i].ruta;
            newpop[j].parent[0] = i;
            newpop[j].parent[1] = i;
            newpop[j].evaluarHasta = pop[i].evaluarHasta;
            // cout << " -> " << pop[i].fitness  << endl;
            j--;
        }
        if(sizePop - 1 - j >= floor( sizePop * 0.3 ))
            break;
    }
    // exit(0);
}

/**
 *
 * Calcula Estadistica de la poblacion
 *
 **/
void AGenetic::statistics( struct individuos *pop )
{
    unsigned j;
    int k = -1;

    min = pop[0].fitness;
    max = pop[0].fitness;
    med = 0;

    varianza = 0;

    /* Loop para max, min */
    for ( j = 0; j < sizePop; j++ ) {
        if ( pop[j].fitness > max )
            max = pop[j].fitness; /* Nuevo máximo fitness */

        if ( pop[j].fitness < min )
            min = pop[j].fitness; /* nuevo mínimo fitness */

        med = med + pop[j].fitness; /* Suma de fitness */

        /* calculo de la varianza */
        varianza = varianza + ( pop[j].fitness * pop[j].fitness );

        /* Nuevo mejor individuo global */
        if ( pop[j].fitness < bestfit.fitness ) {
            bestfit.fitness = pop[j].fitness;
            k = j; /* Quiere decir que se actualizó mejor individuo */
        }
    }
    /* Se Obtiene promedios */
    med = ( double ) ( med / ( double ) sizePop );

    varianza = fabs( ( ( double ) ( varianza / ( double ) sizePop ) ) - ( med * med ) );
    /* Si actualizó mejor individuo se pasan los datos restantes del mejor */
    /* individuo. */
    if ( k >= 0 ) {
        bestfit.chrm = pop[k].chrm;
        bestfit.ruta = pop[k].ruta;
        bestfit.parent[0] = gen;
        bestfit.parent[1] = gen;
        bestfit.evaluarHasta = pop[k].evaluarHasta;
    }
}


/**
 *
 * Funcion Objetivo del AG esta llama a una funcion que convierte el
 * cromosoma en un arbol y evalua a todos los individuos de dicha poblacion
 *
 **/
void AGenetic::funcionObjetivo( struct individuos *indiv )
{
    // #pragma omp parallel for num_threads(hebras) schedule(dynamic, 1)
    for ( int i = 0; i < sizePop; i++ ) {
        // cout << "----------" << endl;
        // for(int j = 0; j < sizeChrm; j++)
        // 	cout << pop[i].chrm[j] << " ";
        // cout << endl;
        // int k = 0;// omp_get_thread_num();
        // cout << pop[i].chrm.size() << endl;

        int *puntero = &indiv[i].evaluarHasta;
        float costo = grafo->objective_function( indiv[i].chrm, suppliers, depot, P, associations, puntero );
        indiv[i].ruta = floor( costo * 100.00 + 0.5 ) / 100.00; //el costo se redondea a dos decimales

        // cout << endl << i << " -- " << valor << endl;
        if ( indiv[i].evaluarHasta == 0 ) {
            cout << "me caí" << endl;
            exit( 0 );
        }
        indiv[i].fitness = ( (floor( costo * 100.00 + 0.5 ) / 100.00) - optimo) / optimo;
    }

}

/**
 *
 * Función que imprime el reporte de algoritmo Genético
 *
 **/
void AGenetic::genera_reporte( )
{
    string genRep( ruta_resultados + nom_instance + "_" +
                   static_cast < ostringstream* > ( &( ostringstream( ) << ejecucion ) )->str( ) + "_" + "reporte.txt" );
    string name[] =  {
        "RLKH", "NNH ", "NNC ",
        "PMX  ", "OX1  ", "OX2  ", "MOX  ","POS  " , "CX   " , "DPX  ", "AP   " , "MPX  ", "HX   " , "IO   " , "MIO  ", "VR   " , "adhoc", "ER   "  ,
        "GSTX ",
        "EM   " , "SIM  ", "SM   " , "DM   " , "IVM  ", "ISM  ", "DBM  ", "LS1  ", "LS2  ", "LS3  ", "LS4  ", "LKH  ", "2opt ", "SHMO ", "DBM2 ",
        "3opt "
    };
    ofstream reporte( genRep.c_str( ) );
    reporte << fixed

            << "Instancia                           : " << nom_instance << endl
            << "Óptimo                              : " << optimo << endl
            << "Número de Facilities                : " << nSuppliers << endl
            << "Número de customers                 : " << customers.size() << endl
            << "Número de P                         : " << P << endl
            << "Número de Vértices                  : " << optimo << endl
            << "Datos del Algoritmo Genético"
            << endl
            << "Semilla Random                      : " << seed << endl
            << "Porcentaje Crossover                : " << pcross << endl
            << "Porcentaje Mutación                 : " << pmut << endl
            << "Tamaño de la Población              : " << sizePop << endl
            << "Número de Generaciones              : " << maxgen << endl
            << "Operadores Utilizados               : " << endl
            << "Sel "
            << endl;
    char *s = new char[1000];
    strcpy(s, "");
    for(int i = 0; i < 3; i++)
        sprintf(s, "%s%s  %.2lf\n", s, name[i].c_str(), porcentage0[i]);
    sprintf(s, "%sCross 1er   2da       Mut   1er   2da\n", s);

    for(int i = 3; i < 19; i++)
        sprintf(s, "%s%s %.2lf  %.2lf      %s %.2lf  %.2lf\n",
                s, name[i].c_str(), porcentage11[i-3], porcentage12[i-3], name[i+16].c_str(), porcentage21[i-3], porcentage22[i-3]);
    reporte << s
            << "Número de cruzamientos realizados   : " << ncross << endl
            << "Número de mutaciones  realizadas    : " << nmutation << endl
            << "Número de Threads                   : " << hebras << endl
            << "Tiempo (Segundos)                   : " << tiempo << endl << endl
            << "Datos del Mejor Individuo             " << endl
            << "Fitness                             : " << bestfit.fitness << endl
            << "Proviene de la Generación           : " << bestfit.parent[0] << endl
            << "Datos de la Solución                  " << endl
            << "Costo Ruta                          : " << bestfit.ruta << endl
            << "Price of the P >= P                 : " << grafo->calcularPrice( bestfit.chrm, P, &associations ) << " >= " << P << endl
            << "Número de suppliers cubiertos       : " << bestfit.evaluarHasta + 1 << endl
            << "Ruta                                : " << " ";
    for(int j = 0; j <= bestfit.evaluarHasta; j++)
        reporte << bestfit.chrm[j] << " ";
    reporte << endl;
}

/**
 *
 * Función que imprime una salida valores de promedio, max, min y var de
 * cada generacion del fitness, altura y número de nodos
 *
 **/
void AGenetic::genera_resultados_generacion( )
{
    string genRes( ruta_resultados + nom_instance + "_" +
                   static_cast < ostringstream* > ( &( ostringstream( ) << ejecucion ) )->str( ) + "_" + "generacion.csv" );
    if ( gen == 0 ) {
        ofstream generacion( genRes.c_str( ) );
        generacion << "#\tPromedio\tMáximo\tMínimo\tVarianza\tTiempo" << endl
                << gen << "\t" << fixed << med << "\t" << max << "\t" << min << "\t" << varianza << "\t"
                << tiempoGen << endl;
        generacion.close( );

        // printf("Generación Promedio Mejor - Best: Costo Instalación Costo Perdida Tiempo \n");
        // printf("%d %f %f %f\n",
        // 	     gen, med, bestfit.fitness,  tiempoGen);
    } else {
        ofstream generacion( genRes.c_str( ), ios::app );
        generacion << gen << "\t" << fixed << med << "\t" << max << "\t" << min << "\t" << varianza << "\t"
                << tiempoGen << endl;
        generacion.close( );

        // printf("%d %f %f  %f\n",
        // 	     gen, med, bestfit.fitness, tiempoGen);
    }
}

void AGenetic::genera_resultados_layout( )
{
    string outName(ruta_resultados + "layout_" + nom_instance + "_"
                   + static_cast < ostringstream* > ( &( ostringstream( ) << ejecucion ) )->str( ) +".gml");
    grafo->graficar(outName, bestfit.chrm, suppliers, customers, depot, bestfit.evaluarHasta, associations);

    // graficar(ofstream of, vector<unsigned> suppliers, set < node* > container, node *depot, int P, map < int, set < int > > associations)
}

/**
 *
 * Función de Población inicial
 *
 **/
void AGenetic::initPop( )
{
    for ( int i = 0; i < sizePop; i++ ) {
        // int rand = rnd(0, 5);
        switch ( distSel( generator ) )
            {
            case 0:
                {
                    initpop->Random( pop[i].chrm );
                    // Actualizamos el n, ya que se cambio con los cruzamientos
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int n = conjuntoConCustomersAll(&coversOriginal, &associations, pop[i].chrm , P);
                    n++; // agregar el depot, se agrega de forma ficticia
                    mut->LKH( pop[i].chrm, tsp, n);
                    break;
                }
            case 1:
                {
                    initpop->NNH( pop[i].chrm, tsp );
                    break;
                }
            case 2:
                {
                    initpop->NNC( pop[i].chrm, tsp );
                    break;
                }
            }
    }
    funcionObjetivo( pop );
    statistics( pop );
}

/**
 *
 * Funcion de selección
 *
 **/
unsigned AGenetic::select( )
{
    int k = 7;
    //int k = rnd(2, 10);
    unsigned selected;
    selected = sel->TorneoK( pop, k );
    //selected = sel->Ruleta(pop);
    return selected;
}

void AGenetic::reparar(vector <unsigned> &child1, vector <unsigned> &childNew1)
{
    // Eliminar depot
    childNew1.erase( childNew1.begin( ) );
    // Reparar
    for(int i = 0; i < childNew1.size(); i++)
        {
            int posDelRepetido = find( child1.begin( )+childNew1.size(), child1.end( ), childNew1[i] ) - child1.begin( );
            // cout << posDelRepetido << endl;
            if(posDelRepetido < child1.size())
                {
                    // cout << "hay uno, es " << childNew1[i];
                    // cout << endl;
                    for(int j = 0; j < childNew1.size(); j++)
                        {
                            int posDelReemplazar = find( childNew1.begin( ), childNew1.end( ), child1[j] ) - childNew1.begin( );
                            if(posDelReemplazar == childNew1.size())
                                {
                                    // cout << "es " << posDelReemplazar<< " falta " << child1[j] << endl;
                                    child1[posDelRepetido] = child1[j];
                                }
                        }
                }
        }
    //reemplazar
    for(int i = 0; i < childNew1.size(); i++)
        child1[i] = childNew1[i];
}
void AGenetic::reparar2(vector <unsigned> &child1, vector <unsigned> &childNew1)
{
    // Reparar
    for(int i = 0, j = 0; i < childNew1.size(); i++)
        {
            int posDelRepetido = find( child1.begin( )+childNew1.size(), child1.end( ), childNew1[i] ) - child1.begin( );
            // cout << posDelRepetido << endl;
            if(posDelRepetido < child1.size())
                {
                    // cout << "hay uno, es " << childNew1[i];
                    // cout << endl;
                    for(; j < childNew1.size(); j++)
                        {
                            int posDelReemplazar = find( childNew1.begin( ), childNew1.end( ), child1[j] ) - childNew1.begin( );
                            if(posDelReemplazar == childNew1.size())
                                {
                                    // cout << "es " << posDelReemplazar<< " falta " << child1[j] << endl;
                                    child1[posDelRepetido] = child1[j];
                                    j++;
                                    break;
                                }
                        }
                }
        }
    //reemplazar
    for(int i = 0; i < childNew1.size(); i++)
        child1[i] = childNew1[i];
}

/**
 *
 * Funcion de Cruzamiento
 *
 **/
void AGenetic::crossover( vector <unsigned> &child1, vector <unsigned> &child2, int n, int m )
{
    if ( flip( pcross ) ) {
        switch ( distCross( generator ) )
            {
            case 0:
                ncross += cross->PMX( child1, child2 ) + 1;
                break;
            case 1:
                ncross += cross->OX1( child1, child2 ) + 1;
                break;
            case 2:
                ncross += cross->OX2( child1, child2 ) + 1;
                break;
            case 3:
                ncross += cross->MOX( child1, child2 ) + 1;
                break;
            case 4:
                ncross += cross->POS( child1, child2 ) + 1;
                break;
            case 5:
                ncross += cross->CX( child1, child2 ) + 1;
                break;
            case 6:
                {
                    // Nuevos children del tamaño necesario hasta cubrir P
                    vector <unsigned> childNew1(child1.begin(), child1.begin()+n);
                    // Copia de Child1, para mantener el original y hacer el segundo DPX
                    vector <unsigned> childNew11(child1.begin(), child1.begin()+n);
                    vector <unsigned> childNew2(child2.begin(), child2.begin()+m);
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it1 = childNew1.begin( );
                    vector<unsigned>::iterator it11 = childNew11.begin( );
                    vector<unsigned>::iterator it2 = childNew2.begin( );
                    //Insertar depot
                    it1 = childNew1.insert( it1, 0 );
                    it11 = childNew11.insert( it11, 0 );
                    it2 = childNew2.insert( it2, 0 );

                    ncross += cross->DPX( childNew1, childNew2, tsp ) + cross->DPX( childNew2, childNew11, tsp );
                    //Reparar child1 y child2
                    reparar(child1, childNew1);
                    reparar(child2, childNew2);
                    break;
                }
            case 7:
                ncross += cross->AP( child1, child2 ) + 1;
                break;
            case 8:
                ncross += cross->MPX( child1, child2 ) + 1;
                break;
            case 9:
                ncross += cross->HX( child1, child2, tsp ) + cross->HX( child2, child1, tsp );
                break;
            case 10:
                {
                    // Nuevos children del tamaño necesario hasta cubrir P
                    vector <unsigned> childNew1(child1.begin(), child1.begin()+n);
                    // Copia de Child1, para mantener el original y hacer el segundo DPX
                    // vector <unsigned> childNew11(child1.begin(), child1.begin()+n);
                    vector <unsigned> childNew2(child2.begin(), child2.begin()+m);
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it1 = childNew1.begin( );
                    // vector<unsigned>::iterator it11 = childNew11.begin( );
                    vector<unsigned>::iterator it2 = childNew2.begin( );
                    //Insertar depot
                    it1 = childNew1.insert( it1, 0 );
                    it2 = childNew2.insert( it2, 0 );

                    ncross += cross->IO(childNew1, pop, tsp) + cross->IO(childNew2, pop, tsp);
                    //Reparar child1 y child2
                    reparar(child1, childNew1);
                    reparar(child2, childNew2);
                    break;
                }
            case 11:
                {
                    // Nuevos children del tamaño necesario hasta cubrir P
                    vector <unsigned> childNew1(child1.begin(), child1.begin()+n);
                    // Copia de Child1, para mantener el original y hacer el segundo DPX
                    // vector <unsigned> childNew11(child1.begin(), child1.begin()+n);
                    vector <unsigned> childNew2(child2.begin(), child2.begin()+m);
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it1 = childNew1.begin( );
                    // vector<unsigned>::iterator it11 = childNew11.begin( );
                    vector<unsigned>::iterator it2 = childNew2.begin( );
                    //Insertar depot
                    it1 = childNew1.insert( it1, 0 );
                    it2 = childNew2.insert( it2, 0 );

                    ncross += cross->MIO(childNew1, pop, tsp, maxgen, gen) + cross->MIO(childNew2, pop, tsp, maxgen, gen);
                    //Reparar child1 y child2
                    reparar(child1, childNew1);
                    reparar(child2, childNew2);
                    break;
                }
            case 12:
                ncross += cross->VR( child1, child2, pop ) + cross->VR( child2, child1, pop );
                break;
            case 13:
                {
                    // Nuevos children del tamaño necesario hasta cubrir P
                    vector <unsigned> childNew1(child1.begin(), child1.begin()+n);
                    // Copia de Child1, para mantener el original y hacer el segundo DPX
                    vector <unsigned> childNew11(child1.begin(), child1.begin()+n);
                    vector <unsigned> childNew2(child2.begin(), child2.begin()+m);
                    // No usa depot este operador
                    ncross += cross->adhoc( childNew1, childNew2, tsp ) + cross->adhoc( childNew2, childNew11, tsp );
                    //Reparar child1 y child2
                    reparar2(child1, childNew1);
                    reparar2(child2, childNew2);
                    break;
                }
            case 14:
                {
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it1 = child1.begin( );
                    // vector<unsigned>::iterator it11 = childNew11.begin( );
                    vector<unsigned>::iterator it2 = child2.begin( );
                    //Insertar depot
                    it1 = child1.insert( it1, 0 );
                    it2 = child2.insert( it2, 0 );

                    ncross += cross->ER( child1, child2 ) + 1;

                    // Eliminar depot
                    child1.erase( child1.begin( ) );
                    child2.erase( child2.begin( ) );
                }
                break;

            case 15:
                ncross += cross->GSTX( child1, child2 ) + cross->GSTX( child2, child1 );
                break;
            }
    }
}

/**
 *
 * Funcion que llama a los operadores de Mutación
 *
 **/

/**
 * Función que realiza lala
 *
 * @param child1: cromosoma
 * @param n: cantidad de genes para cubrir P
 *
 * @return tipoDato
 *
 */

void AGenetic::mutation( vector <unsigned> &child1, int n )
{
    if ( flip( pmut ) ) {
        switch ( distMut( generator ) )
            {
            case 0:
                nmutation += mut->EM( child1 );
                break;
            case 1:
                nmutation += mut->SIM( child1 );
                break;
            case 2:
                nmutation += mut->SM( child1 );
                break;
            case 3:
                nmutation += mut->DM( child1 );
                break;
            case 4:
                nmutation += mut->IVM( child1 );
                break;
            case 5:
                {
                    // Actualizamos el n, ya que se cambio con los cruzamientos
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, child1, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;
                    nmutation += mut->ISM( child1, n );
                    break;
                }
            case 6:
                nmutation += mut->DBM( child1 );
                break;
            case 7:
                nmutation += mut->LS1( child1, tsp, n);
                break;
            case 8:
                nmutation += mut->LS2( child1, tsp, n);
                break;
            case 9:
                nmutation += mut->LS3( child1, tsp, n);
                break;
            case 10:
                nmutation += mut->LS4( child1, tsp, n);
                break;
            case 11:
                {
                    // Actualizamos el n, ya que se cambio con los cruzamientos
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, child1, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;
                    n++; // agregar el depot, se agrega de forma ficticia
                    nmutation += mut->LKH( child1, tsp, n);
                    break;
                }
            case 12:
                {
                    // Actualizamos el n, ya que se cambio con los cruzamientos
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, child1, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;
                    // Nuevo children del tamaño necesario hasta cubrir P
                    vector <unsigned> child2(child1.begin(), child1.begin()+n);
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it = child2.begin( );
                    it = child2.insert( it, 0 );
                    nmutation += mut->twoOPT( child2, tsp );

                    child2.erase( child2.begin( ) );
                    // Colocar nuevo child en child1
                    for(int i = 0; i < n; i++)
                        child1[i] = child2[i];
                    break;
                }
            case 13:
                {
                    // Actualizamos el n, ya que se cambio con los cruzamientos
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, child1, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;
                    // Nuevo children del tamaño necesario hasta cubrir P
                    vector <unsigned> child2(child1.begin(), child1.begin()+n);
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it = child2.begin( );
                    it = child2.insert( it, 0 );
                    nmutation += mut->SHMO( child2, tsp, n+1);

                    child2.erase( child2.begin( ) );
                    // Colocar nuevo child en child1
                    for(int i = 0; i < n; i++)
                        child1[i] = child2[i];
                    break;
                }
            case 14:
                {
                    // Actualizamos el n, ya que se cambio con los cruzamientos
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, child1, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;

                    // Nuevo child del tamaño necesario hasta cubrir P
                    vector <unsigned> child2(child1.begin(), child1.begin()+n);
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it = child2.begin( );
                    it = child2.insert( it, 0 );
                    nmutation += mut->DBM2( child2, tsp, n+1);

                    child2.erase( child2.begin( ) );
                    // Colocar nuevo child en child1
                    for(int i = 0; i < n; i++)
                        child1[i] = child2[i];
                    break;
                }
            case 15:
                {
                    // Actualizamos el n, ya que se cambio con los cruzamientos
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, child1, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;
                    // Nuevo child del tamaño necesario hasta cubrir P
                    vector <unsigned> child2(child1.begin(), child1.begin()+n);
                    // Agregar depot al inicio del cromosoma
                    vector<unsigned>::iterator it = child2.begin( );
                    it = child2.insert( it, 0 );
                    nmutation += mut->threeOPT( child2, tsp, n+1);

                    child2.erase( child2.begin( ) );
                    // Colocar nuevo child en child1
                    for(int i = 0; i < n; i++)
                        child1[i] = child2[i];
                    break;
                }
            }

            // mut->LS4( child1, tsp, n);
    }

}

void AGenetic::barra_progreso( float time )
{
    float valor;
    unsigned pos;
    int i;
    const char s[] = "-\\|/-\\|/";
    i = gen + 1;
    int n = strlen( s );
    pos = gen % n;

    if ( gen == 0 )
        printf( "Generación\tPromedio\tMáximo\t\tMínimo\t\t\tTiempo\t\tProgreso\n" );

    valor = ( float ) ( 100 * i * sizePop ) / ( float ) ( ( maxgen + 1 ) * sizePop );
    fprintf( stdout, "%d\t\t%.4f\t\t%.4f\t\t%.4f (%d)\t\t%.2f\t\t%.2f %% %c\r",
             // gen, med*100, max*100, min*100, bestfit.parent[0],time, valor, s[pos]);
             gen, med, max, min, bestfit.parent[0], time, valor, s[pos] );
    fflush( stdout );
}

/**
 *
 * Función que realiza el ciclo evolutivo del algoritmo genetico
 *
 **/
vector <unsigned> AGenetic::evolution( unsigned nGen )
{
    unsigned mat1, mat2;
    double ti, tf, tiG, tfG;
    // Instante inicial
    ti = omp_get_wtime( );
    maxgen = nGen;
    gen = 0;

    tiG = omp_get_wtime( );
    initPop( ); //Poblation initial
    tfG = omp_get_wtime( );
    tiempoGen = tfG - tiG;

    genera_resultados_generacion( );
    barra_progreso( omp_get_wtime( ) - ti );
    tiG = omp_get_wtime( );


    for ( gen = 1; ( unsigned ) gen <= maxgen; gen++ ) {

        if(gen == maxgen / 2) {
            distCross = dist2daCross;
            distMut = dist2daMut;
            // pcross = 0.3;
        }

        for ( unsigned i = 0; i < sizePop - 1; i = i + 2 )
            // for(unsigned i = 0; i < sizePop-1; i++)
        {
            // //SOLO PARA VALIDAR
            // validarOp();

            // Select
            mat1 = select( );
            mat2 = select( );

            // reproduccion
            newpop[i].chrm = pop[mat1].chrm;
            newpop[i + 1].chrm = pop[mat2].chrm;
            newpop[i].evaluarHasta = pop[i].evaluarHasta;
            newpop[i + 1].evaluarHasta = pop[i + 1].evaluarHasta;
            // Crossover
            crossover( newpop[i].chrm, newpop[i + 1].chrm, newpop[i].evaluarHasta, newpop[i + 1].evaluarHasta );

            // Mutation

            mutation( newpop[i].chrm, newpop[i].evaluarHasta );
            mutation( newpop[i + 1].chrm, newpop[i + 1].evaluarHasta );

            newpop[i].parent[0] = newpop[i + 1].parent[0] = mat1;
            newpop[i].parent[1] = newpop[i + 1].parent[1] = mat2;
        }


        funcionObjetivo( newpop );

        elitism( );

        statistics( newpop );
        barra_progreso( omp_get_wtime( ) - ti );
        if(bestfit.fitness == 0.0)
        	break;
        tfG = omp_get_wtime( );
        tiempoGen = tfG - tiG;

        genera_resultados_generacion( );
        // cout<<"termino generacion "<< gen<<endl;

        //Inicia tiempo para la generación que sigue
        tiG = omp_get_wtime( );

        // Avanza de Generación
        tempold = pop;
        pop = newpop;
        newpop = tempold;


    }
    // Instante final
    tf = omp_get_wtime( );
    tiempo = tf - ti;

    genera_reporte( );
    genera_resultados_layout( );
    cout << endl;
    return bestfit.chrm;
}



double AGenetic::evaluar(int who)
{
    int *puntero = &pop[who].evaluarHasta;
    float costo = grafo->objective_function( pop[who].chrm, suppliers, depot, P, associations, puntero );

    // cout << "costo es " << costo << endl;
    pop[who].ruta = floor( costo * 100.00 + 0.5 ) / 100.00; //el costo se redondea a dos decimales

    // cout << endl << i << " -- " << valor << endl;
    if ( pop[who].evaluarHasta == 0 ) {
        cout << "me caí" << endl;
        exit( 0 );
    }
    return (pop[who].fitness = (( (floor( costo * 100.00 + 0.5 ) / 100.00) - optimo) / optimo) * 100) ;
}


void AGenetic::removal(int who)
{
    map <double, int> R;
    map < int, set < int > >::iterator iter;
    set < int > vec;
    int iMasUno = 0, iMenosUno = 0;
    double costSaving = 0;
    for(int i = 0; i < pop[who].chrm.size(); i++)
        {
            iMasUno = ( i == pop[who].chrm.size( ) - 1 ) ? 0 : i + 1;
            iMenosUno = ( i == 0 ) ? pop[who].chrm.size( ) - 1 : i - 1;
            iter = associations.find( pop[who].chrm[i] );
            costSaving =
                tsp->distancia2Ciudades(pop[who].chrm[iMenosUno], pop[who].chrm[i])
                -  tsp->distancia2Ciudades(pop[who].chrm[i], pop[who].chrm[iMasUno])
                + tsp->distancia2Ciudades(pop[who].chrm[iMenosUno], pop[who].chrm[iMasUno]);
            // cout << "Node id = " << iter->first << " " <<  (iter->second).size() << " = "<< i<< endl;
            // cout << costSaving << endl;
            //R.push_back();
            // es mejor guardar su posición en el arreglo
            R.insert(std::pair<double, int>( costSaving/ ((iter->second).size() + 1), i));
            // cout << R.back() << endl;
            // cout << "Associated to: ";
            // //show_set( iter->second );
            // set < int > vec = iter->second;
            // // for ( set <int>::iterator iter = vec.begin( );
            // //       iter != vec.end( );
            // //       iter++ ) {
            // //     cout << *iter << " ";
            // // }

            // cout << vec.size() << endl;

        }

    // for (auto& it: R)
    //     std::cout << it.first << " => " << it.second << '\n';

    int n = 0;
    set < int > coversAux; // set con los todos los customers cubiertos
    int newP = conjuntoConCustomersAll(&coversAux, &associations, pop[who].chrm, P); // sin la ciudad que se desea reemplazar
    if(newP != -1) // Se actualiza
        n = newP;

    map <double, int>::iterator it = R.begin();

    //cout << "laallalla " << it->second << endl;
    set < int > coversOriginal;//coversOriginal.clear();
    int reemplazar = rnd(0, n*0.1); // Se escoge una ciudad al azar

    //reemplazar = 0;
    std::advance(it, reemplazar);
    int ciudad = pop[who].chrm[it->second]; // ciudad candidata a cambiar
    //cout << "choose " << ciudad << " "<< it->second << endl;
    // Si se cumple mejor quitar la ciudad
    //if(coversOriginal.size() >= P)
    {
        pop[who].chrm.erase( pop[who].chrm.begin( ) + reemplazar );
        pop[who].chrm.push_back(ciudad); // La agregamos al final
        //return true;
    }
    conjuntoConCustomers(&coversOriginal, &associations, pop[who].chrm, ciudad, n); // sin la ciudad que se desea reemplazar
    if(coversOriginal.size() < P)
        pop[who].evaluarHasta = n+1;

}

vector <unsigned> AGenetic::ILS( unsigned nGen )
{
    double ti, tf;
    cout << "ILS" << endl;
    int n = 0;
    // Instante inicial
    ti = omp_get_wtime( );
    maxgen = nGen;
    //initpop->Random( pop[0].chrm );
    //initpop->NNC(pop[0].chrm, tsp );
    //initpop->NNH( pop[0].chrm, tsp );
    //tsp->vecinoCercanoAlDepot (pop[0].chrm, associations, 1 );
    tsp->vecinoMasCercano(0, pop[0].chrm);
    pop[0].fitness = evaluar(0);
    cout << "Initial Solution1 "<< pop[0].fitness << endl;
    mut->LS1(pop[0].chrm, tsp, n);
    pop[0].fitness = evaluar(0);
    cout << "Initial Solution2 "<< pop[0].fitness << endl;
    mut->LS2(pop[0].chrm, tsp, n);
    pop[0].fitness = evaluar(0);

    cout << "Initial Solution3 "<< pop[0].fitness << endl;
    mut->LS3(pop[0].chrm, tsp, n);
    pop[0].fitness = evaluar(0);
    cout << "Initial Solution4 "<< pop[0].fitness << endl;
    set < int > coversOriginal; // set con los todos los customers cubiertos
    n = conjuntoConCustomersAll(&coversOriginal, &associations, pop[0].chrm , P);
    // cout << "nuevo n "<< n << endl;
    // exit(0);
    // mut->LS4(pop[0].chrm, tsp, n);
    // set < int > coversOriginal; // set con los todos los customers cubiertos
    // n = conjuntoConCustomersAll(&coversOriginal, &associations, pop[0].chrm , P);
    // n++; // agregar el depot, se agrega de forma ficticia
    //mut->LKH( pop[0].chrm, tsp, n);
    pop[0].fitness = evaluar(0);
    cout << "Initial SolutionF "<< pop[0].fitness << endl;

    int history = 0;
    gen = 0;
    // solution 1
    pop[0].parent[0] = gen;
    // solution 2
    pop[1].chrm = pop[0].chrm;
    pop[1].fitness = pop[0].fitness;
    pop[1].ruta = pop[0].ruta;
    pop[1].parent[0] = pop[0].parent[0];
    pop[1].evaluarHasta = pop[0].evaluarHasta;
    //best
    // bestCost = pop[0].fitness;
    // bestRoute = pop[0].chrm;
    double bestCost = pop[0].fitness;
    vector <unsigned> bestRoute(pop[0].chrm.begin(), pop[0].chrm.end());
    bestfit.ruta = pop[0].ruta;
    bestfit.parent[0] = gen;
    bestfit.evaluarHasta = pop[0].evaluarHasta;
    nGen = 5000;


    do
        {
            if(flip(0.7))
                //removal(1);
                mut->DBM(pop[1].chrm);
            //mut->EM(pop[1].chrm);
            else
                //mut->DBM(pop[1].chrm);
                //removal(1);
                //mut->SM(pop[1].chrm);
                //mut->EM2(pop[1].chrm, tsp, pop[1].evaluarHasta);
                mut->LS1(pop[1].chrm, tsp, pop[1].evaluarHasta);

            if(flip(0.7))
                //mut->LS1(pop[0].chrm, tsp, pop[0].evaluarHasta);
                {
                    removal(0);
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, pop[0].chrm, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;
                    //n++; // agregar el depot, se agrega de forma ficticia
                    //mut->LKH( pop[0].chrm, tsp, n + 1);
                    nmutation += mut->twoOPT( pop[0].chrm, tsp );

                }
            else
                mut->EM2(pop[1].chrm, tsp, pop[1].evaluarHasta);


            // mut->LS2(pop[0].chrm, tsp, n);
            //compute fitness
            pop[0].fitness = evaluar(0);
            pop[1].fitness = evaluar(1);

            if(history > 10)
                {
                    pop[0].fitness = pop[1].fitness;
                    pop[0].chrm = pop[1].chrm;
                    pop[0].ruta = pop[1].ruta;
                    pop[0].parent[0] = pop[1].parent[0];
                    pop[0].evaluarHasta = pop[1].evaluarHasta;


                    //mut->EM2(pop[0].chrm, tsp, pop[0].evaluarHasta);
                    set < int > coversOriginal; // set con los todos los customers cubiertos
                    int newP = conjuntoConCustomersAll(&coversOriginal, &associations, pop[0].chrm, P);
                    if(newP != -1) // Se actualiza el n
                        n = newP;
                    //n++; // agregar el depot, se agrega de forma ficticia
                    mut->LKH( pop[0].chrm, tsp, n + 1);

                    mut->LS3(pop[0].chrm, tsp, n);


                    pop[0].fitness = evaluar(0);
                    history = 0;

                }
            else
                {
                    if(pop[1].fitness < pop[0].fitness)
                        {
                            pop[0].fitness = pop[1].fitness;
                            pop[0].chrm = pop[1].chrm;
                            pop[0].ruta = pop[1].ruta;
                            pop[0].evaluarHasta = pop[1].evaluarHasta;
                            pop[0].parent[0] = pop[1].parent[0];
                            // cout << "funcioné "
                        }
                    pop[1].fitness = bestCost;
                    pop[1].chrm = bestRoute;
                    pop[1].ruta = bestfit.ruta;
                    pop[1].evaluarHasta = bestfit.evaluarHasta;
                    pop[1].parent[0] = bestfit.parent[0];
                }

            if(bestCost > pop[0].fitness)
                {
                    mut->LS3(pop[0].chrm, tsp, n);
                    bestCost = pop[0].fitness;
                    bestRoute = pop[0].chrm;
                    bestfit.ruta = pop[0].ruta;
                    bestfit.parent[0] = gen;
                    bestfit.evaluarHasta = pop[0].evaluarHasta;
                    history = 0;
                    //cout << "bestCost " << pop[0].ruta << " " << bestCost << "%, iter " << gen <<endl;
                    printf("bestCost %f (%.2f %%) iter %d\n", pop[0].ruta, bestCost, gen);

                }
            else
                {
                    history++;
                }
            gen++;
        }
    while(gen < nGen);

    // Instante final
    tf = omp_get_wtime( );
    tiempo = tf - ti;


    //statistics(pop);
    printf("Final %f (%.2f %%) iter %d, %.2f secs\n", bestfit.ruta, bestCost, bestfit.parent[0], tiempo);
    //cout << "Final " << bestfit.ruta << " " << bestCost << "%, iter " << bestfit.parent[0] << " time: "  << tiempo<<endl;
    bestfit.fitness = bestCost / 100;
    bestfit.chrm = bestRoute;
    //bestfit.ruta = bestRoute;
    // bestfit.chrm = pop[0].chrm;
    // bestfit.fitness = pop[0].fitness
    return bestfit.chrm;
}
