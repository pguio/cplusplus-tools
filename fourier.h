/**************************************************************************
 *
 * $Id: fourier.h,v 1.70 2011/03/26 07:05:54 patrick Exp $
 *
 * Copyright (c) 2002-2011 Patrick Guio <patrick.guio@gmail.com>
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

#ifndef FOURIER_H
#define FOURIER_H

#include <iostream>
#include <sstream>
#include <string>
#include <blitz/array.h>
#include <classexception.h>

namespace fourier {

  typedef std::complex<double> complex;

  class AbstractDFT1D {
  public:

    typedef std::string string;
    typedef std::ostream ostream;
    typedef std::ostringstream ostringstream;
    typedef blitz::Range Range;

    friend ostream& operator<<(ostream& os, const AbstractDFT1D &ft1d) {
      ft1d.printOn(os);
      return os;
    }
    AbstractDFT1D(int _n, int _direct_sign, const char *_source_code) :
      n(_n), direct_sign(_direct_sign), source_code(_source_code) {
      if (direct_sign != 1 && direct_sign != -1)
        throw ClassException("AbstractDFT1D", "direct sign is either +1 or -1");
    }

    virtual ~AbstractDFT1D() {}

    virtual void resize(int _n, int _direct_sign=-1) {
      free();
      n = _n;
      direct_sign = _direct_sign;
      create();
    }

  protected:

    int n;
    int direct_sign;
    string source_code;
    virtual void create() = 0;
    virtual void free() = 0;
    virtual void printOn(ostream &os) const {
      std::ios::fmtflags f = os.flags() & std::ios::adjustfield;
      os << std::left << std::setw(6) << source_code
         << std::resetiosflags(std::ios::left) << std::setiosflags(f)
         << ": " << n << "," << std::showpos << direct_sign << std::noshowpos;
    }
  };


  template <class In_numtype, class Out_numtype>
  class InPlace : public AbstractDFT1D {
  public:

    typedef blitz::Array<In_numtype, 1> Array1di;
    typedef blitz::Array<In_numtype, 2> Array2di;

    InPlace(int _n, int _direct_sign, const char *_source_code) :
      AbstractDFT1D(_n, _direct_sign, _source_code) {}

    virtual ~InPlace() {}

    // direct is out_i = \sum_j in_j\exp(direct_sign 2\pi ij\sqrt(-1)/n)
    virtual void direct(Array1di &in) const = 0;

    void direct(Array2di &ins) {
      using blitz::secondDim;
      int ncols = ins.extent(secondDim);
      Array1di in(n);
      for (int i=0; i<ncols; ++i) {
        in = ins(Range::all(),i);
        this->direct(in);
        ins(Range::all(),i) = in;
      }
    }
    // inverse is out_i = \sum_j in_j\exp(-direct_sign 2\pi ij\sqrt(-1)/n)
    virtual void inverse(Array1di &in) const = 0;

    void inverse(Array2di &ins) {
      using blitz::secondDim;
      int ncols = ins.extent(secondDim);
      Array1di in(n);
      for (int i=0; i<ncols; ++i) {
        in = ins(Range::all(),i);
        this->inverse(in);
        ins(Range::all(),i) = in;
      }
    }

  protected:

    virtual void create() = 0;
    virtual void free() = 0;
    virtual void printOn(ostream &os) const {
      AbstractDFT1D::printOn(os);
      if (typeid(In_numtype) == typeid(double) ||
          typeid(In_numtype) == typeid(float))
        os << ",r2" ;
      if (typeid(In_numtype) == typeid(complex))
        os << ",c2" ;
      if (typeid(Out_numtype) == typeid(double) ||
          typeid(Out_numtype) == typeid(float))
        os << "r,i" ;
      if (typeid(Out_numtype) == typeid(complex))
        os << "c,i" ;
    }
  };

  template <class In_numtype, class Out_numtype>
  class OutPlace : public AbstractDFT1D {
  public:

    typedef blitz::Array<In_numtype, 1> Array1di;
    typedef blitz::Array<In_numtype, 2> Array2di;
    typedef blitz::Array<Out_numtype, 1> Array1do;
    typedef blitz::Array<Out_numtype, 2> Array2do;

    OutPlace(int _n, int _direct_sign, const char *_source_code) :
      AbstractDFT1D(_n, _direct_sign, _source_code) {}

    virtual ~OutPlace() {}

    // direct is out_i = \sum_j in_j\exp(direct_sign 2\pi ij\sqrt(-1)/n)
    virtual void direct(const Array1di &in, Array1do &out) const = 0;

    void direct(const Array2di &ins, Array2do &outs) {
      using blitz::secondDim;
      int ncols = ins.extent(secondDim);
      Array1di in(n);
      Array1do out(n);
      for (int i=0; i<ncols; ++i) {
        in = ins(Range::all(),i);
        this->direct(in, out);
        outs(Range::all(),i) = out;
      }
    }
    // inverse is out_i = \sum_j in_j\exp(-direct_sign 2\pi ij\sqrt(-1)/n)
    virtual void inverse(const Array1do &in, Array1di &out) const = 0;

    void inverse(const Array2do &ins, Array2di &outs) {
      using blitz::secondDim;
      int ncols = ins.extent(secondDim);
      Array1do in(n);
      Array1di out(n);
      for (int i=0; i<ncols; ++i) {
        in = ins(Range::all(),i);
        this->inverse(in, out);
        outs(Range::all(),i) = out;
      }
    }

  protected:

    virtual void create() = 0;
    virtual void free() = 0;
    virtual void printOn(ostream &os) const {
      AbstractDFT1D::printOn(os);
      if (typeid(In_numtype) == typeid(double) ||
          typeid(In_numtype) == typeid(float))
        os << ",r2" ;
      if (typeid(In_numtype) == typeid(complex))
        os << ",c2" ;
      if (typeid(Out_numtype) == typeid(double) ||
          typeid(Out_numtype) == typeid(float))
        os << "r,o" ;
      if (typeid(Out_numtype) == typeid(complex))
        os << "c,o" ;
    }
  };

  template <class In_numtype, class Out_numtype>
  class ODFT1D;

  template <class In_numtype, class Out_numtype>
  class IDFT1D;

  template<class T_numtype>
  blitz::Array<T_numtype,1> fftshift(const blitz::Array<T_numtype,1> f)
  {
    using blitz::Range;
    // Shift DC component to center
    int n = f.rows();
    blitz::Array<T_numtype,1> f_shifted(n);
    // ceil(n/2)
    int n2 = (n+1) >> 1;
    f_shifted(Range(n2,n-1)-n2)   = f(Range(n2,n-1));
    f_shifted(Range(0,n2-1)+n-n2) = f(Range(0,n2-1));
    return f_shifted;
  }

  template<class T_numtype>
  blitz::Array<T_numtype,1> ifftshift(const blitz::Array<T_numtype,1> f)
  {
    using blitz::Range;
    // Inverse fftshift
    int n = f.rows();
    blitz::Array<T_numtype,1> f_ishifted(n);
    // floor(n/2)
    int n2 = n >> 1;
    f_ishifted(Range(n2,n-1)-n2)   = f(Range(n2,n-1));
    f_ishifted(Range(0,n2-1)+n-n2) = f(Range(0,n2-1));
    return f_ishifted;
  }


  template<class T_numtype>
  blitz::Array<T_numtype,1> binom2filter(const blitz::Array<T_numtype,1> &f,
                                         const T_numtype &zero)
  {
    int n = f.rows();
    blitz::Array<T_numtype,1> result(n);
    blitz::TinyVector<double,3> filter(1./3., 2./3., 1./3.);
    for (int i=0; i<n; ++i) {
      int jl = i-1;
      if (jl < 0)
        jl = 0;
      int jh = i+1;
      if (jh >= n)
        jh = n-1;
      T_numtype temp = zero;
      for (int j=jl; j <= jh; ++j)
        temp += f(j) * filter(i-j+1);
      result(i) = temp;
    }
    return result;
  }

  template<class T_numtype>
  blitz::Array<T_numtype,1> binom4filter(const blitz::Array<T_numtype,1> &f,
                                         const T_numtype &zero)
  {
    int n = f.rows();
    blitz::Array<T_numtype,1> result(n);
    blitz::TinyVector<double,5> filter(1./16., 4./16., 6./16., 4./16., 1./16.);
    for (int i=0; i<n; ++i) {
      int jl = i-2;
      if (jl < 0)
        jl = 0;
      int jh = i+2;
      if (jh >= n)
        jh = n-1;
      T_numtype temp = zero;
      for (int j=jl; j <= jh; ++j)
        temp += f(j) * filter(i-j+2);
      result(i) = temp;
    }
    return result;
  }

  template<class T_numtype>
  blitz::Array<T_numtype,1> binom6filter(const blitz::Array<T_numtype,1> &f,
                                         const T_numtype &zero)
  {
    int n = f.rows();
    blitz::Array<T_numtype,1> result(n);
    blitz::TinyVector<double,7> filter(1./64., 6./64., 15./64., 20./64.,
                                       15./64., 6./64., 1./64.);
    for (int i=0; i<n; ++i) {
      int jl = i-3;
      if (jl < 0)
        jl = 0;
      int jh = i+3;
      if (jh >= n)
        jh = n-1;
      T_numtype temp = zero;
      for (int j=jl; j <= jh; ++j)
        temp += f(j) * filter(i-j+3);
      result(i) = temp;
    }
    return result;
  }

  template<class T_numtype>
  blitz::Array<T_numtype,1> binom8filter(const blitz::Array<T_numtype,1> &f,
                                         const T_numtype &zero)
  {
    int n = f.rows();
    blitz::Array<T_numtype,1> result(n);
    blitz::TinyVector<double,9> filter(1./256., 8./256., 28./256., 56./256.,
                                       70./256., 56./256., 28./256., 8./256.,
                                       1./256.);
    for (int i=0; i<n; ++i) {
      int jl = i-4;
      if (jl < 0)
        jl = 0;
      int jh = i+4;
      if (jh >= n)
        jh = n-1;
      T_numtype temp = zero;
      for (int j=jl; j <= jh; ++j)
        temp += f(j) * filter(i-j+4);
      result(i) = temp;
    }
    return result;
  }

}

#if defined(HAVE_FFTW_FFT)
#include <fourier-fftw.h>
#elif defined(HAVE_FFTW3_FFT)
#include <fourier-fftw3.h>
#elif defined(HAVE_DXML_FFT)
#include <fourier-dxml.h>
#elif defined(HAVE_MLIB_FFT)
#include <fourier-mlib.h>
#else
#error in <fourier.h>: no fast fourier transform implementation specified!
#endif


#endif // FOURIER_H
