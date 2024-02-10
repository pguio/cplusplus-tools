
/**************************************************************************
 *
 * $Id: any.h,v 1.16 2011/04/05 19:24:44 patrick Exp $
 *
 * Copyright (c) 2001-2011 Patrick Guio <patrick.guio@gmail.com>
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

#ifndef ANY_H
#define ANY_H

#include <algorithm>
#include <typeinfo>
#include <iostream>


#if defined(HAVE_ANY_NAMESPACE)
namespace any {
#endif

  class Any {
  public: // structors

    Any() : content(0)
    {}
    template<typename ValueType>
    Any(const ValueType & value) : content(new holder<ValueType>(value))
    {}
    Any(const Any & other) : content(other.content ? other.content->clone() : 0)
    {}
    ~Any() {
      delete content;
    }

  public: // modifiers

    Any & swap(Any & rhs) {
      std::swap(content, rhs.content);
      return *this;
    }
    template<typename ValueType>
    Any & operator=(const ValueType & rhs) {
      Any(rhs).swap(*this);
      return *this;
    }
#if 1
    Any & operator=(const Any & rhs) {
      Any(rhs).swap(*this);
      return *this;
    }
#else
    Any& operator=(const Any& value) {
      content = (value.content ? value.content->clone() : 0);
      return *this;
    }
#endif

  public: // queries

    bool empty() const {
      return !content;
    }
    const std::type_info & type() const {
      return content ? content->type() : typeid(void);
    }
    friend std::ostream& operator << (std::ostream& os, const Any& value) {
      value.content->printOn(os);
      return os;
    }

  public: // types (public so any_cast can be non-friend)

    class placeholder {
    public: // structors

      virtual ~placeholder()
      {}

    public: // queries
      virtual const std::type_info & type() const = 0;
      virtual placeholder * clone() const = 0;
      virtual void printOn(std::ostream&) const = 0;
    };
    template<typename ValueType>
    class holder : public placeholder {
    public: // structors

      explicit holder(const ValueType & value) : held(value)
      {}

    public: // queries

      virtual const std::type_info & type() const {
        return typeid(ValueType);
      }
      virtual placeholder * clone() const {
        return new holder(held);
      }
      virtual void printOn(std::ostream& os) const {
        os << held;
      }
    public: // representation

      ValueType held;
    };

  public:
    placeholder * content;

  };

  class bad_any_cast : public std::bad_cast {
  public:
    virtual const char * what() const throw() {
      return "bad_any_cast: "
             "failed conversion using any_cast";
    }
  };

  template<typename ValueType>
  ValueType * any_cast(Any * operand)
  {
    return operand && operand->type() == typeid(ValueType)
           ? &static_cast<Any::holder<ValueType> *>(operand->content)->held
           : 0;
  }

  template<typename ValueType>
  const ValueType * any_cast(const Any * operand)
  {
    return any_cast<ValueType>(const_cast<Any *>(operand));
  }

  template<typename ValueType>
  ValueType any_cast(const Any & operand)
  {
    const ValueType * result = any_cast<ValueType>(&operand);
    if(!result)
      throw bad_any_cast();
    return *result;
  }

#if defined(HAVE_ANY_NAMESPACE)
}
#endif

#endif // ANY_H
