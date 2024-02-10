/**************************************************************************
 *
 * $Id: gnuplot-interface.h,v 1.10 2011/04/05 19:24:44 patrick Exp $
 *
 * Copyright (c) 2004-2011 Patrick Guio <patrick.guio@gmail.com>
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

/**************************************************************************
 *
 * A C++ interface to gnuplot.
 *
 * This is a direct translation from the C interface
 *  written by N. Devillard (which is available from
 *  http://ndevilla.free.fr/gnuplot/).
 *
 * As in the C interface this uses pipes and so wont
 *  run on a system that does'nt have POSIX pipe
 * support
 *
 **************************************************************************/

#ifndef GNUPLOT_INTERFACE_H
#define GNUPLOT_INTERFACE_H

#include <stdarg.h>
#include <unistd.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <stdexcept>

namespace gnuplot {

  const int GP_MAX_TMP_FILES = 64;
  const int GP_TMP_NAME_SIZE = 512;
  const int GP_CMD_SIZE      = 1024;
  const int GP_TITLE_SIZE    = 80;

  using std::string;
  using std::vector;

  class GnuplotException : public std::runtime_error {
  public:
    explicit GnuplotException(const std::string msg) : std::runtime_error(msg)
    {}
  }
  ;

  class Gnuplot {
  public:

    typedef std::string string;
    typedef std::vector<double> Vector;

    explicit Gnuplot(const string style="lines");

    ~Gnuplot();

    // send a command to gnuplot
    void sendCmd(const char*, ...);

    void setLineStyle(const string style);
    void setXLabel(const string label);
    void setYLabel(const string label);
    void setZLabel(const string label);
    void setTitle(const string title);

    template <typename Num_type>
    void plot(std::vector<Num_type> &x, const string legend="");
    template <typename Num_type>
    void plot(std::vector<Num_type> &x, std::vector<Num_type> &y, const string legend="");

#ifdef BZ_ARRAY_H

    template <typename Num_type>
    void plot(blitz::Array<Num_type,1> &x, const string legend="");
    template <typename Num_type>
    void plot(blitz::Array<Num_type,1> &x, blitz::Array<Num_type,1> &y, const string legend="");
#endif

    void plot(const string &function, const string legend="");

    void pause(unsigned seconds=0);

    void reset();

  private:

    void             initialise(const string);
    string           getTmpFile();


    FILE            *pipe;
    vector<string>   garbage;
    int              numPlots;

    string           lineStyle;

  };


  inline const string quote(const string str)
  {
    return string("\"" + str + "\"");
  }

#ifdef BZ_ARRAY_H

  template <typename Num_type>
  void Gnuplot::plot(blitz::Array<Num_type,1> &d, const string legend)
  {
    if (d.size() == 0 )
      throw GnuplotException("vectors of length zero");
    string name(getTmpFile());
    // write the data to file
    std::ofstream tmp(name.c_str());
    for (unsigned i = 0; i < d.size(); ++i)
      tmp << d(i) << std::endl;
    tmp.close();
    // command to be sent to gnuplot
    string plot  = (numPlots > 0 ? "replot " : "plot ");
    string title = (legend.empty() ? " notitle" : string(" title " + quote(legend)));
    string cmd(plot + quote(name) + title + " with " + lineStyle);

    // Do the actual plot
    sendCmd(cmd.c_str());
    ++numPlots;
  }

  template <typename Num_type>
  void Gnuplot::plot(blitz::Array<Num_type,1> &x, blitz::Array<Num_type,1> &y, const string legend)
  {
    if (x.size() == 0 || y.size() == 0 || x.size() != y.size())
      throw GnuplotException("vectors of length zero");
    string name(getTmpFile());
    // write the data to file
    std::ofstream tmp(name.c_str());
    for (unsigned i = 0; i < x.size(); ++i)
      tmp << x(i) << " " << y(i) << std::endl;
    tmp.close();
    // command to be sent to gnuplot
    string plot  = (numPlots > 0 ? "replot " : "plot ");
    string title = (legend.empty() ? " notitle" : string(" title " + quote(legend)));
    string cmd(plot + quote(name) + title + " with " + lineStyle);

    // Do the actual plot
    sendCmd(cmd.c_str());
    ++numPlots;
  }
#endif

}

#endif
