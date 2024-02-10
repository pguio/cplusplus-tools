/**************************************************************************
 *
 * $Id: bench.h,v 1.19 2017/10/23 16:20:31 patrick Exp $
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


#ifndef BENCH_H
#define BENCH_H

#include <time-utils.h>

#include <sstream>
#include <string>

namespace bench {

#if 1

  static bool verbose = false;

  struct Test {
    virtual ~Test()
    {}
    virtual void op() = 0;
    virtual std::string opName() const = 0;
  };

  template <class Test>
  double  benchClassOp(Test & test, double minTime, int repeatTime=1)
  {
    timeutils::Timer timer;
    double avOneOpTime=0;

    for (int r=0; r<repeatTime; ++r) {
      double oneOpTime;
      double totalTime;
      int ni = 1;
      do {
        timer.start();
        for (int i = 0; i < ni; ++i)
          test.op();
        timer.stop();
        oneOpTime = (totalTime=(timer.userElapsed())) / ni;
        ni <<= 1;
      } while (totalTime < minTime);
      avOneOpTime += oneOpTime;
    }
    avOneOpTime /= double(repeatTime);

    if (verbose)
      std::cout << "time for one '" << test.opName() << "' = "
                << timeutils::smartTime(avOneOpTime) << std::endl;
    return avOneOpTime;
  }

#define BENCH(block,name,mintime,t)                                  \
{                                                                    \
	timeutils::Timer timer;                                            \
	double total_t;                                                    \
	int ni = 1;                                                        \
	do {                                                               \
		timer.start();                                                   \
		for (int i = 0; i < ni; ++i) {                                   \
			block                                                          \
	}                                                                  \
	timer.stop();                                                      \
	t = (total_t=(timer.userElapsed())) / ni;                          \
	ni <<= 1;                                                          \
	} while (total_t < mintime);                                       \
	if (bench::verbose)                                                \
	  std::cout << "time for one " << name << " = "                    \
		        << timeutils::smartTime(t) << std::endl;                 \
}                                                                    \
 
#else

#include <cycle.h>

#define BENCH(block,name,mintime,t)                                  \
{                                                                    \
	ticks t0, t1;                                                      \
	double total_t;                                                    \
	int ni = 1;                                                        \
	do {                                                               \
	  t0 = getticks();                                                 \
		for (int i = 0; i < ni; ++i) {                                   \
			block                                                          \
	}                                                                  \
	t1 = getticks();                                                   \
	t = (total_t=(elapsed(t1,t0))) / ni;                               \
	ni <<= 1;                                                          \
	} while (total_t < mintime);                                       \
	std::cout << "time for one " << name << " = "                      \
		        << t << std::endl;                                       \
}                                                                    \
 
#endif

}

#endif
