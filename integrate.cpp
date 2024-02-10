/**************************************************************************
 *
 * $Id: integrate.cpp,v 1.22 2011/03/26 07:05:54 patrick Exp $
 *
 * Copyright (c) 2000-2011 Patrick Guio <patrick.guio@gmail.com>
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2.  of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/

/*
 * Numerical integration calculation
 *
 * Code uses the Blitz C++ library (http://oonumerics.org/blitz)
 *
 */

#include <integrate.h>

#define INTEGRATE_IMPL(type,dim)                                           \
type integrate(blitz::Array<type,dim> &F,                                  \
               blitz::TinyVector<type,dim> &dr, type &dI)                  \
{                                                                          \
	return integrate_t<type>(F, dr, dI);                                     \
}                                                                          \
 

INTEGRATE_IMPL(float,1)
INTEGRATE_IMPL(float,2)
INTEGRATE_IMPL(float,3)

INTEGRATE_IMPL(double,1)
INTEGRATE_IMPL(double,2)
INTEGRATE_IMPL(double,3)

