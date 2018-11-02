/*
 * util.hpp
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

#ifndef _UTIL_H
#define	_UTIL_H

#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <cstdio>
#include <map>
#include <functional>
#include <boost/array.hpp>
#include <vector>
#include <set>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include "node.h"
using namespace std;

class Util {
public:

    struct individuos {
        vector <unsigned> chrm; /* cromosoma de enteros */
        int evaluarHasta;
        float fitness;
        float errorRelativo;
        float ruta;
        short unsigned parent[2]; /* padres del individuo */
    };

    // TSP **tsp;
    boost::minstd_rand generator;
    typedef boost::minstd_rand base_generator_type;

    Util( );
    virtual ~Util( );
    int rnd( unsigned low, unsigned high );
    bool flip( float prob );

private:
    /* data */
};

template<template <typename> class P = std::less >
struct compare_pair_second {

    template<class T1, class T2> bool operator()(const std::pair<T1, T2>& left,
            const std::pair<T1, T2>& right) {
        return P<T2>()(left.second, right.second);
    }
};
#endif	/* _UTIL_H */
