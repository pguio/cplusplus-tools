/**************************************************************************
 *
 * $Id: hdf4-interface.cpp,v 1.9 2015/11/09 12:11:14 patrick Exp $
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

namespace hdf {

  using parser::header;

  using std::cout;
  using std::endl;
  using std::ostream;

#define ID "$Id: hdf4-interface.cpp,v 1.9 2015/11/09 12:11:14 patrick Exp $"


  Hdf::Hdf(int nargs, char *args[]) : HdfBase(nargs, args, "diagnostics.hdf"),
    sdId(-1), sdsId(-1), sdsIndex(-1), dimId(-1), status(-1)
  {
    initParsing(nargs, args);
    paramParsing();

    // If filename extension is .h5 replace it by .hdf
    HdfBase::ReplaceStringInPlace(fileName, ".h5", ".hdf");
  }

  Hdf::~Hdf()
  {}


  void Hdf::createSD(const string &version)
  {

    if ((sdId = SDstart(fileName.c_str(), DFACC_CREATE)) == FAIL)
      hdfError(__FILE__, __LINE__);

    // Initialise version
    int len = version.size();
    VOIDP ver = static_cast<VOIDP>(const_cast<char*>(version.c_str()));
    if ((status = SDsetattr(sdId, "Version", DFNT_CHAR, len, ver)) == FAIL)
      hdfError(__FILE__, __LINE__);

    if ((status = SDend(sdId)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDend()\n");

  }

  void Hdf::initSDvar(SDvar &V)
  {
    HdfBase::initSDvar(V);

    if ((sdId = SDstart(fileName.c_str(), DFACC_RDWR)) == FAIL)
      hdfError(__FILE__, __LINE__);

    if ((sdsId = SDcreate(sdId, V.varname.c_str(), V.vartype,
                          V.dimsize.size(), (int32*)&V.dimsize[0])) == FAIL)
      hdfError(__FILE__, __LINE__, "SDcreate()\n");

    int index = 0;
    for (unsigned d=0; d<V.dimsize.size(); ++d) {
      if ((dimId = SDgetdimid(sdsId, index++)) == FAIL)
        hdfError(__FILE__, __LINE__, "SDgetdimid()\n");

      if ((status = SDsetdimname(dimId, V.dimname[d].c_str())) == FAIL)
        hdfError(__FILE__, __LINE__, "SDsetdimname()\n");

      V.axis.resize(V.dimsize[d]);
      for (int32 s=0; s<V.dimsize[d]; ++s)
        V.axis[s] = V.start[d] + V.stride[d]*s;

      if ((status = SDsetdimscale(dimId, V.dimsize[d], V.dimtype[d],
                                  static_cast<VOIDP>(&V.axis[0]))) == FAIL)
        hdfError(__FILE__, __LINE__, "SDsetdimscale()\n");
    }
    if ((status = SDendaccess(sdsId)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDendaccess()\n");

    if ((status = SDend(sdId)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDend()\n");
  }

#if 0
  void Hdf::endInitSD()
  {
    if ((status = SDend(sdId)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDend()\n");
  }
#endif

  template <typename T_numtype>
  void Hdf::writeSDvar(const string &name, VecInt &start, VecInt &edge,
                       T_numtype *data)
  {

    HdfBase::writeSDvar(name, start, edge);

    if ((sdId = SDstart(fileName.c_str(), DFACC_RDWR)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDstart("+fileName+")\n");

    if ((sdsIndex = SDnametoindex(sdId, name.c_str())) == FAIL)
      hdfError(__FILE__, __LINE__, "SDnametoindex("+name+")\n");

    if ((sdsId = SDselect(sdId, sdsIndex)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDselect()\n");

    // start : 1d array start of slab (relative to 0) to be written
    // stride: 1d array interval between values in each dimension to be written
    //         for better performance for contiguous slab, use NULL stride
    // edge  : 1d array length of slab in each dimension to be written
    if ((status = SDwritedata(sdsId, (int32*)&start[0], NULL, (int32*)&edge[0],
                              (VOIDP)data)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDwritedata()\n");

    if ((status = SDendaccess(sdsId)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDendaccess()\n");

    if ((status = SDend(sdId)) == FAIL)
      hdfError(__FILE__, __LINE__, "SDend()\n");

    if (debugLevel() >= 5) {
      cout << "Hdf::writeSDvar(): done." << endl;
    }
  }

  template
  void Hdf::writeSDvar<float>
  (const string &name, VecInt &start, VecInt &edge,
   float *data);

  template
  void Hdf::writeSDvar<double>
  (const string &name, VecInt &start, VecInt &edge,
   double *data);


  void Hdf::hdfError(const string file, int line, const string extraMsg)
  {
#if 0
    HEprint(stderr, 0);
#endif

    std::stringstream msg;
    int i = 1;
    hdf_err_code_t e;
    while ( (e=(hdf_err_code_t)HEvalue(i)) != DFE_NONE ) {
      msg << "  error code (" << e << ") <" << HEstring(e) << ">\n";
      ++i;
    }
    std::stringstream where;
    where << "in " << file << ':' << line << ':' <<  extraMsg;

    throw ClassException("Hdf", where.str() + msg.str());
  }

  void Hdf::initParsing(int nargs, char *args[])
  {}

  void Hdf::paramParsing()
  {}

#undef ID

}

