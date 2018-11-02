/*
 * selection.hpp
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

#ifndef _SELECTION_H
#define	_SELECTION_H

#include <cstdio>
#include <algorithm>
#include "util.hpp"
using namespace std;

class Selection : public Util {
public:
    unsigned sizePop; /* tamaño de la poblacion */
    Selection( unsigned nPop, unsigned seed );
    virtual ~Selection( );
    unsigned TorneoK( individuos *pop, int k );
    unsigned Ruleta( individuos *pop );
private:
    /* data */
};

#endif	/* _SELECTION_H */
