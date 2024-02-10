/**************************************************************************
 *
 * $Id: fourier-dxml.h,v 1.13 2011/03/26 07:05:54 patrick Exp $
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

#include <dxmldef.h>

namespace fourier {

  template <>
  class IDFT1D<double, double> : public InPlace<double, double> {
  public:
    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<double, double>(_n, _direct_sign, "DXML") {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      int status = dfft_apply_("r", "r", const_cast<char*>(forward),
                               in.data(), in.data(),
                               const_cast<DXML_D_FFT_STRUCTURE*>(&fft_struct),
                               const_cast<int*>(&stride));
      if (direct_sign == 1)
        in *= static_cast<double>(n);
#if 0

      if (status) {
        ostringstream os;
        os << "dfft_apply returned status = " << status;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void inverse(Array1di &in) const {
      int status = dfft_apply_("r", "r", const_cast<char*>(bckward),
                               in.data(), in.data(),
                               const_cast<DXML_D_FFT_STRUCTURE*>(&fft_struct),
                               const_cast<int*>(&stride));
      if (direct_sign == 1)
        in *= (1.0/n);
#if 0

      if (status) {
        ostringstream os;
        os << "dfft_apply returned status = " << status;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      dfft_exit_(&fft_struct);
    }

  private:

    char forward[2];
    char bckward[2];
    int stride;
    DXML_D_FFT_STRUCTURE fft_struct;

    virtual void create() {
      if (direct_sign == -1) {
        forward[0] = 'f';
        bckward[0] = 'b';
      } else {
        forward[0] = 'b';
        bckward[0] = 'f';
      }
      forward[1] = '\0';
      bckward[1] = '\0';
      stride = 1;
      int status = dfft_init_(&n, &fft_struct, &stride);
      if (status) {
        ostringstream os;
        os << "dfft_init returned status = " << status;
        throw ClassException("FT1d", os.str());
      }
    }
  };

  template <>
  class IDFT1D<complex, complex> : public InPlace<complex, complex> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<complex, complex>(_n, _direct_sign, "DXML") {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      int status = zfft_apply_("c", "c", const_cast<char*>(forward),
                               in.data(), in.data(), &fft_struct, &stride);
      if (direct_sign == 1)
        in *= static_cast<double>(n);
#if 0

      if (status) {
        ostringstream os;
        os << "zfft_apply returned status = " << status;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void inverse(Array1di &in) const {
      int status = zfft_apply_("c", "c", const_cast<char*>(bckward),
                               in.data(), in.data(), &fft_struct, &stride);
      if (direct_sign == 1)
        in *= (1.0/n);
#if 0

      if (status) {
        ostringstream os;
        os << "zfft_apply returned status = " << status;
        throw ClassException("IDFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      zfft_exit_(&fft_struct);
    }

  private:

    char forward[2];
    char bckward[2];
    int stride;
    DXML_Z_FFT_STRUCTURE fft_struct;

    virtual void create() {
      if (direct_sign == -1) {
        forward[0] = 'f';
        bckward[0] = 'b';
      } else {
        forward[0] = 'b';
        bckward[0] = 'f';
      }
      forward[1] = '\0';
      bckward[1] = '\0';
      stride = 1;
      int status = zfft_init_(&n, &fft_struct, &stride);
      if (status) {
        ostringstream os;
        os << "zfft_init returned status = " << status;
        throw ClassException("FT1d", os.str());
      }
    }
  };

  template <>
  class ODFT1D<double, double> : public OutPlace<double, double> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<double, double>(_n, _direct_sign, "DXML") {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      int status = dfft_apply_("r", "r", const_cast<char*>(forward),
                               const_cast<double*>(in.data()), out.data(),
                               const_cast<DXML_D_FFT_STRUCTURE*>(&fft_struct),
                               const_cast<int*>(&stride));
      if (direct_sign == 1)
        out *= static_cast<double>(n);
#if 0

      if (status) {
        ostringstream os;
        os << "dfft_apply returned status = " << status;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      int status = dfft_apply_("r", "r", const_cast<char*>(bckward),
                               const_cast<double*>(in.data()), out.data(),
                               const_cast<DXML_D_FFT_STRUCTURE*>(&fft_struct),
                               const_cast<int*>(&stride));
      if (direct_sign == 1)
        out *= (1.0/n);
#if 0

      if (status) {
        ostringstream os;
        os << "dfft_apply returned status = " << status;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      dfft_exit_(&fft_struct);
    }

  private:

    char forward[2];
    char bckward[2];
    int stride;
    DXML_D_FFT_STRUCTURE fft_struct;

    virtual void create() {
      if (direct_sign == -1) {
        forward[0] = 'f';
        bckward[0] = 'b';
      } else {
        forward[0] = 'b';
        bckward[0] = 'f';
      }
      forward[1] = '\0';
      bckward[1] = '\0';
      stride = 1;
      int status = dfft_init_(&n, &fft_struct, &stride);
      if (status) {
        ostringstream os;
        os << "dfft_init returned status = " << status;
        throw ClassException("FT1d", os.str());
      }
    }
  };

  template <>
  class ODFT1D<complex, complex> : public OutPlace<complex, complex> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<complex, complex>(_n, _direct_sign, "DXML") {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      int status = zfft_apply_("c", "c", const_cast<char*>(forward),
                               in.data(), out.data(), &fft_struct, &stride);
      if (direct_sign == 1)
        out *= static_cast<double>(n);
#if 0

      if (status) {
        ostringstream os;
        os << "zfft_apply returned status = " << status;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      int status = zfft_apply_("c", "c", const_cast<char*>(bckward),
                               in.data(), out.data(), &fft_struct, &stride);
      if (direct_sign == 1)
        out *= (1.0/n);
#if 0

      if (status) {
        ostringstream os;
        os << "zfft_apply returned status = " << status;
        throw ClassException("ODFT1D", os.str());
      }
#endif

    }
    virtual void free() {
      zfft_exit_(&fft_struct);
    }

  private:

    char forward[2];
    char bckward[2];
    int stride;
    DXML_Z_FFT_STRUCTURE fft_struct;

    virtual void create() {
      if (direct_sign == -1) {
        forward[0] = 'f';
        bckward[0] = 'b';
      } else {
        forward[0] = 'b';
        bckward[0] = 'f';
      }
      forward[1] = '\0';
      bckward[1] = '\0';
      stride = 1;
      int status = zfft_init_(&n, &fft_struct, &stride);
      if (status) {
        ostringstream os;
        os << "zfft_init returned status = " << status;
        throw ClassException("ODFT1D", os.str());
      }
    }
  };

}
