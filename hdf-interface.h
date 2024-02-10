/**************************************************************************
 *
 * $Id: hdf-interface.h,v 1.40 2019/12/09 19:27:12 patrick Exp $
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

#ifndef HDF_INTERFACE_H
#define HDF_INTERFACE_H

#define HDF_INTERFACE_COPYRIGHT \
"Copyright (c) 2000-2011 Patrick Guio <patrick.guio@gmail.com>\n\n"\
"This is free software; see the source for copying conditions.  There is NO\n"\
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <classexception.h>
#include <parser.h>

#if defined(HAVE_HDF5) // define hid_t type
#include <H5Ipublic.h>
#endif

namespace hdf {
  typedef std::string string;

#if defined(HAVE_HDF4)
  typedef int datatype;
#elif defined(HAVE_HDF5)
  typedef hid_t datatype;
#endif

  typedef std::vector<string> VecStr;
  typedef std::vector<int> VecInt;
  typedef std::vector<float> VecFloat;
  typedef std::vector<datatype> VecDataType;

  struct SDvar {
    friend std::ostream &operator<<(std::ostream &os, const SDvar &V);

    datatype vartype;
    string varname;
    VecInt dimsize;
    VecStr dimname;
    VecDataType dimtype;
    VecFloat start, stride, end, axis;

    explicit SDvar(int d=0);
    void resize(int d);
    void initialiseDim(int d, VecFloat &axis, const string &name);
  };

  class HdfBase : public parser::Parser {
  public:

    friend std::ostream& operator<<(std::ostream& os, const HdfBase &hdf);

    HdfBase(int nargs, char *args[], const string &filename);
    virtual ~HdfBase();

    virtual void createSD(const string &version) = 0;
    virtual void initSDvar(SDvar &V);
#if 0
    virtual void endInitSD() = 0;
#endif
    template <typename T_numtype>
    void writeSDvar(const string &name, VecInt &start, VecInt &edge,
                    T_numtype *data);

    string getFilename() const;

  protected:

    typedef parser::Parser Parser;

    string fileName;

    void writeSDvar(const string &name, VecInt &start, VecInt &edge);
    void ReplaceStringInPlace(string& subject, const string& search,
                              const string& replace);

  private:

    enum parser_enum { _fileName=1 };
    void initParsing(int nargs, char *args[]);
    void paramParsing();
  };

}


#if defined(HAVE_HDF4)
#include <hdf4-interface.h>
#elif defined(HAVE_HDF5)
#include <hdf5-interface.h>
#else
#error in <hdf-interface.h>: no HDF interface implementation specified!
#endif


#endif // HDF_INTERFACE_H
