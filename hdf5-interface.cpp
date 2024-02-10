/**************************************************************************
 *
 * $Id: hdf5-interface.cpp,v 1.21 2019/12/09 19:27:12 patrick Exp $
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
#include <algorithm>

namespace hdf {

  using parser::header;

  using std::cout;
  using std::endl;
  using std::ostream;

#define ID "$Id: hdf5-interface.cpp,v 1.21 2019/12/09 19:27:12 patrick Exp $"

  const hid_t type<short>::def  =  H5T_NATIVE_SHORT;
  const hid_t type<int>::def    =  H5T_NATIVE_INT;
  const hid_t type<long>::def   =  H5T_NATIVE_LONG;
  const hid_t type<float>::def  =  H5T_NATIVE_FLOAT;
  const hid_t type<double>::def =  H5T_NATIVE_DOUBLE;

  const hid_t type<char*>::def  =  H5T_C_S1;

  Hdf::Hdf(int nargs, char *args[]) : HdfBase(nargs, args, "diagnostics.h5")
  {
    initParsing(nargs, args);
    paramParsing();

    // If filename extension is .hdf replace it by .h5
    HdfBase::ReplaceStringInPlace(fileName, ".hdf", ".h5");
  }

  Hdf::~Hdf() {}


  void Hdf::createSD(const string &version)
  {

    // Create or overwrite file
    file = H5Fcreate(fileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file < 0) hdfError(__FILE__, __LINE__);

    // create global attribute version
    dataspace = H5Screate(H5S_SCALAR);
    datatype  = H5Tcopy(type<char*>::def);
    status    = H5Tset_size(datatype, version.length());
#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
    attribute = H5Acreate(file, "Version", datatype, dataspace,
                          H5P_DEFAULT);
#else
    attribute = H5Acreate(file, "Version", datatype, dataspace,
                          H5P_DEFAULT, H5P_DEFAULT);
#endif
    if (attribute < 0) hdfError(__FILE__, __LINE__);

    status = H5Awrite(attribute, datatype, version.c_str());

    status = H5Aclose(attribute);
    status = H5Sclose(dataspace);

    // create group for dimensions
#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
    group = H5Gcreate(file, "/dims", H5P_DEFAULT);
#else
    group = H5Gcreate(file, "/dims", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
#endif
    if (group < 0) hdfError(__FILE__, __LINE__);

    status = H5Gclose(group);
    status = H5Fclose(file);
  }

  void Hdf::initSDvar(SDvar &V)
  {
    HdfBase::initSDvar(V);

    file = H5Fopen(fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    if (file < 0) hdfError(__FILE__, __LINE__);

    hsize_t rank = V.dimsize.size();
    std::vector<hsize_t> dims(rank);
    std::copy(V.dimsize.begin(), V.dimsize.end(), dims.begin());

    // init variable
    dataspace = H5Screate_simple(rank, &dims[0], 0);
    datatype = H5Tcopy(V.vartype);
#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
    varset = H5Dcreate(file, V.varname.c_str(), datatype, dataspace,
                       H5P_DEFAULT);
#else
    varset = H5Dcreate(file, V.varname.c_str(), datatype, dataspace,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
#endif
   if (varset < 0) hdfError(__FILE__, __LINE__);

    status = H5Tclose(datatype);
    status = H5Dclose(varset);
    status = H5Sclose(dataspace);

    // open group for dimensions
#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
    group = H5Gopen(file, "/dims");
#else
    group = H5Gopen(file, "/dims", H5P_DEFAULT);
#endif
    if (group < 0) hdfError(__FILE__, __LINE__);

    std::vector<void*> ref(rank);
    for (unsigned d=0; d<rank; ++d) {
      hsize_t dims = V.dimsize[d];

      // describe size of array and create data space for fixed size dataset
      dataspace = H5Screate_simple(1, &dims, NULL);

      // define datatype for the data in the file.
      datatype = H5Tcopy(V.dimtype[d]);

#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
// Trick described in H5Epublic.h:
// One often needs to temporarily disable automatic error reporting when
// trying something that's likely or expected to fail.  For instance, to
// determine if an object exists one can call H5Gget_objinfo() which will
// fail if the object doesn't exist.  The code to try can be nested between
// calls to H5Eget_auto() and H5Eset_auto(), but it's easier just to use
// this macro like
// H5E_BEGIN_TRY {
//     ...stuff here that's likely to fail...
//     } H5E_END_TRY;
// Warning: don't break, return, or longjmp() from the body of the loop or
// the error reporting won't be properly restored!
H5E_BEGIN_TRY {
      // (re)create dimension
      dimset = H5Dcreate(group, V.dimname[d].c_str(), datatype, dataspace,
                         H5P_DEFAULT);
      V.axis.resize(V.dimsize[d]);
      for (VecInt::value_type s=0; s<V.dimsize[d]; ++s)
        V.axis[s] = V.start[d] + V.stride[d]*s;

      status = H5Dwrite(dimset, type<float>::def,
                        H5S_ALL, H5S_ALL, H5P_DEFAULT, &V.axis[0]);

      status = H5Dclose(dimset);
} H5E_END_TRY;
#else
      if ( !H5Lexists(group,V.dimname[d].c_str(), H5P_DEFAULT)) {
        // dimension does not exists, create it
        dimset = H5Dcreate(group, V.dimname[d].c_str(), datatype, dataspace,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        V.axis.resize(V.dimsize[d]);
        for (VecInt::value_type s=0; s<V.dimsize[d]; ++s)
          V.axis[s] = V.start[d] + V.stride[d]*s;

        status = H5Dwrite(dimset, type<float>::def,
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, &V.axis[0]);
        //if (status < 0) hdfError(__FILE__, __LINE__); 

        status = H5Dclose(dimset);
      }
#endif

      // keep reference of dimensions for variable
      status = H5Rcreate(&ref[d], group,V.dimname[d].c_str(),H5R_OBJECT, -1);
      //if (status < 0) hdfError(__FILE__, __LINE__);
    }

    // save references to dimensions in variable attribute dims_ref
#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
    varset = H5Dopen(file, V.varname.c_str());
#else
    varset = H5Dopen(file, V.varname.c_str(), H5P_DEFAULT);
#endif
    dataspace = H5Screate(H5S_SIMPLE);
    status = H5Sset_extent_simple(dataspace, 1, &rank, NULL);
    datatype = H5Tcopy(H5T_STD_REF_OBJ);
#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
    attribute = H5Acreate(varset, "dims_ref", datatype,
                          dataspace, H5P_DEFAULT);
#else
    attribute = H5Acreate(varset, "dims_ref", datatype,
                          dataspace, H5P_DEFAULT, H5P_DEFAULT);
#endif

    status = H5Awrite(attribute, datatype, &ref[0]);
    if (status < 0) hdfError(__FILE__, __LINE__);

    status = H5Aclose(attribute);
    status = H5Tclose(datatype);
    status = H5Dclose(varset);
    status = H5Sclose(dataspace);
    status = H5Gclose(group);
    status = H5Fclose(file);
  }


  template <typename T_numtype>
  void Hdf::writeSDvar(const string &name, VecInt &start, VecInt &edge,
                       T_numtype *data)
  {
    HdfBase::writeSDvar(name, start, edge);

    // Open the file
    file = H5Fopen(fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    if (file < 0) hdfError(__FILE__, __LINE__);

    // Open the dataset
#if defined(H5_USE_16_API) || H5_VERS_MINOR < 8 // prior to version 1.8
    dataset = H5Dopen(file, name.c_str());
#else
    dataset = H5Dopen(file, name.c_str(), H5P_DEFAULT);
#endif
    if (dataset < 0) hdfError(__FILE__, __LINE__);

    // Get dataset rank and dimension
    hid_t filespace = H5Dget_space(dataset); // Get filespace handle first
    hsize_t rank = H5Sget_simple_extent_ndims(filespace);

    std::vector<hsize_t> dims(rank);
    status = H5Sget_simple_extent_dims(filespace, &dims[0], NULL);

#if 0
    std::cout << "dims = " << dims << std::endl;
#endif

    // Define the memory space to write dataset
    std::vector<hsize_t> dims_sub(rank);
    std::vector<hsize_t> offset(rank);
    std::vector<hsize_t> count(rank);
    for (unsigned i=0; i<rank; ++i) {
      dims_sub[i] = edge[i];
      offset[i] = start[i];
      count[i] = edge[i];
    }
#if 0
    std::cout << "dims_sub = " << dims_sub << std::endl;
#endif
    hid_t memspace = H5Screate_simple(rank, &dims_sub[0], NULL);

    // Define the hyperslab as start (offset), stride, count, block
    // For example, a 2d dataspace with hyperslab selection settings
    // start (offset) [1,1], stride [4,4], count [3,7], and block [2,2]
    // specifies, in C, a hyperslab consisting of 21 2x2 blocks of
    // array elements starting with location (1,1) with selected blocks
    // at locations (1,1), (5,1), (9,1), (1,5), (5,5), etc.;
    status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET,
                                 &offset[0], NULL, &count[0], NULL);

    if ((status = H5Dwrite(dataset, type<T_numtype>::def,
                           memspace, filespace, H5P_DEFAULT, data)) < 0)
      hdfError(__FILE__, __LINE__);

    H5Dclose(dataset);
    H5Fclose(file);

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
    std::stringstream msg;
    std::stringstream where;
    where << "in " << file << ':' << line << ':' <<  extraMsg;

    throw ClassException("Hdf", where.str() + msg.str());
  }

  void Hdf::initParsing(int nargs, char *args[]) {}

  void Hdf::paramParsing() {}

}

#undef ID
