/**************************************************************************
 *
 * $Id: plplot-interface.cpp,v 1.2 2011/03/26 07:05:54 patrick Exp $
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


#include <plplpot-interface.h>

namespace gnuplot {

  PLplot::PLplot(const string style) : numPlots(0), lineStyle(style)
  {
    pls = new plstream();
    pls->sdev("xwin");
    pls->init();           // Initialize PLplot
    xp = new PLFLT[N];
    yp = new PLFLT[N];
    pls->fontld(1);        // Select the multi-stroke font
    pls->font(1);          //
    pls->spause(0);
  }

  PLplot::~PLplot()
  {
  }

  void PLplot::setLineStyle(const string style)
  {
    lineStyle = style;
  }

  void PLplot::setXLabel(const string label)
  {
  }

  void PLplot::setYLabel(const string label)
  {
  }

  void PLplot::setZLabel(const string label)
  {
  }

  void PLplot::setTitle(const string title)
  {
  }

  template <typename Num_type>
  void PLplot::plot(std::vector<Num_type> &d, const string legend)
  {
    if (d.empty() )
      throw PLplotException("vectors of length zero");

    for (unsigned i = 0; i < d.size(); ++i)
      xp[i] = d[i];

  }

  template void PLplot::plot(std::vector<double> &x, const string legend);

  template <typename Num_type>
  void PLplot::plot(std::vector<Num_type> &x, std::vector<Num_type> &y, const string legend)
  {
    if (x.empty() || y.empty() || x.size() != y.size())
      throw PLplotException("vectors of length zero or different size");

    for (unsigned i = 0; i < x.size(); ++i) {
      xp[i] = x(i);
      yp[i] = y(i);
    }
  }

  template void PLplot::plot(std::vector<double> &x, std::vector<double> &y, const string title) ;


  void PLplot::pause(unsigned seconds)
  {
    if (seconds == 0) {
      cout << "Press enter to continue" << endl;
      while (getchar() != '\n') {}
    } else {
      sleep(seconds);
    }

  }

  void PLplot::reset()
  {
  }
}


