/**************************************************************************
 *
 * $Id: parser.h,v 1.53 2011/04/05 20:45:44 patrick Exp $
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

#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <any.h>

#if defined(HAVE_MPI)
#include <mpi.h>
const int masterProc = 0;
const int tagProc    = 100;
const int mpiRoot    = 0;
#endif

#include <classexception.h>


namespace parser {

  class ParserException : public ClassException {
  public:
    explicit ParserException(std::string s) throw() :
      ClassException("parser::Parser", s)
    {}
    virtual ~ParserException() throw()
    {}
  }
  ;
#if 0

  class BadConversion : public std::runtime_error {
  public:
    BadConversion(const std::string& s) : std::runtime_error(s)
    {}
    virtual ~BadConversion() throw()
    {}
  }
  ;
#else

  class BadConversion {
  public:
    explicit BadConversion(std::string s) {
      throw ParserException("Cannot convert '" + s + "' to specified type\n");
    }
    virtual ~BadConversion() throw()
    {}
  }
  ;
#endif

  namespace types {
    const static int none       = 0;
    const static int boolean    = 1;
    const static int character  = 2;
    const static int integer    = 3;
    const static int real       = 4;
    const static int charStr    = 5;
    const static int boolVect   = 6;
    const static int charVect   = 7;
    const static int intVect    = 8;
    const static int realVect   = 9;
    const static int stringVect = 10;
  }


  class Parser {
  public:
    enum tabulator { tab1=30, tab2=45, tab3=80, last_tab=tab3 };
    enum parseMode { inpFile, cmdLine, inpFile_cmdLine, cmdLine_inpFile };

    typedef std::string string;
    typedef std::ostringstream ostringstream;
    typedef std::istringstream istringstream;
    typedef std::ifstream ifstream;

#if defined(HAVE_ANY_NAMESPACE)

    typedef any::Any Any;
#endif

    typedef string::iterator stringIter;
    typedef string::size_type stringSizeType;

    typedef std::list<string> ArgList;
    typedef ArgList::const_iterator ArgListConstIter;

    typedef int keyType;
    typedef int valueType;

    typedef std::map<keyType, valueType> OptionValueList;
    typedef OptionValueList::value_type OptionValuePair;

    typedef std::map<keyType, string> OptionDescList;
    typedef OptionDescList::value_type OptionDescPair;

    typedef std::multimap<keyType, const string> OptionList;
    typedef OptionList::const_iterator OptionListConstIter;
    typedef OptionList::value_type OptionPair;

    typedef std::map<keyType, Any> ValueList;
    typedef ValueList::const_iterator ValueListConstIter;
    typedef ValueList::value_type ValuePair;

    typedef std::map<valueType, const string> ValuesDescList;
    typedef ValuesDescList::value_type ValueDescPair;

    typedef std::map<int, string> LUT;
    typedef LUT::value_type LUTPair;
    typedef LUT::const_iterator LUTConstIter;

    explicit Parser(int nargs=0, char* args []=0);
    virtual ~Parser();

    void printCmd(std::ostream &os) const;
    void viewArgs(std::ostream &os) const;

    void registerClass(const string &name);
    void registerProgram(const string &name);
    void registerPackage(const string &package, const string &version,
                         const string &copyright);

    void setPrefix(const string name);

    void insertOption(keyType key, const string name, valueType type,
                      const string desc, const Any &defval);
    void insertOptionAlias(keyType key, const string alias);

    void parseLevelDebugOption(const string name);
    int debugLevel() const;

    virtual bool parseHelp() const;
    virtual bool parseVersion() const;
    virtual bool parseTemplate() const;

    bool parseCmdLine(keyType key) const;
    bool parseInpFile(keyType key) const;

    bool parseOption(keyType key, parseMode mode = inpFile_cmdLine) const;

    template<class T>
    bool parseCmdLine(keyType key, T &value) const;
    template<class T>
    bool parseInpFile(keyType key, T &value) const;

    template<class T>
    bool parseOption(keyType key, T &value, parseMode mode = inpFile_cmdLine) const;

    template<class T>
    bool parseOptions(keyType key, std::vector<T> &value) const;

  protected:

#if defined(HAVE_MPI)

    int nbProc;
    int rankProc;
#endif

    void checkMap(keyType key, const LUT &_map, int value) const;
#ifdef BZ_TINYVEC_H

    template<int N>
    void checkMap(keyType key, const LUT &_map,
                  const  blitz::TinyVector<int,N> &values) const;
#endif

  private:

    enum DefaultOptions {
      _help=-10, _version, _template, _file, _last=_file
    };

    string Cmd;
    ArgList Args;
    OptionList Options;
    ValueList OptionsDefaultValue;
    OptionValueList OptionType;
    OptionDescList OptionDesc;

    static ValuesDescList TypeValues;

    string className;
    string packageName;
    string numVersion;
    string copyrightText;
    string progName;
    string prefixName;

    bool debugParser;
    int classDebugLevel;

    std::string inputFileName;
    bool inputFileNameParsed;

    static void formatString(string &str, unsigned tabend);
    static void formatString(string &str, unsigned tab1, unsigned tab2);

    void viewOptions(std::ostream &os) const;
    void viewTemplateOptions(string &options_templates) const;

    bool isKeyDefined(keyType key) const;

    template<typename T>
    static void convert(const string &str, T &x, bool failIfLeftoverChars = true);

    template<class T>
    void convert(const string &str, std::vector<T> &val) const;

#ifdef BZ_TINYVEC_H

    template<class T,int N>
    void convert(const string &str,  blitz::TinyVector<T,N> &val) const;
#endif
#ifdef BZ_ARRAY_H

    template<class T>
    void convert(const string &str,  blitz::Array<T,1> &val) const;
#endif

  };

  template<class T>
  void  Parser::convert(const string &str, T &x, bool failIfLeftoverChars)
  {
    istringstream is(str);
    char c;
    if (!(is >> x) || (failIfLeftoverChars && is.get(c)))
      throw BadConversion(str);
  }

  template<>
  void Parser::convert(const string &str, bool &value, bool failIfLeftoverChars);

  template<>
  void Parser::convert(const string &str, char* &value, bool failIfLeftoverChars);

  template<>
  void Parser::convert(const string &str, float &value, bool failIfLeftoverChars);

  template<>
  void Parser::convert(const string &str, double &value, bool failIfLeftoverChars);

  template<class T>
  bool Parser::parseOption(keyType key, T &value, parseMode mode) const
  {
    try {

      bool parsed = false;
      switch (mode) {
      case inpFile:
        parsed = parseInpFile(key, value);
        break;
      case cmdLine:
        parsed = parseCmdLine(key, value);
        break;
      case inpFile_cmdLine:
        parsed = parseInpFile(key, value) | parseCmdLine(key, value);
        break;
      case cmdLine_inpFile:
        parsed = parseCmdLine(key, value) | parseInpFile(key, value);
        break;
      }
      return parsed;

    } catch (ParserException &e) {
      OptionListConstIter O(Options.find(key));
      string msg("In option " + O->second);
      for (++O; O!=Options.upper_bound(key); ++O) {
        msg = msg + ", " + O->second;
      }
      msg.append("\n" + string(e.what()));
      throw ParserException(msg);
    }
  }

#ifdef BZ_TINYVEC_H
  template<int N>
  void Parser::checkMap(keyType key, const LUT &_map,
                        const blitz::TinyVector<int,N> &values) const
  {
    for (int d=0; d<N; ++d)
      checkMap(key, _map, values(d));
  }
#endif

  template<class T>
  bool Parser::parseCmdLine(keyType key, T &value) const
  {
    if (! isKeyDefined(key))
      return false;

    ArgListConstIter A;
    OptionListConstIter O;
    for (A=Args.begin(); A!=Args.end(); ++A) {
      for (O=Options.find(key); O!=Options.upper_bound(key); ++O) {
        if (*A == O->second && *A != Args.back()) {
          convert(*(++A), value);
          return true;
        }
      }
    }
    return false;
  }

  template<class T>
  bool Parser::parseInpFile(keyType key, T &value) const
  {
    using std::getline;
    if (! isKeyDefined(key) || ! inputFileNameParsed)
      return false;

    // there is no "nocreate" in StdC++ :(
    // ios::in is default
    ifstream fid(inputFileName.c_str());
    if (! fid)
      throw ParserException("Cannot open file '" + inputFileName + "'");

    // automatic memory management, also doesn't overflow
    string line;
    OptionListConstIter O;
    // fixes the usual eof() bug
    while (getline(fid, line)) {
      if (line[0]!='#' && line[0]!='%') {
        for (O=Options.find(key); O!=Options.upper_bound(key); ++O) {
          string opt1(O->second + "=");
          string opt2(O->second + " ");
          if (line.find(opt1) == 0 || line.find(opt2) == 0) {
            line.erase(0, opt1.size());
            convert(line, value);
            fid.close();
            return true;
          }
        }
      }
    }
    // id is closed automatically no need for id.close();
    return false;
  }

  template<class T>
  bool Parser::parseOptions(keyType key, std::vector<T> &values) const
  {
    if ( !isKeyDefined(key) )
      return false;

    ArgListConstIter A;
    OptionListConstIter O;
    for (A=Args.begin(); A!=Args.end(); ++A) {
      for (O=Options.find(key); O!=Options.upper_bound(key); ++O) {
        if (*A == O->second && *A != Args.back()) {
          T value;
          convert(*(++A), value);
          values.push_back(value);
        }
      }
    }
    return (values.empty() ? false : true);
  }

  template<class T>
  void Parser::convert(const string &str, std::vector<T> &vec) const
  {

    vec.clear();
    string::size_type begin(0);
    string::size_type end(str.npos);
    while ( (end=str.find(',', begin)) != str.npos ) {
      string sstr = str.substr(begin, end-begin);
      T value;
      convert(sstr, value);
      vec.push_back(value);
      begin = end+1;
    }
    string sstr = str.substr(begin);
    T value;
    convert(sstr, value);
    vec.push_back(value);

  }

#ifdef BZ_TINYVEC_H
  template<class T, int N>
  void Parser::convert(const string &str, blitz::TinyVector<T,N> &vec) const
  {
    string::size_type begin(0);
    string::size_type end(str.npos);
    if ( (end=str.find(',', begin)) == str.npos ) {
      T value;
      convert(str, value);
      vec = value;
    } else {
      int i = 0;
      do {
        string sstr = str.substr(begin, end-begin);
        T value;
        convert(sstr, value);
        vec(i++) = value;
        begin = end+1;
      } while ( (end=str.find(',', begin)) != str.npos && i < N );
      string sstr = str.substr(begin);
      T value;
      convert(sstr, value);
      vec(i) = value;
      if ( i != N-1 ) {
        ostringstream os;
        os << "Incorrect length for vector '" << str <<  "': " << i+1 << ", should be " << N << ".";
        throw ParserException(os.str());
      }
    }
  }
#endif

#ifdef BZ_ARRAY_H
  template<class T>
  void Parser::convert(const string &str, blitz::Array<T,1> &vec) const
  {
    string::size_type begin(0);
    string::size_type end(str.npos);
    if ( (end=str.find(',', begin)) == str.npos ) {
      T value;
      convert(str, value);
      vec.resize(1);
      vec = value;
    } else {
      int i = 0;
      do {
        string sstr = str.substr(begin, end-begin);
        T value;
        convert(sstr, value);
        vec.resizeAndPreserve(i+1);
        vec(i++) = value;
        begin = end+1;
      } while ( (end=str.find(',', begin)) != str.npos );
      string sstr = str.substr(begin);
      T value;
      convert(sstr, value);
      vec.resizeAndPreserve(i+1);
      vec(i) = value;
    }
  }

#endif

  // Trick to display elements of a Parser::LUT
  class map_elt {
  public:
    typedef Parser::LUT LUT;
    typedef std::ostream ostream;

    friend ostream& operator<<(ostream &os, const map_elt &val) {
      if (!val.elt.empty()) {
        StrSetConstIter i = val.elt.begin();
        StrSetConstIter end = val.elt.end();
        os << *i;
        for ( ++i; i != end; ++i )
          os << ',' << *i;
      }
      return os;
    }

    map_elt(LUT map, Parser::keyType key) : elt() {
      elt.push_back(map.find(key)->second);
    }
    map_elt(LUT map, std::vector<int> &keys) : elt() {
      if (!keys.empty()) {
        std::vector<int>::const_iterator i = keys.begin();
        std::vector<int>::const_iterator end = keys.end();
        for ( ; i != end; ++i )
          elt.push_back(map.find(*i)->second);
      }
    }
#ifdef BZ_TINYVEC_H
    template<int N>
    map_elt(LUT map, const blitz::TinyVector<int,N> &keys) {
      for (int i=0; i<N; ++i)
        elt.push_back(map.find(keys(i))->second);
    }
#endif

  private:

    typedef std::string string;
    typedef std::vector<string> StrSet;
    typedef StrSet::const_iterator StrSetConstIter;

    StrSet elt;
  };

  struct header {
    typedef std::string string;
    typedef std::ostream ostream;

    explicit header(const string &_title,
                    unsigned _blankbefore=0, unsigned _blankafter=0) :
      title(_title), blankbefore(_blankbefore),
      blankafter(_blankafter)
    {}
    friend ostream& operator<<(ostream &os, const header &val) {
      const string hashes(string(Parser::last_tab,'#') + '\n');
      const string blank("##" + string(Parser::last_tab-4,' ') + "##\n");
      string output;

      for (unsigned i=0; i<val.blankbefore; ++i)
        output += blank;

      string::size_type len = Parser::last_tab-val.title.length()-4;
      output = output + "##" + string((len+1)/2,' ') +
               val.title + string(len/2,' ') + "##\n";

      for (unsigned i=0; i<val.blankafter; ++i)
        output += blank;

      return os << hashes << output << hashes;
    }
    const string title;
    unsigned blankbefore;
    unsigned blankafter;
  };


  // Trick to display a bool type as yes or no
  struct yesno {
    bool b_;
    explicit yesno(bool b) : b_(b) {}
    friend std::ostream& operator<<(std::ostream &os, const yesno &val) {
      return os << (val.b_ ? "yes" : "no");
    }
  };

} // namespace parser

#endif // PARSER_H
