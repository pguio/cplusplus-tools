/**************************************************************************
 *
 * $Id: time-utils.h,v 1.20 2011/11/10 14:14:04 patrick Exp $
 *
 * Copyright (c) 2003-2011 Patrick Guio <patrick.guio@gmail.com>
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

#ifndef _TIME_UTILS_H
#define _TIME_UTILS_H

#include <ctime>
#include <sys/times.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>

#if defined(HAVE_MPI)
#include <mpi.h>
#endif

#if defined CLK_TCK
#undef CLK_TCK
#endif
#define CLK_TCK sysconf(_SC_CLK_TCK)

namespace timeutils {

  class Timer {
  public:
    Timer() : tic(), toc(), tbeg(), tend()
#if defined(HAVE_MPI)
      , mpi_tck(MPI_Wtick()), mpi_tbeg(), mpi_tend()
#endif
    {}
    ~Timer() {}
    void wallTime(struct tms *buffer, clock_t &rc) const {
      rc = times(buffer);
    }
    void start() {
      wallTime(&tic, tbeg);
#if defined(HAVE_MPI)
      mpi_tbeg = MPI_Wtime();
#endif
    }
    void stop() {
      wallTime(&toc, tend);
#if defined(HAVE_MPI)
      mpi_tend = MPI_Wtime();
#endif
    }
    double userElapsed() const {
      return static_cast<double>(toc.tms_utime-tic.tms_utime)/CLK_TCK;
    }
    double sysElapsed() const {
      return static_cast<double>(toc.tms_stime-tic.tms_stime)/CLK_TCK;
    }
    double realElapsed() const {
      return static_cast<double>(tend-tbeg)/CLK_TCK;
    }

    double userRunningElapsed() const {
      struct tms now;
      clock_t t;
      wallTime(&now, t);
      return static_cast<double>(now.tms_utime-tic.tms_utime)/CLK_TCK;
    }
    double sysRunningElapsed() const {
      struct tms now;
      clock_t t;
      wallTime(&now, t);
      return static_cast<double>(now.tms_stime-tic.tms_stime)/CLK_TCK;
    }
    double realRunningElapsed() const {
      struct tms now;
      clock_t t;
      wallTime(&now, t);
      return static_cast<double>(t-tbeg)/CLK_TCK;
    }
#if defined(HAVE_MPI)
    double mpiElapsed() {
      return mpi_tend - mpi_tbeg;
    }
    double mpiRunningElapsed() {
      return MPI_Wtime() - mpi_tbeg;
    }
#endif
  private:
    struct tms tic, toc;
    clock_t tbeg, tend;
#if defined(HAVE_MPI)
    double mpi_tck;            // resolution of MPI_Wtime
    double mpi_tbeg, mpi_tend; // seconds
#endif
  };

  struct iterStatus {
    int iter, maxIter;
    iterStatus(int _iter , int _maxIter) : iter(_iter), maxIter(_maxIter) {}
    friend std::ostream& operator<<(std::ostream &os, const iterStatus &x) {
      std::ios::fmtflags f = os.flags() & std::ios::floatfield;
      int p = os.precision();
      return os
             << std::setw(7) << x.iter << "/" << std::setw(7) << x.maxIter
             << " [" << std::fixed << std::setprecision(2) << std::setw(6)
             << (100.0*x.iter)/x.maxIter << " %]"
             << std::resetiosflags(std::ios::fixed)
             << std::setiosflags(f)
             << std::setprecision(p);
    }
  };


  struct toHMS {
    double t;
    explicit toHMS(double _t) : t(_t) {}
    friend std::ostream& operator<<(std::ostream &os, const toHMS &x) {
      unsigned t = static_cast<unsigned>(x.t);
      char charfill = os.fill();
      return os << std::setfill('0')
             << std::setw(2) << t/3600 << ":"
             << std::setw(2) << (t % 3600) / 60 << ":"
             << std::setw(2) << (t % 3600) % 60
             << std::setfill(charfill);
    }
  };

  struct smartTime {
    double t;
    explicit smartTime(double _t) : t(_t) {}
    friend std::ostream& operator<<(std::ostream &os, const smartTime &x) {
      if (x.t < 1.0e-6)
        return os << 1.0e9*x.t << " ns";
      else if (x.t < 1.0e-3)
        return os << 1.0e6*x.t << " us";
      else if (x.t < 1.0)
        return os << 1.0e3*x.t << " ms";
      else
        return os << x.t << " s";
    }
  };

}

#endif // _TIME_UTILS_H
