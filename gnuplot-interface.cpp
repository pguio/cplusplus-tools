/**************************************************************************
 *
 * $Id: gnuplot-interface.cpp,v 1.9 2011/04/05 20:45:44 patrick Exp $
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


#include <gnuplot-interface.h>

namespace gnuplot {

  const int PATH_MAXNAMESZ = 4096;

  using std::cout;
  using std::endl;

  using std::list;

  using std::string;
  using std::ofstream;
  using std::ostringstream;

  /* example:
   *       list<string>       ls;
   *       stringtok (ls, " this  \t is\t\n  a test  ");
   *       for (list<string>::const_iterator i = ls.begin();
   *            i != ls.end(); ++i)
   *       {
   *            cerr << ':' << (*i) << ":\n";
   *       }
   */
  inline bool
  isws (char c, char const * const wstr)
  {
    using namespace std;
    return (strchr(wstr,c) != NULL);
  }


  template <typename Container>
  void
  stringtok (Container &l, string const &s, const char * const ws = " \t\n")
  {
    typedef std::string::size_type size_type;

    const size_type  S = s.size();
    size_type  i = 0;

    while (i < S) {
      // eat leading whitespace
      while ((i < S) && (isws(s[i],ws)))
        ++i;
      if (i == S)
        return;  // nothing left but WS

      // find end of word
      size_type  j = i+1;
      while ((j < S) && (!isws(s[j],ws)))
        ++j;

      // add word
      l.push_back(s.substr(i,j-i));

      // set up for next loop
      i = j+1;
    }
  }


  Gnuplot::Gnuplot(const string style) : pipe(0), garbage(0),
    numPlots(0), lineStyle(style)
  {
    initialise("gnuplot");
  }

  Gnuplot::~Gnuplot()
  {
    for (vector<string>::iterator i=garbage.begin(), e=garbage.end(); i != e; ++i)
      remove
      (i->c_str());

    garbage.clear();

    if (pclose(pipe))
      throw GnuplotException("Problem closing communication to gnuplot");
  }

  void Gnuplot::sendCmd(const char *cmd, ...)
  {
    va_list ap;
    char local_cmd[GP_CMD_SIZE];

    va_start(ap, cmd);
    vsprintf(local_cmd, cmd, ap);
    va_end(ap);
    strcat(local_cmd, "\n");
    fputs(local_cmd, pipe);
    fflush(pipe);
  }

  void Gnuplot::setLineStyle(const string style)
  {
    lineStyle = style;
  }

  void Gnuplot::setXLabel(const string label)
  {
    string cmd("set xlabel " + quote(label));
    sendCmd(cmd.c_str());
  }

  void Gnuplot::setYLabel(const string label)
  {
    string cmd("set ylabel " + quote(label));
    sendCmd(cmd.c_str());
  }

  void Gnuplot::setZLabel(const string label)
  {
    string cmd("set zlabel " + quote(label));
    sendCmd(cmd.c_str());
  }

  void Gnuplot::setTitle(const string title)
  {
    string cmd("set title " + quote(title));
    sendCmd(cmd.c_str());
  }


  void Gnuplot::plot(const string &function, const string legend)
  {

    string plot  = (numPlots > 0 ? "replot " : "plot ");
    string title = (legend.empty() ? " notitle" : string(" title " + quote(legend)));

    string cmd(plot + function + title + " with " + lineStyle);

    sendCmd(cmd.c_str());
    ++numPlots;
  }

  template <typename Num_type>
  void Gnuplot::plot(std::vector<Num_type> &d, const string legend)
  {
    if (d.empty() )
      throw GnuplotException("vectors of length zero");

    string name(getTmpFile());
    // write the data to file
    ofstream tmp(name.c_str());
    for (unsigned i = 0; i < d.size(); ++i)
      tmp << d[i] << endl;
    tmp.close();

    // command to be sent to gnuplot
    string plot  = (numPlots > 0 ? "replot " : "plot ");
    string title = (legend.empty() ? " notitle" : string(" title " + quote(legend)));
    string cmd(plot + quote(name) + title + " with " + lineStyle);

    // Do the actual plot
    sendCmd(cmd.c_str());
    ++numPlots;
  }

  template void Gnuplot::plot(std::vector<double> &x, const string legend)
  ;

  template <typename Num_type>
  void Gnuplot::plot(std::vector<Num_type> &x, std::vector<Num_type> &y, const string legend)
  {
    if (x.empty() || y.empty() || x.size() != y.size())
      throw GnuplotException("vectors of length zero or different size");

    string name(getTmpFile());
    // write the data to file
    ofstream tmp(name.c_str());
    for (unsigned i = 0; i < x.size(); ++i)
      tmp << x[i] << " " << y[i] << endl;
    tmp.close();

    // command to be sent to gnuplot
    string plot  = (numPlots > 0 ? "replot " : "plot ");
    string title = (legend.empty() ? " notitle" : string(" title " + quote(legend)));
    string cmd(plot + quote(name) + title + " with " + lineStyle);

    // Do the actual plot
    sendCmd(cmd.c_str());
    ++numPlots;
  }

  template void Gnuplot::plot(std::vector<double> &x, std::vector<double> &y, const string title)
  ;


  void Gnuplot::pause(unsigned seconds)
  {
    if (seconds == 0) {
      cout << "Press enter to continue" << endl;
      while (getchar() != '\n') {}
    } else {
      sleep(seconds);
    }

  }

  void Gnuplot::reset()
  {
    for (vector<string>::iterator i=garbage.begin(), e=garbage.end(); i != e; ++i)
      remove
      (i->c_str());

    garbage.clear();
    numPlots = 0;
  }

  void Gnuplot::initialise(const string pname)
  {
    if (getenv("DISPLAY") == NULL) {
      throw GnuplotException("cannot find DISPLAY variable");
    }

    char *path = getenv("PATH");

    if (! path) {
      throw GnuplotException("PATH variable not set");
    } else {
      list<string> ls;
      stringtok(ls, path, ":");
      for (list<string>::const_iterator i = ls.begin(); i != ls.end(); ++i) {
        string tmp = (*i) + "/" + pname;
        if (access(tmp.c_str(), X_OK)) {
          pipe = popen("gnuplot", "w");
          if (! pipe) {
            throw GnuplotException("Couldn't open connection to gnuplot");
          }
          return;
        }
      }
    }
    throw GnuplotException("Can't find gnuplot in your PATH");

  }

  string Gnuplot::getTmpFile()
  {
    char tmpname[] = "/tmp/gnuplotiXXXXXX";
    string name;

    if (garbage.size() == GP_MAX_TMP_FILES - 1) {
      throw GnuplotException("Maximum number of temporary files reached");
    }
    //open temporary files for output
    if (mkstemp(tmpname) == -1) {
      throw GnuplotException("Cannot create temporary file: "+string(tmpname));
    }
    ofstream tmp(tmpname);
    if (tmp.bad()) {
      throw GnuplotException("Cannot create temorary file: exiting plot");
    } else {
      tmp.close();
      name = tmpname;
    }

    // Save the temporary filename
    garbage.push_back(name);

    return tmpname;
  }
}

