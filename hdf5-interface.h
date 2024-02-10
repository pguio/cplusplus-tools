/**************************************************************************
 *
 * $Id: hdf5-interface.h,v 1.8 2012/11/07 17:39:48 patrick Exp $
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
 **************************************************************************/


#ifndef HDF5_INTERFACE_H
#define HDF5_INTERFACE_H

#include <hdf5.h>
#if 0
// high level interface
#include <hdf5_hl.h>
#endif

namespace hdf {

  template<typename T_numtype>
  struct type {
    static const hid_t  def = H5T_NO_CLASS;
  };

  template<> struct type<char>   {
    static const hid_t  def;
  };
  template<> struct type<short>  {
    static const hid_t  def;
  };
  template<> struct type<int>    {
    static const hid_t  def;
  };
  template<> struct type<long>   {
    static const hid_t  def;
  };
  template<> struct type<float>  {
    static const hid_t  def;
  };
  template<> struct type<double> {
    static const hid_t  def;
  };

  template<> struct type<char*>  {
    static const hid_t  def;
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

    hid_t file, root, group;
    hid_t dataset, varset, dimset;
    hid_t attribute, dataspace, datatype;
    herr_t status;

    void hdfError(const string file, int line, const string msg="");
    void initParsing(int nargs, char *args[]);
    void paramParsing();

  };

}

#endif // HDF5_INTERFACE_H

