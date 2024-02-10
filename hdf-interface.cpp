/**************************************************************************
 *
 * $Id: hdf-interface.cpp,v 1.50 2013/11/08 17:35:14 patrick Exp $
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

#include <hdf-interface.h>
#include <iomanip>

namespace hdf {

  using parser::header;

  using std::cout;
  using std::endl;
  using std::ostream;

#define ID "$Id: hdf-interface.cpp,v 1.50 2013/11/08 17:35:14 patrick Exp $"

  template <class T_type>
  ostream &operator<<(ostream &os, const std::vector<T_type> &v)
  {
    using std::setw;
    int w=os.width();

    if (!v.empty()) {
      typename std::vector<T_type>::const_iterator i = v.begin();
      typename std::vector<T_type>::const_iterator end = v.end();
      os << *i;
      for ( ++i; i != end; ++i )
        os << ", " << setw(w) << *i;
    }
    return os;
  }

  ostream &operator<<(ostream &os, const SDvar &V)
  {
    using std::setw;
    int w=15;

    std::ios::fmtflags f = os.flags() & std::ios::adjustfield;

    return os
           << "SD variable: " << V.varname << " type: " << V.vartype
           << " with dimensions: "
           << '\n' << std::left
           << "     name  : " << setw(w) << V.dimname << '\n'
           << "     type  : " << setw(w) << V.dimtype << '\n'
           << "     size  : " << setw(w) << V.dimsize << '\n'
           << "     start : " << setw(w) << V.start << '\n'
           << "     stride: " << setw(w) << V.stride << '\n'
           << "     end   : " << setw(w) << V.end
           << std::resetiosflags(std::ios::left) << std::setiosflags(f) << endl;
  }

  SDvar::SDvar(int d) : vartype(0), varname(),
    dimsize(d), dimname(d), dimtype(d),
    start(d), stride(d), end(d), axis(0)
  {}


  void SDvar::resize(int d)
  {
    dimsize.resize(d);
    dimname.resize(d);
    dimtype.resize(d);
    start.resize(d);
    stride.resize(d);
    end.resize(d);
  }

  void SDvar::initialiseDim(int d, VecFloat &axis, const string &name)
  {
    dimsize[d] = axis.size();
    dimname[d] = name;
    start[d] = axis[0];
    stride[d] = (axis[dimsize[d]-1]-axis[0])/(dimsize[d]-1);
    end[d] = axis[dimsize[d]-1];
  }

  ostream& operator<<(ostream& os, const HdfBase &hdf)
  {
    return os
           << header("HDF setup")
           << "HDF filename = " << hdf.fileName << endl;
  }

  HdfBase::HdfBase(int nargs, char *args[], const string &filename) :
    Parser(nargs, args), fileName(filename)
  {
    initParsing(nargs, args);
    paramParsing();
  }

  HdfBase::~HdfBase()
  {}

  void HdfBase::initSDvar(SDvar &V)
  {
    if (debugLevel() >= 5)
      cout << "Hdf::initSDvar(): " << endl <<  V << endl;
  }

  string HdfBase::getFilename() const
  {
    return fileName;
  }

  void HdfBase::writeSDvar(const string &name, VecInt &start, VecInt &edge)
  {
    if (debugLevel() >= 5) {
      using std::setw;
      int w=10;
      cout << "Hdf::writeSDvar(): " << endl
           << "  name = " << name << endl
           << "  start= " << setw(w) << start << endl
           << "  edge = " << setw(w) << edge << endl;

    }
  }

  void HdfBase::ReplaceStringInPlace(std::string& subject,
                                     const std::string& search,
                                     const std::string& replace)
  {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
    }
  }

  void HdfBase::initParsing(int nargs, char *args[])
  {
    using namespace parser::types;
    registerClass("Hdf");
    registerPackage(PACKAGE, VERSION "\n" ID "\n", HDF_INTERFACE_COPYRIGHT);
    parseLevelDebugOption("Hdf::dl");
    insertOption(_fileName, "-o", charStr,
                 "Filename of the HDF diagnostics file", Any(fileName));
    insertOptionAlias(_fileName, "--output");
    insertOptionAlias(_fileName, "output");

  }
  void HdfBase::paramParsing()
  {
    parseOption(_fileName, fileName);

  }

#undef ID

}


#if defined(HAVE_HDF4)
#include <hdf4-interface.cpp>
#elif defined(HAVE_HDF5)
#include <hdf5-interface.cpp>
#else
#error in <hdf-interface.cpp>: no HDF interface implementation specified!
#endif

