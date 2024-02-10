/**************************************************************************
 *
 * $Id: fourier-mlib.h,v 1.13 2011/03/26 07:05:54 patrick Exp $
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

#include <veclib.h>

namespace fourier {

  template <>
  class IDFT1D<double, double> : public InPlace<double, double> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<double, double>(_n, _direct_sign, "MLIB") {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      drc1ft(in.data(), const_cast<int*>(&n), const_cast<double*>(work),
             const_cast<int*>(&forward), const_cast<int*>(&ier));
      if (direct_sign == 1)
        in *= static_cast<double>(n);
#if 0

      if (ier) {
        ostringstream os;
        os << "drc1ft in direct returned ier = " << ier;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void inverse(Array1di &in) const {
      drc1ft(in.data(), const_cast<int*>(&n), const_cast<double*>(work),
             const_cast<int*>(&bckward), const_cast<int*>(&ier));
      if (direct_sign == 1)
        in *= (1.0/n);
#if 0

      if (ier) {
        ostringstream os;
        os << "z1dfft in inverse returned ier = " << ier;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      delete[] work;
    }

  private:

    int iopt;
    int ier;
    int forward;
    int bckward;
    double *work;
    size_t blckSize;

    virtual void create() {
      iopt=-3;
      work = new double[(5*n)/2];
      blckSize = n*sizeof(complex);
      drc1ft(0, &n, work, &iopt, &ier);
      if (ier) {
        ostringstream os;
        os << "z1dfft in ctor returned ier = " << ier;
        throw ClassException("IDFT1D", os.str());
      }
      if (direct_sign == -1) {
        forward =  1;
        bckward = -2; // unscaled
        bckward = -1; // scaled
      } else {
        forward = -2; // unscaled
        forward = -1; // scaled
        bckward =  1;
      }
    }
  };

  template <>
  class IDFT1D<complex, complex> : public InPlace<complex, complex> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<complex, complex>(_n, _direct_sign, "MLIB") {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      z1dfft(reinterpret_cast<complex16_t*>(in.data()), const_cast<int*>(&n),
             const_cast<double*>(work), const_cast<int*>(&forward),
             const_cast<int*>(&ier));
      if (direct_sign == 1)
        in *= static_cast<double>(n);
#if 0

      if (ier) {
        ostringstream os;
        os << "z1dfft in direct returned ier = " << ier;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void inverse(Array1di &in) const {
      z1dfft(reinterpret_cast<complex16_t*>(in.data()), const_cast<int*>(&n),
             const_cast<double*>(work), const_cast<int*>(&bckward),
             const_cast<int*>(&ier));
      if (direct_sign == 1)
        in *= (1.0/n);
#if 0

      if (ier) {
        ostringstream os;
        os << "z1dfft in inverse returned ier = " << ier;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      delete[] work;
    }

  private:

    int iopt;
    int ier;
    int forward;
    int bckward;
    double *work;
    size_t blckSize;

    virtual void create() {
      iopt=-3;
      work = new double[(5*n)/2];
      blckSize = n*sizeof(complex);
      z1dfft(0, &n, work, &iopt, &ier);
      if (ier != 0) {
        ostringstream os;
        os << "z1dfft in ctor returned ier = " << ier;
        throw ClassException("FT1d", os.str());
      }
      if (direct_sign == -1) {
        forward =  1;
        bckward = -2; // unscaled
        bckward = -1; // scaled
      } else {
        forward = -2; // unscaled
        forward = -1; // scaled
        bckward =  1;
      }
    }
  };

  template <>
  class ODFT1D<double, double> : public OutPlace<double, double> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<double, double>(_n, _direct_sign, "MLIB") {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      memcpy(out.data(), in.data(), blckSize);
      drc1ft(out.data(), const_cast<int*>(&n), const_cast<double*>(work),
             const_cast<int*>(&forward), const_cast<int*>(&ier));
      if (direct_sign == 1)
        out *= static_cast<double>(n);
#if 0

      if (ier) {
        ostringstream os;
        os << "drc1ft in direct returned ier = " << ier;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      memcpy(out.data(), in.data(), blckSize);
      drc1ft(out.data(), const_cast<int*>(&n), const_cast<double*>(work),
             const_cast<int*>(&bckward), const_cast<int*>(&ier));
      if (direct_sign == 1)
        out *= (1.0/n);
#if 0

      if (ier) {
        ostringstream os;
        os << "drc1ft in inverse returned ier = " << ier;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      delete[] work;
    }

  private:

    int iopt;
    int ier;
    int forward;
    int bckward;
    double *work;
    size_t blckSize;

    virtual void create() {
      iopt=-3;
      work = new double[(5*n)/2];
      blckSize = n*sizeof(double);
      drc1ft(0, &n, work, &iopt, &ier);
      if (ier) {
        ostringstream os;
        os << "z1dfft in ctor returned ier = " << ier;
        throw ClassException("ODFT1D", os.str());
      }
      if (direct_sign == -1) {
        forward =  1;
        bckward = -2; // unscaled
        bckward = -1; // scaled
      } else {
        forward = -2; // unscaled
        forward = -1; // scaled
        bckward =  1;
      }
    }
  };

  template <>
  class ODFT1D<complex, complex> : public OutPlace<complex, complex> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<complex, complex>(_n, _direct_sign, "MLIB") {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      memcpy(out.data(), in.data(), blckSize);
      z1dfft(reinterpret_cast<complex16_t*>(out.data()), const_cast<int*>(&n),
             const_cast<double*>(work), const_cast<int*>(&forward),
             const_cast<int*>(&ier));
      if (direct_sign == 1)
        out *= static_cast<double>(n);
#if 0

      if (ier) {
        ostringstream os;
        os << "z1dfft in direct returned ier = " << ier;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      memcpy(out.data(), in.data(), blckSize);
      z1dfft(reinterpret_cast<complex16_t*>(out.data()), const_cast<int*>(&n),
             const_cast<double*>(work), const_cast<int*>(&bckward),
             const_cast<int*>(&ier));
      if (direct_sign == 1)
        out *= (1.0/n);
#if 0

      if (ier) {
        ostringstream os;
        os << "z1dfft in inverse returned ier = " << ier;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      delete[] work;
    }

  private:

    int iopt;
    int ier;
    int forward;
    int bckward;
    double *work;
    size_t blckSize;

    virtual void create() {
      iopt=-3;
      work = new double[(5*n)/2];
      blckSize = n*sizeof(complex);
      z1dfft(0, &n, work, &iopt, &ier);
      if (ier != 0) {
        ostringstream os;
        os << "z1dfft in ctor returned ier = " << ier;
        throw ClassException("ODFT1D", os.str());
      }
      if (direct_sign == -1) {
        forward =  1;
        bckward = -2; // unscaled
        bckward = -1; // scaled
      } else {
        forward = -2; // unscaled
        forward = -1; // scaled
        bckward =  1;
      }
    }
  };

}

