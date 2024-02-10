/**************************************************************************
 *
 * $Id: parser2.h,v 1.6 2011/04/05 20:45:44 patrick Exp $
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
#include <set>
#include <vector>
#include <algorithm>
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
    explicit ParserException(std::string s) throw()
      : ClassException("parser::Parser", s)
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

    typedef int valueType;

    struct Option {
      valueType type;
      string description;
      Any value;
      Option(valueType t, const string &d, const Any &v) : type(t), description(d), value(v)
      {}
    };

    typedef std::map<string, Option> OptionList;
    typedef OptionList::value_type OptionPair;
    typedef OptionList::const_iterator OptionIter;

    typedef std::multimap<string,string> AliasList;
    typedef AliasList::value_type AliasPair;
    typedef AliasList::const_iterator AliasIter;

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

    void setPrefix(const string &name);

    void insertOption(const string &opt, valueType type, const string &desc, const Any &defval);
    void insertOptionAlias(const string &opt, const string &alias);

    void parseLevelDebugOption(const string &name);
    int debugLevel() const;

    virtual bool parseHelp() const;
    virtual bool parseVersion() const;
    virtual bool parseTemplate() const;

    bool parseCmdLine(const string &opt) const;
    bool parseInpFile(const string &opt) const;

    bool parseOption(const string &opt, parseMode mode=inpFile_cmdLine) const;

    template<typename T_type>
    bool parseCmdLine(const string &opt, T_type &value) const;
    template<typename T_type>
    bool parseInpFile(const string &opt, T_type &value) const;

    template<typename T_type>
    bool parseOption(const string &opt, T_type &value, parseMode mode=inpFile_cmdLine) const;

    template<typename T_type, typename Test>
    bool parseOption(const string &opt, T_type &value, parseMode mode=inpFile_cmdLine) const;

    template<typename T_type>
    bool parseOptions(const string &opt, std::vector<T_type> &value) const;

  protected:

#if defined(HAVE_MPI)

    int nbProc;
    int rankProc;
#endif

    void checkMap(const string &opt, const LUT &map, int value) const;
#ifdef BZ_TINYVEC_H

    template<int N>
    void checkMap(const string &opt, const LUT &map,
                  const  blitz::TinyVector<int,N> &values) const;
#endif

  private:

    string Cmd;
    ArgList Args;
    OptionList Options;
    AliasList Aliases;

    static ValuesDescList TypeValues;

    string className;
    string packageName;
    string numVersion;
    string copyrightText;
    string progName;
    string prefixName;

    bool debugParser;
    int classDebugLevel;

    string inputFileName;
    bool inputFileNameParsed;

    static void formatString(string &str, unsigned tabend);
    static void formatString(string &str, unsigned tab1, unsigned tab2);

    void viewOptions(std::ostream &os) const;
    void viewTemplateOptions(string &opt) const;

    bool isKeyDefined(const string &opt) const;

    template<typename T_type>
    static void convert(const string &str, T_type &x, bool failIfLeftoverChars = true);

    template<typename T_type>
    void convert(const string &str, std::vector<T_type> &val) const;

#ifdef BZ_TINYVEC_H

    template<typename T_type,int N>
    void convert(const string &str,  blitz::TinyVector<T_type,N> &val) const;
#endif
#ifdef BZ_ARRAY_H

    template<typename T_type>
    void convert(const string &str,  blitz::Array<T_type,1> &val) const;
#endif

  };

  template<typename T_type>
  void  Parser::convert(const string &str, T_type &x, bool failIfLeftoverChars)
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

  template<typename T_type>
  bool Parser::parseOption(const string &opt, T_type &value, parseMode mode) const
  {
    try {

      bool parsed = false;
      switch (mode) {
      case inpFile:
        parsed = parseInpFile(opt, value);
        break;
      case cmdLine:
        parsed = parseCmdLine(opt, value);
        break;
      case inpFile_cmdLine:
        parsed = parseInpFile(opt, value) | parseCmdLine(opt, value);
        break;
      case cmdLine_inpFile:
        parsed = parseCmdLine(opt, value) | parseInpFile(opt, value);
        break;
      }
      return parsed;

    } catch (ParserException &e) {
      AliasIter O(Aliases.find(opt));
      string msg("In option " + O->second);
      for (++O; O!=Aliases.upper_bound(opt); ++O) {
        if (O->second == opt)
          msg = msg + ", " + O->second;
      }
      msg.append("\n" + string(e.what()));
      throw ParserException(msg);
    }
  }

  template<typename T_type, typename Test>
  bool Parser::parseOption(const string &opt, T_type &value, parseMode mode) const
  {
    try {
      bool parsed = parseOption<T_type>(opt, value, mode);
      if (parsed) {
        Test test(value);
        test.run();
      }
      return parsed;
    } catch (ParserException &e) {
      AliasIter O(Aliases.find(opt));
      string msg("Invalid range of parameter in option " + O->second);
      for (++O; O!=Aliases.upper_bound(opt); ++O) {
        if (O->second == opt)
          msg = msg + ", " + O->second;
      }
      msg.append("\n" + string(e.what()));
      throw ParserException(msg);
    }
  }

#ifdef BZ_TINYVEC_H
  template<int N>
  void Parser::checkMap(const string &opt, const LUT &map,
                        const blitz::TinyVector<int,N> &values) const
  {
    for (int d=0; d<N; ++d)
      checkMap(opt, map, values(d));
  }
#endif

  template<typename T_type>
  bool Parser::parseCmdLine(const string &opt, T_type &value) const
  {
    if (! isKeyDefined(opt))
      return false;

//    for (ArgListIter A=Args.begin(); A!=Args.end(); ++A) {
    for (AliasIter O=Aliases.find(opt); O!=Aliases.upper_bound(opt); ++O) {
      ArgListConstIter A = std::find(Args.begin(), Args.end(), O->second);
      if (A != Args.end() && *A !=  Args.back()) {
//        if (*A == O->second && *A != Args.back()) {
        convert(*(++A), value);
        return true;
      }
    }
//    }
    return false;
  }

  template<typename T_type>
  bool Parser::parseInpFile(const string &opt, T_type &value) const
  {
    using std::getline;
    if (! isKeyDefined(opt) || ! inputFileNameParsed)
      return false;

    // there is no "nocreate" in StdC++ :(
    // ios::in is default
    ifstream fid(inputFileName.c_str());
    if (! fid)
      throw ParserException("Cannot open file '" + inputFileName + "'");

    // automatic memory management, also doesn't overflow
    string line;
    // fixes the usual eof() bug
    while (getline(fid, line)) {
      if (line[0]!='#' && line[0]!='%') {
        for (AliasIter O(Aliases.find(opt)); O!=Aliases.upper_bound(opt); ++O) {
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

  template<typename T_type>
  bool Parser::parseOptions(const string &opt, std::vector<T_type> &values) const
  {
    if ( !isKeyDefined(opt) )
      return false;

    for (ArgListConstIter A(Args.begin()); A!=Args.end(); ++A) {
      for (AliasIter O(Aliases.find(opt)); O!=Aliases.upper_bound(opt); ++O) {
        if (*A == O->second && *A != Args.back()) {
          T_type value;
          convert(*(++A), value);
          values.push_back(value);
        }
      }
    }
    return (values.empty() ? false : true);
  }

  template<typename T_type>
  void Parser::convert(const string &str, std::vector<T_type> &vec) const
  {

    vec.clear();
    string::size_type begin(0);
    string::size_type end(str.npos);
    while ( (end=str.find(',', begin)) != str.npos ) {
      string sstr = str.substr(begin, end-begin);
      T_type value;
      convert(sstr, value);
      vec.push_back(value);
      begin = end+1;
    }
    string sstr = str.substr(begin);
    T_type value;
    convert(sstr, value);
    vec.push_back(value);

  }

#ifdef BZ_TINYVEC_H
  template<typename T_type, int N>
  void Parser::convert(const string &str, blitz::TinyVector<T_type,N> &vec) const
  {
    string::size_type begin(0);
    string::size_type end(str.npos);
    if ( (end=str.find(',', begin)) == str.npos ) {
      T_type value;
      convert(str, value);
      vec = value;
    } else {
      int i = 0;
      do {
        string sstr = str.substr(begin, end-begin);
        T_type value;
        convert(sstr, value);
        vec(i++) = value;
        begin = end+1;
      } while ( (end=str.find(',', begin)) != str.npos && i < N );
      string sstr = str.substr(begin);
      T_type value;
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
  template<typename T_type>
  void Parser::convert(const string &str, blitz::Array<T_type,1> &vec) const
  {
    string::size_type begin(0);
    string::size_type end(str.npos);
    if ( (end=str.find(',', begin)) == str.npos ) {
      T_type value;
      convert(str, value);
      vec.resize(1);
      vec = value;
    } else {
      int i = 0;
      do {
        string sstr = str.substr(begin, end-begin);
        T_type value;
        convert(sstr, value);
        vec.resizeAndPreserve(i+1);
        vec(i++) = value;
        begin = end+1;
      } while ( (end=str.find(',', begin)) != str.npos );
      string sstr = str.substr(begin);
      T_type value;
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
        StrSetIter i = val.elt.begin();
        StrSetIter end = val.elt.end();
        os << *i;
        for ( ++i; i != end; ++i )
          os << ',' << *i;
      }
      return os;
    }

    map_elt(LUT map, int key) : elt() {
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
    typedef StrSet::const_iterator StrSetIter;

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
  // Trick to display a bool type as true or false
  struct truefalse {
    bool b_;
    explicit truefalse(bool b) : b_(b) {}
    friend std::ostream& operator<<(std::ostream &os, const truefalse &val) {
      return os << (val.b_ ? "true" : "false");
    }
  };

} // namespace parser

#endif // PARSER_H
