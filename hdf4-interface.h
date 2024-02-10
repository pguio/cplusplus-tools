/**************************************************************************
 *
 * $Id: hdf4-interface.h,v 1.5 2011/03/26 07:05:54 patrick Exp $
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


#ifndef HDF4_INTERFACE_H
#define HDF4_INTERFACE_H

#include <hdf.h>
#include <mfhdf.h>

namespace hdf {

  template<typename T_numtype>
  struct type {
    static const int def = DFNT_NONE;
  };

  template<>
  struct type<char> {
    static const int def = DFNT_CHAR;
  };

  template<>
  struct type<float> {
    static const int def = DFNT_FLOAT32;
  };

  template<>
  struct type<double> {
    static const int def = DFNT_FLOAT64;
  };


  class Hdf : public HdfBase {
  public:
    Hdf(int nargs, char *args[]);
    virtual ~Hdf();

    virtual void createSD(const string &version);
    virtual void initSDvar(SDvar &V);
#if 0
    virtual void endInitSD();
#endif
    template <typename T_numtype>
    void writeSDvar(const string &name, VecInt &start, VecInt &edge,
                    T_numtype *data);

  private:

    int32 sdId, sdsId, sdsIndex, dimId;
    intn status;

    void hdfError(const string file, int line, const string msg="");
    void initParsing(int nargs, char *args[]);
    void paramParsing();

  };

}

#endif // HDF4_INTERFACE_H

