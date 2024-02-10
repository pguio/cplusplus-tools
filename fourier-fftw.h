/**************************************************************************
 *
 * $Id: fourier-fftw.h,v 1.11 2011/03/26 07:05:54 patrick Exp $
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

#include <fftw.h>
#include <rfftw.h>

namespace fourier {

  template <>
  class IDFT1D<double, double> : public InPlace<double, double> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<double, double>(_n, _direct_sign, "FFTW") {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      rfftw_one(forward,
                reinterpret_cast<fftw_real*>(in.data()),
                reinterpret_cast<fftw_real*>(in.data()));
    }
    virtual void inverse(Array1di &in) const {
      rfftw_one(bckward,
                reinterpret_cast<fftw_real*>(in.data()),
                reinterpret_cast<fftw_real*>(in.data()));
      in *= (1.0/n);
    }
    virtual void free() {
      rfftw_destroy_plan(forward);
      rfftw_destroy_plan(bckward);
    }

  private:

    rfftw_plan forward;
    rfftw_plan bckward;

    virtual void create() {
      int flags = FFTW_MEASURE | FFTW_IN_PLACE;
      if (direct_sign == -1) {
        forward = rfftw_create_plan(n, FFTW_FORWARD , flags);
        bckward = rfftw_create_plan(n, FFTW_BACKWARD, flags);
      } else {
        forward = rfftw_create_plan(n, FFTW_BACKWARD, flags);
        bckward = rfftw_create_plan(n, FFTW_FORWARD , flags);
      }
    }
  };


  template <>
  class IDFT1D<complex, complex> : public InPlace<complex, complex> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<complex,complex>(_n, _direct_sign, "FFTW") {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      fftw_one(forward,
               reinterpret_cast<fftw_complex*>(in.data()),
               reinterpret_cast<fftw_complex*>(in.data()));
    }
    virtual void inverse(Array1di &in) const {
      fftw_one(bckward,
               reinterpret_cast<fftw_complex*>(in.data()),
               reinterpret_cast<fftw_complex*>(in.data()));
      in *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
    }

  private:

    fftw_plan forward;
    fftw_plan bckward;

    virtual void create() {
      int flags = FFTW_MEASURE | FFTW_IN_PLACE;
      if (direct_sign == -1) {
        forward = fftw_create_plan(n, FFTW_FORWARD , flags);
        bckward = fftw_create_plan(n, FFTW_BACKWARD, flags);
      } else {
        forward = fftw_create_plan(n, FFTW_BACKWARD, flags);
        bckward = fftw_create_plan(n, FFTW_FORWARD , flags);
      }
    }
  };


  template <>
  class ODFT1D<double, double> : public OutPlace<double, double> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<double, double>(_n, _direct_sign, "FFTW") {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      rfftw_one(forward,
                const_cast<fftw_real*>
                (reinterpret_cast<const fftw_real*>(in.data())),
                reinterpret_cast<fftw_real*>(out.data()));
    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      rfftw_one(bckward,
                const_cast<fftw_real*>
                (reinterpret_cast<const fftw_real*>(in.data())),
                reinterpret_cast<fftw_real*>(out.data()));
      out *= (1.0/n);
    }
    virtual void free() {
      rfftw_destroy_plan(forward);
      rfftw_destroy_plan(bckward);
    }

  private:

    rfftw_plan forward;
    rfftw_plan bckward;

    virtual void create() {
      int flags = FFTW_MEASURE | FFTW_OUT_OF_PLACE;
      if (direct_sign == -1) {
        forward = rfftw_create_plan(n, FFTW_FORWARD , flags);
        bckward = rfftw_create_plan(n, FFTW_BACKWARD, flags);
      } else {
        forward = rfftw_create_plan(n, FFTW_BACKWARD, flags);
        bckward = rfftw_create_plan(n, FFTW_FORWARD , flags);
      }
    }
  };


  template <>
  class ODFT1D<complex, complex> : public OutPlace<complex, complex> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<complex, complex>(_n, _direct_sign, "FFTW") {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      fftw_one(forward,
               const_cast<fftw_complex*>
               (reinterpret_cast<const fftw_complex*>(in.data())),
               reinterpret_cast<fftw_complex*>(out.data()));
    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      fftw_one(bckward,
               const_cast<fftw_complex*>
               (reinterpret_cast<const fftw_complex*>(in.data())),
               reinterpret_cast<fftw_complex*>(out.data()));
      out *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
    }

  private:

    fftw_plan forward;
    fftw_plan bckward;

    virtual void create() {
      int flags = FFTW_MEASURE | FFTW_OUT_OF_PLACE;
      if (direct_sign == -1) {
        forward = fftw_create_plan(n, FFTW_FORWARD , flags);
        bckward = fftw_create_plan(n, FFTW_BACKWARD, flags);
      } else {
        forward = fftw_create_plan(n, FFTW_BACKWARD, flags);
        bckward = fftw_create_plan(n, FFTW_FORWARD , flags);
      }
    }
  };


}
