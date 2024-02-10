/**************************************************************************
 *
 * $Id: integrate.h,v 1.19 2011/03/26 07:05:54 patrick Exp $
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
 * Quasi Neutrality Check and Correction
 *
 * Code uses the Blitz C++ library (http://oonumerics.org/blitz)
 *
 */

#ifndef INTEGRATE_H
#define INTEGRATE_H

#include <iostream>
#include <blitz/array.h>

#define INTEGRATE_DECL(type,dim,statvar)                                   \
type integrate(blitz::Array<type,dim> &F, blitz::TinyVector<type,dim> &dr, \
               type &dI=statvar);

static float int_ferror;
INTEGRATE_DECL(float,1,int_ferror)
INTEGRATE_DECL(float,2,int_ferror)
INTEGRATE_DECL(float,3,int_ferror)

static double int_derror;
INTEGRATE_DECL(double,1,int_derror)
INTEGRATE_DECL(double,2,int_derror)
INTEGRATE_DECL(double,3,int_derror)

template<typename T_numtype>
void d01gaf(blitz::Array<T_numtype,1> &y, T_numtype &dx, T_numtype &I,
            T_numtype &dI)
{
  using blitz::pow2;
  using blitz::pow5;
  using std::cerr;
  using std::endl;

  const int n = y.rows();

  if (n < 4) {
    cerr << "d01gaf: array must contain at least 4 points." << endl;
    throw(EXIT_FAILURE);
  }

  // Integrate over initial interval
  T_numtype d3 = (y(1)-y(0))/dx;
  T_numtype d1 = (y(2)-y(1))/dx;
  T_numtype d2 = (d1-d3)/(2.0*dx);
  T_numtype r1 = (y(3)-y(2))/dx;
  T_numtype r2 = (r1-d1)/(2.0*dx);
  T_numtype r3 = (r2-d2)/(3.0*dx);

  I = dx*(y(0)+dx*(d3*0.5-dx*(d2/6.0-dx*r3/4.0)));
  T_numtype s = -19.0/30.0*pow5(dx);
  T_numtype r4 = 0.0;

  // Integrate over central portion of range
  dI = 0.0;
  for (int i=2; i<n-1; ++i) {
    I += dx*((y(i)+y(i-1))*0.5-pow2(dx)*(d2+r2)/12.0);
    T_numtype c = 11.0/60.0*pow5(dx);
    dI += (c+s)*r4;
    if ( i != 2 )
      s = c;
    else
      s += 2.0*c;
    if ( i != n-2 ) {
      d1 = r1;
      d2 = r2;
      d3 = r3;
      r1 = (y(i+2)-y(i+1))/dx;
      r2 = (r1-d1)/(2.0*dx);
      r3 = (r2-d2)/(3.0*dx);
      r4 = (r3-d3)/(4.0*dx);
    } else
      break;
  }
  I += dx*(y(n-1)-dx*(r1*0.5+dx*(r2/6.0+dx*r3/4.0)));
  dI -= 19.0/30.0*pow5(dx)*r4+s*r4;
  I += dI;
}

template<typename T_numtype>
T_numtype integrate_t(blitz::Array<T_numtype,1> &y,
                      blitz::TinyVector<T_numtype,1> &dr, T_numtype &dI)
{
  T_numtype I;
  d01gaf(y, dr(0), I, dI);
  return I;
}

template<typename T_numtype>
T_numtype integrate_t(blitz::Array<T_numtype,2> &F,
                      blitz::TinyVector<T_numtype,2> &dr, T_numtype &dI)
{
  const int ny = F.cols();
  blitz::Array<T_numtype,1> Iy(ny), dIy(ny);
  blitz::TinyVector<T_numtype,1> dx(dr(0));
  for (int j=0; j<ny; ++j) {
    blitz::Array<T_numtype,1> Fx ( F(blitz::Range::all(), j) );
    Iy(j) = integrate_t<T_numtype>(Fx, dx, dIy(j));
  }
  blitz::TinyVector<T_numtype,1> dy(dr(1));
  T_numtype I = integrate_t<T_numtype>(Iy, dy, dI);
  dI += blitz::sum(dIy);
  return I;
}

template<typename T_numtype>
T_numtype integrate_t(blitz::Array<T_numtype,3> &F,
                      blitz::TinyVector<T_numtype,3> &dr, T_numtype &dI)
{
  const int nz = F.depth();
  blitz::Array<T_numtype,1> Iz(nz), dIz(nz);
  blitz::TinyVector<T_numtype,2> dxdy(dr(0), dr(1));
  for (int k=0; k<nz; ++k) {
    blitz::Array<T_numtype,2> Fxy ( F(blitz::Range::all(),
                                      blitz::Range::all(), k) );
    Iz(k) = integrate_t<T_numtype>(Fxy, dxdy, dIz(k));
  }
  blitz::TinyVector<T_numtype,1> dz(dr(2));
  T_numtype I = integrate_t<T_numtype>(Iz, dz, dI);
  dI += blitz::sum(dIz);
  return I;
}


#endif
