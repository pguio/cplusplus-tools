/**************************************************************************
 *
 * $Id: plplot-interface.h,v 1.2 2011/03/26 07:05:54 patrick Exp $
 *
 * Copyright (c) 2007-2011 Patrick Guio <patrick.guio@gmail.com>
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

#ifndef PLPLOT_INTERFACE_H
#define PLPLOT_INTERFACE_H

#include <plplot>

namespace plplot {

  class PLplotException : public std::runtime_error {
  public:
    PLplotException(const std::string msg) : std::runtime_error(msg) {}
  };


  class PLplot {
  public:

    typedef std::string string;
    typedef std::vector<double> Vector;

    PLplot(const char[]="xwin");
    ~PLplot();

    void setLineStyle(const string style);
    void setXLabel(const string label);
    void setYLabel(const string label);
    void setZLabel(const string label);
    void setTitle(const string title);

#ifdef BZ_ARRAY_H
    template <typename Num_type>
    void plot(blitz::Array<Num_type,1> &x, const string legend="");
    template <typename Num_type>
    void plot(blitz::Array<Num_type,1> &x, blitz::Array<Num_type,1> &y, const string legend="");
#endif

    void pause(unsigned seconds=0);

    void reset();

  private:
    bool plplot;

    plstream *pls;
    PLFLT *xp, *yp;
    PLFLT xpmin, xpmax, ypmin, ypmax;
  };


#ifdef BZ_ARRAY_H
  template <typename Num_type>
  void PLplot::plot(blitz::Array<Num_type,1> &d, const string legend)
  {
    if (d.size() == 0 )
      throw PLplotException("vectors of length zero");

    for (unsigned i = 0; i < d.size(); ++i) {
      xp[i] = i;
      yp[i] = d(i);
    }

    xpmin = 0;
    xpmax = d.size()-1;
    ypmin = min(d);
    ypmax = max(d);

    pls->col0(15);
    pls->env( xpmin, xpmax, ypmin, ypmax, 0, 0 );
    pls->col0(14);
    pls->line(N, xp, yp);
  }

  template <typename Num_type>
  void PLplot::plot(blitz::Array<Num_type,1> &x, blitz::Array<Num_type,1> &y, const string legend)
  {
    if (x.size() == 0 || y.size() == 0 || x.size() != y.size())
      throw PLplotException("vectors of length zero");

    for (unsigned i = 0; i < x.size(); ++i) {
      xp[i] = x(i);
      yp[i] = y(i);
    }

    xpmin = min(x);
    xpmax = max(x);
    ypmin = min(y);
    ypmax = max(y);

    pls->col0(15);
    pls->env( xpmin, xpmax, ypmin, ypmax, 0, 0 );
    pls->col0(14);
    pls->line(N, xp, yp);
  }
#endif

}

#endif
