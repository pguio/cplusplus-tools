/**************************************************************************
 *
 * $Id: classexception.h,v 1.16 2016/06/02 17:11:48 patrick Exp $
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

#ifndef CLASSEXCEPTION_H
#define CLASSEXCEPTION_H

#include <string>
#include <cstring>
#include <stdexcept>

class ClassException : public std::exception {
public:

  ClassException(const std::string &name, const std::string &msg) throw()
    : ClassName(name), Msg(msg)
  {}
  virtual ~ClassException() throw() {}
  virtual const char *what() const throw()
  {
    std::string str(" *** " + ClassName + " exception: " + Msg);
    char * cstr = new char [str.length()+1];
    std::strcpy (cstr, str.c_str());
    return cstr;
  }

private:

  const std::string ClassName;
  const std::string Msg;
};

#endif // CLASSEXCEPTION_H
