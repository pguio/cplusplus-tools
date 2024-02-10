/**************************************************************************
 *
 * $Id: fourier-fftw3.h,v 1.13 2009/06/29 16:19:36 patrick Exp $
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

#include <fftw3.h>

/*

For more informations about Planner Flags have a look at the following online
documentation:

http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags
http://www.fftw.org/faq/section3.html#nondeterministic
http://www.fftw.org/fftw3_doc/Wisdom.html#Wisdom

*/

namespace fourier {

  const unsigned defaultPlanFlag = FFTW_ESTIMATE;
  const unsigned betterPlanFlag  = FFTW_MEASURE;
  const unsigned optimalPlanFlag = FFTW_PATIENT;

  template <>
  class IDFT1D<double, double> : public InPlace<double, double> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<double, double>(_n, _direct_sign, "FFTW3"), planFlags(defaultPlanFlag) {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(forward);
      memcpy(in.data(), inFftw, blckSize);
    }
    virtual void inverse(Array1di &in) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(bckward);
      memcpy(in.data(), inFftw, blckSize);
      in *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
      fftw_free(inFftw);
    }

  private:

    fftw_plan restrict forward;
    fftw_plan restrict bckward;
    double * restrict inFftw;
    size_t blckSize;
    unsigned planFlags;

    virtual void create() {
      fftw_import_system_wisdom();
      inFftw  = (double *)fftw_malloc(n*sizeof(double));
      blckSize = n*sizeof(double);
      if (direct_sign == -1) {
        forward = fftw_plan_r2r_1d(n, inFftw, inFftw, FFTW_R2HC, planFlags);
        bckward = fftw_plan_r2r_1d(n, inFftw, inFftw, FFTW_HC2R, planFlags);
      } else {
        forward = fftw_plan_r2r_1d(n, inFftw, inFftw, FFTW_R2HC, planFlags);
        bckward = fftw_plan_r2r_1d(n, inFftw, inFftw, FFTW_HC2R, planFlags);
      }
    }
  };

  template <>
  class IDFT1D<double, complex> : public InPlace<double, complex> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<double, complex>(_n, _direct_sign, "FFTW3"), planFlags(defaultPlanFlag) {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(forward);
      memcpy(in.data(), inFftw, blckSize);
    }
    virtual void inverse(Array1di &in) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(bckward);
      memcpy(in.data(), inFftw, blckSize);
      in *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
      fftw_free(inFftw);
    }

  private:

    fftw_plan restrict forward;
    fftw_plan restrict bckward;
    double * restrict inFftw;
    size_t blckSize;
    unsigned planFlags;

    virtual void create() {
      fftw_import_system_wisdom();
      inFftw  = (double *)fftw_malloc(2*(n/2+1)*sizeof(double));
      blckSize = n*sizeof(double);
      if (direct_sign == -1) {
        forward = fftw_plan_dft_r2c_1d(n, inFftw,
                                       reinterpret_cast<fftw_complex*>(inFftw),
                                       planFlags);
        bckward = fftw_plan_dft_c2r_1d(n,
                                       reinterpret_cast<fftw_complex*>(inFftw),
                                       inFftw, planFlags);
      } else {
        forward = fftw_plan_dft_r2c_1d(n, inFftw,
                                       reinterpret_cast<fftw_complex*>(inFftw),
                                       planFlags);
        bckward = fftw_plan_dft_c2r_1d(n,
                                       reinterpret_cast<fftw_complex*>(inFftw),
                                       inFftw, planFlags);
      }
    }
  };

  template <>
  class IDFT1D<complex, complex> : public InPlace<complex, complex> {
  public:

    IDFT1D(int _n=1, int _direct_sign=-1) :
      InPlace<complex, complex>(_n, _direct_sign, "FFTW3"), planFlags(defaultPlanFlag) {
      create();
    }
    virtual ~IDFT1D()
    {}
    virtual void direct(Array1di &in) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(forward);
      memcpy(in.data(), inFftw, blckSize);
    }
    virtual void inverse(Array1di &in) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(bckward);
      memcpy(in.data(), inFftw, blckSize);
      in *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
      fftw_free(inFftw);
    }

  private:

    fftw_plan restrict forward;
    fftw_plan restrict bckward;
    fftw_complex * restrict inFftw;
    size_t blckSize;
    unsigned planFlags;

    virtual void create() {
      fftw_import_system_wisdom();
      inFftw  = (fftw_complex *)fftw_malloc(n*sizeof(fftw_complex));
      blckSize = n*sizeof(fftw_complex);
      if (direct_sign == -1) {
        forward = fftw_plan_dft_1d(n, inFftw, inFftw, FFTW_FORWARD , planFlags);
        bckward = fftw_plan_dft_1d(n, inFftw, inFftw, FFTW_BACKWARD, planFlags);
      } else {
        forward = fftw_plan_dft_1d(n, inFftw, inFftw, FFTW_BACKWARD, planFlags);
        bckward = fftw_plan_dft_1d(n, inFftw, inFftw, FFTW_FORWARD , planFlags);
      }
    }
  };


  template <>
  class ODFT1D<double, double> : public OutPlace<double, double> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<double, double>(_n, _direct_sign, "FFTW3"), planFlags(defaultPlanFlag) {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(forward);
      memcpy(out.data(), outFftw, blckSize);
    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(bckward);
      memcpy(out.data(), outFftw, blckSize);
      out *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
      fftw_free(inFftw);
      fftw_free(outFftw);
    }

  private:

    fftw_plan restrict forward;
    fftw_plan restrict bckward;
    double * restrict inFftw;
    double * restrict outFftw;
    size_t blckSize;
    unsigned planFlags;

    virtual void create() {
      fftw_import_system_wisdom();
      inFftw  = (double *)fftw_malloc(n*sizeof(double));
      outFftw = (double *)fftw_malloc(n*sizeof(double));
      blckSize = n*sizeof(double);
      if (direct_sign == -1) {
        forward = fftw_plan_r2r_1d(n, inFftw, outFftw, FFTW_R2HC, planFlags);
        bckward = fftw_plan_r2r_1d(n, inFftw, outFftw, FFTW_HC2R, planFlags);
      } else {
        forward = fftw_plan_r2r_1d(n, inFftw, outFftw, FFTW_HC2R, planFlags);
        bckward = fftw_plan_r2r_1d(n, inFftw, outFftw, FFTW_R2HC, planFlags);
      }
    }
  };

  template <>
  class ODFT1D<double, complex> : public OutPlace<double, complex> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<double, complex>(_n, _direct_sign, "FFTW3"), planFlags(defaultPlanFlag) {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(forward);
      memcpy(out.data(), outFftw, blckSize);
    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      memcpy(outFftw, in.data(), blckSize);
      fftw_execute(bckward);
      memcpy(out.data(), inFftw, blckSize);
      out *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
      fftw_free(inFftw);
      fftw_free(outFftw);
    }

  private:

    fftw_plan restrict forward;
    fftw_plan restrict bckward;
    double * restrict inFftw;
    fftw_complex *outFftw;
    size_t blckSize;
    unsigned planFlags;

    virtual void create() {
      fftw_import_system_wisdom();
      inFftw  = (double *)fftw_malloc(n*sizeof(double));
      outFftw = (fftw_complex *)fftw_malloc((n/2+1)*sizeof(fftw_complex));
      blckSize = n*sizeof(double);
      if (direct_sign == -1) {
        forward = fftw_plan_dft_r2c_1d(n, inFftw, outFftw, planFlags);
        bckward = fftw_plan_dft_c2r_1d(n, outFftw, inFftw, planFlags);
      } else {
        forward = fftw_plan_dft_r2c_1d(n, inFftw, outFftw, planFlags);
        bckward = fftw_plan_dft_c2r_1d(n, outFftw, inFftw, planFlags);
      }
    }
  };

  template <>
  class ODFT1D<complex, complex> : public OutPlace<complex, complex> {
  public:

    ODFT1D(int _n=1, int _direct_sign=-1) :
      OutPlace<complex, complex>(_n, _direct_sign, "FFTW3"), planFlags(defaultPlanFlag) {
      create();
    }
    virtual ~ODFT1D()
    {}
    virtual void direct(const Array1di &in, Array1do &out) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(forward);
      memcpy(out.data(), outFftw, blckSize);
    }
    virtual void inverse(const Array1do &in, Array1di &out) const {
      memcpy(inFftw, in.data(), blckSize);
      fftw_execute(bckward);
      memcpy(out.data(), outFftw, blckSize);
      out *= (1.0/n);
    }
    virtual void free() {
      fftw_destroy_plan(forward);
      fftw_destroy_plan(bckward);
      fftw_free(inFftw);
      fftw_free(outFftw);
    }
    void setPlanFlag(unsigned _flags) {
      planFlags = _flags;
      free();
      create();
    }

  private:

    fftw_plan restrict forward;
    fftw_plan restrict bckward;
    fftw_complex * restrict inFftw;
    fftw_complex * restrict outFftw;
    size_t blckSize;
    unsigned planFlags;

    virtual void create() {
      fftw_import_system_wisdom();
      inFftw  = (fftw_complex *)fftw_malloc(n*sizeof(fftw_complex));
      outFftw = (fftw_complex *)fftw_malloc(n*sizeof(fftw_complex));
      blckSize = n*sizeof(fftw_complex);
      if (direct_sign == -1) {
        forward = fftw_plan_dft_1d(n, inFftw, outFftw, FFTW_FORWARD , planFlags);
        bckward = fftw_plan_dft_1d(n, inFftw, outFftw, FFTW_BACKWARD, planFlags);
      } else {
        forward = fftw_plan_dft_1d(n, inFftw, outFftw, FFTW_BACKWARD, planFlags);
        bckward = fftw_plan_dft_1d(n, inFftw, outFftw, FFTW_FORWARD , planFlags);
      }
    }
  };

}
