/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/*
 * util.cpp
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

#include "util.hpp"

Util::Util( )
{
}

Util::~Util( )
{

}

/**
 *
 * Funcion que retorna valores random entre un rango
 *
 **/
int Util::rnd( unsigned low, unsigned high )
{
    //return low + rand() % ((high + 1) - low);
    return low + generator( ) % ( ( high + 1 ) - low );
}

/**
 *
 * Funcion que funciona como una moneda al aire, dependiendo de la
 * probabilidad que esta devuelve un 1 o un 0
 *
 **/
bool Util::flip( float prob )
{
    boost::uniform_real<> uni_dist( 0, 1 );
    boost::variate_generator<base_generator_type&, boost::uniform_real<> > uni( generator, uni_dist );
    double d = uni( );
    //double d = generate_canonical<double, numeric_limits<double>::digits>(generator);
    if ( d <= prob )
        // if(((double) rand() / (RAND_MAX)) <= prob)
        return 1;
    else
        return 0;
}
