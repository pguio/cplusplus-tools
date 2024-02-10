/**************************************************************************
 *
 * $Id: parser.cpp,v 1.44 2016/06/01 14:53:55 patrick Exp $
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

#include <cmath>
#include <cstring>

#include <unistd.h>        // only needed for definition of gethostname
#include <sys/param.h>     // only needed for definition of MAXHOSTNAMELEN

#include <parser.h>

using std::cerr;
using std::cout;
using std::endl;
using std::getline;

namespace parser {

  Parser::ValueDescPair v_mapInit[] = {
    Parser::ValueDescPair(types::none      ,         ""),
    Parser::ValueDescPair(types::boolean   ,     "bool"),
    Parser::ValueDescPair(types::character ,     "char"),
    Parser::ValueDescPair(types::integer   ,      "int"),
    Parser::ValueDescPair(types::real      ,     "real"),
    Parser::ValueDescPair(types::charStr   ,   "string"),
    Parser::ValueDescPair(types::boolVect  ,   "bool[]"),
    Parser::ValueDescPair(types::charVect  ,   "char[]"),
    Parser::ValueDescPair(types::intVect   ,    "int[]"),
    Parser::ValueDescPair(types::realVect  ,   "real[]"),
    Parser::ValueDescPair(types::stringVect, "string[]"),
  };

  Parser::ValuesDescList::value_type *v_mapInitEnd(v_mapInit+
      sizeof v_mapInit / sizeof v_mapInit[0]);

  Parser::ValuesDescList Parser::TypeValues(v_mapInit, v_mapInitEnd);

  Parser::Parser(int nargs, char* args []) :
    Cmd(), Args(0),
    Options(), OptionsDefaultValue(), OptionType(), OptionDesc(),
    className(), packageName(), numVersion(), copyrightText(), progName(),
    prefixName(), debugParser(false), classDebugLevel(0), inputFileName(),
    inputFileNameParsed(false)
  {
#if defined(HAVE_MPI)
    MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rankProc);
#endif

    if (nargs > 0) Cmd = args[0];
    for (int i=1; i<nargs; ++i) {
      // Split argument if '=' is present
      string str(args[i]);
      stringSizeType pos = str.find('=');
      stringSizeType len = str.length();
      if (pos == string::npos || pos == len-1) {
        Args.push_back(str);
      } else {
        string str1(str.substr(0, pos));
        Args.push_back(str1);
        string str2(str.substr(pos+1));
        Args.push_back(str2);
      }
      //Args.push_back(args[i]);
      if (strcmp(args[i], "debug_parser")==0) {
        debugParser = true;
      }
    }

    if (debugParser) {
      viewArgs(cout);
      viewOptions(cout);
    }

    insertOption(_help,"-h", types::none,
                 "print this help, then exit", Any());
    insertOptionAlias(_help, "--help");

    insertOption(_version,"-v", types::none,
                 "print version number, then exit", Any());
    insertOptionAlias(_version, "--version");

    insertOption(_template,"-t", types::none,
                 "print a template of available options, then exit", Any());
    insertOptionAlias(_template, "--create-template");

    insertOption(_file,"-i",types::charStr,
                 "Filename of an input parameter file",Any());
    insertOptionAlias(_file,"--input");

    if ((inputFileNameParsed = parseOption(_file, inputFileName, cmdLine))) {
      ifstream fid(inputFileName.c_str());
      if (! fid)
        throw ParserException("Input file '" + inputFileName + "' not found\n");
    }
  }

  Parser::~Parser()
  {}


  void Parser::printCmd(std::ostream &os) const
  {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    os << "date        : " << asctime(timeinfo);

    char hostname[MAXHOSTNAMELEN];
    gethostname(hostname, MAXHOSTNAMELEN);
    os << "hostname    : " << hostname << endl;

    char cwd[PATH_MAX];
    getcwd(cwd,PATH_MAX);
    os << "working dir : " << cwd << endl;

    os << "full command: " << Cmd;
    for (ArgListConstIter I=Args.begin(); I!=Args.end(); ++I) {
      os << ' ' << *I;
    }
    os << endl;
  }

  void Parser::viewArgs(std::ostream &os) const
  {
    cout
        << "*****************" << endl
        << "Passed arguments:" << endl;
    int i = 1;
    for (ArgListConstIter I=Args.begin(); I!=Args.end(); ++I)
      cout << "arg(" << i++ << ")=" << *I << endl;
    cout
        << "*****************" << endl;
  }

  void Parser::registerClass(const string &name)
  {
    className = name;
  }

  void Parser::registerProgram(const string &name)
  {
    progName = name;
  }

  void Parser::registerPackage(const string &package, const string &version,
                               const string &copyright)
  {
    packageName   = package;
    numVersion    = version;
    copyrightText = copyright;
  }

  void Parser::setPrefix(const string name)
  {
    prefixName = name;
  }

  void Parser::insertOption(keyType key, const string name, valueType type,
                            const string desc, const Any &defval)
  {
    if (debugParser) {
      cerr << "insertOption(" << key << "," <<
           name << "," << type << "," << desc << ")" << endl;
      viewOptions(cerr);
    }
    if ( OptionType.find(key) != OptionType.end() ) {
      cerr << "*** Warning : " <<
           "option (" << key << ","
           << TypeValues.find(OptionType.find(key)->second)->second << ")";
      if (! className.empty())
        cerr << " in class '" << className << "'";
      if (! progName.empty())
        cerr << " in program '" << progName << "'";
      cerr << " alread exists\nSkipping insertion of key "
           << "(" << key << "," << name << ")" << endl;
      return;
    }
    string tname(prefixName+name);

    Options.insert(OptionList::value_type(key, tname));
    OptionsDefaultValue.insert(ValuePair(key,defval));
    OptionType.insert(OptionValuePair(key,type));
    OptionDesc.insert(OptionDescPair(key,desc));
  }


  void Parser::insertOptionAlias(keyType key, const string alias)
  {
    if (debugParser) {
      cerr << "insertOptionAlias(" << key << "," << alias << ")" << endl;
      viewOptions(cerr);
    }
    if ( OptionType.find(key) == OptionType.end() ) {
      cerr << "*** Warning : key " << key;
      if (! className.empty())
        cerr << " in class '" << className << "'";
      if (! progName.empty())
        cerr << " in program '" << progName << "'";
      cerr << " not found\nSkipping insertion of alias" <<
           "(" << key << "," << alias << ")" << endl;
      return;
    }

    Options.insert(OptionList::value_type(key, alias));
  }


  void Parser::parseLevelDebugOption(const string name)
  {
    keyType key = _last+1;
    insertOption(key, name, types::integer,
                 "Level of debug information", Any(classDebugLevel));
    parseOption(key, classDebugLevel);
  }

  int Parser::debugLevel() const
  {
    return classDebugLevel;
  }


  void Parser::formatString(string &str, unsigned t)
  {
    if ( str.size() < t ) {
      str.resize(t,' ');
      return;
    }
    if ( str.size() > t ) {
      cerr << "*** Warning: Truncate " << str << " to ";
      str.resize(t,' ');
      cerr << str << '\n';
    }
  }

  void Parser::formatString(string &str, unsigned t1, unsigned t2)
  {
    if (str.size() > t2 ) {
      string dummy("\n");
      dummy += string(t1,' ');
      // Don't cut a word in two!
      stringIter iter(str.begin());
      for (unsigned i=1; i<= t2; ++i)
        ++iter;
      stringIter iter1(iter);
      if (*iter1 != ' ' && *iter1++ == ' ') {
        str.erase(iter1);
      } else {
        while (*iter != ' ') {
          t2--;
          iter--;
        }
        str.erase(iter);
      }
      str.insert(t2,dummy);
    }
  }

  void Parser::viewOptions(std::ostream &os) const
  {
    OptionListConstIter O(Options.begin());
    OptionListConstIter last(Options.end());

    // If parser of a class, skip the default parser options
    if (! className.empty()) {
      O = Options.find(_last);
      for (unsigned i=0; i<Options.count(_last); ++i)
        ++O;
    }
    for ( ; O!=last; ++O) {
      keyType key = O->first;
      string opt_msg(" ");
      if (debugParser) {
        ostringstream os;
        os << key;
        opt_msg += os.str() + " ";
        // Format
        formatString(opt_msg, 5u);
      }
      for (unsigned i=0; i<Options.count(key)-1; ++i, ++O) {
        opt_msg +=  O->second + " | ";
      }
      opt_msg += O->second ;
      // Format
      formatString(opt_msg,tab1);
      const string type(TypeValues.find(OptionType.find(O->first)->second)->second);
      if (! type.empty())
        opt_msg += type;
      // Format
      formatString(opt_msg,tab2);
      const string descr(OptionDesc.find(O->first)->second);
      opt_msg += descr;
      // Format
      formatString(opt_msg,tab2,tab3);
      os << opt_msg << '\n';
    }
  }

  void Parser::viewTemplateOptions(string &options_templates) const
  {
    OptionListConstIter O(Options.begin());
    OptionListConstIter last(Options.end());

    // If parser of a class, skip the default parser options
    if (! className.empty()) {
      O = Options.find(_last);
      // skip the values for the default parser options
      for (unsigned i=0; i<Options.count(_last); ++i)
        ++O;
    }
    for ( ; O!=last; ++O) { // Loop over option keys
      keyType key = O->first;
      string template_msg;
      template_msg = "\n## Option      : ";
      for (unsigned i=0; i<Options.count(key)-1; ++i, ++O) // Loop over aliases
        template_msg = template_msg + O->second + ", ";
      template_msg += O->second;
      string hashes("\n" + string(last_tab,'#'));
      template_msg = hashes + template_msg;
      const string type(TypeValues.find(OptionType.find(key)->second)->second);
      if (! type.empty())
        template_msg = template_msg + "\n## Type        : " + type;
      const string descr(OptionDesc.find(O->first)->second);
      template_msg = template_msg + "\n## Description : " + descr + hashes;
      template_msg = template_msg + "\n" + O->second + "=" ;
      if (OptionsDefaultValue.find(key) != OptionsDefaultValue.end() &&
          !OptionsDefaultValue.find(key)->second.empty()) {
        ostringstream os;
        os << OptionsDefaultValue.find(key)->second;
        const string val(os.str());
        template_msg = template_msg + val;
      }
      options_templates += template_msg;
    }
  }

  bool Parser::parseHelp() const
  {
    if (parseOption(_help, cmdLine)) {
      if (! className.empty()) {
        cerr << "Registered options for class '" << className <<"':\n" << endl;
      } else if (! progName.empty()) {
        cerr << "Usage: " << progName <<" [options]\n" << endl;
        cerr << "Registered options:\n" << endl;
      }
      viewOptions(cerr);
      cerr << endl;
      return true;
    }
    return false;
  }

  bool Parser::parseVersion() const
  {
    if (parseOption(_version, cmdLine)) {
      if (! packageName.empty()) {
        if (! className.empty()) {
          cerr << "Class '" << className << "'";
        } else {
          cerr << "Program " << progName ;
        }
        cerr << " from Package " << packageName
             << " Version " << numVersion << endl << copyrightText << endl;
      } else {
        cerr << "No registration for progam " << progName << "\n" << endl;
      }
      return true;
    }
    return false;
  }

  bool Parser::parseTemplate() const
  {
    if (parseOption(_template, cmdLine)) {
      if (! className.empty()) {
        string header;
        header = "\n## Options for class : " + className;
        string hashes("\n" + string(last_tab,'#'));
        header = header + "\n## Package           : " + packageName;
        string version(numVersion);
        string::size_type pos(0);
        while ((pos=version.find('\n',pos)) != version.npos) {
          if (pos < version.size()-1) {
            version.insert(++pos,"## ");
            pos += 4;
          } else
            version.erase(version.size()-1);
        }
        header = header + "\n## Version           : " + version;
        header = hashes + header + hashes + "\n#";
        string options_templates;
        viewTemplateOptions(options_templates);
        header += options_templates;
        cout << header;
      }
      if (! progName.empty()) {
        string header("\n## Automatically generated template setup file");
        const time_t now(time(0));
        string str_now(ctime(&now));
        str_now.erase(str_now.size()-1);
        header = header + "\n## Date              : " + str_now;
        string hashes(last_tab,'#');
        header = hashes + header;
        header = header + "\n## Command           : " + progName;
        ArgListConstIter last(Args.end());
        for (ArgListConstIter A(Args.begin()) ; A!=last; ++A)
          header = header + " " + *A;
        header = header + "\n" + hashes + "\n#";
        cout << header;
      }
      return true;
    }
    return false;
  }

  bool Parser::parseCmdLine(keyType key) const
  {
    if (! isKeyDefined(key))
      return false;

    ArgListConstIter A;
    OptionListConstIter O;
    for (A=Args.begin(); A!=Args.end(); ++A) {
      string arg(*A);
      for (O=Options.find(key); O!=Options.upper_bound(key); ++O) {
        string option(O->second);
        if (arg == option) {
          return true;
        }
      }
    }
    return false;
  }

  bool Parser::parseInpFile(keyType key) const
  {
    if (! isKeyDefined(key) || ! inputFileNameParsed)
      return false;

    // there is no "nocreate" in StdC++ :(
    // ios::in is default
    ifstream fid(inputFileName.c_str());
    if (! fid)
      throw ParserException("Cannot open file '" + inputFileName + "'\n");

    // automatic memory management, also doesn't overflow
    string line;
    OptionListConstIter O;
    // fixes the usual eof() bug
    while (getline(fid, line)) {
      if (line[0]!='#' && line[0]!='%' && (line[0]!='/' && line[0]!='/')) {
        for (O=Options.find(key); O!=Options.upper_bound(key); ++O) {
          string option(O->second);
          if (line == option) {
            return true;
          }
        }
      }
    }
    return false;
  }

  bool Parser::parseOption(keyType key, parseMode mode) const
  {
    try {

      bool parsed = false;
      switch (mode) {
      case inpFile:
        parsed = parseInpFile(key);
        break;
      case cmdLine:
        parsed = parseCmdLine(key);
        break;
      case inpFile_cmdLine:
        parsed = parseInpFile(key) | parseCmdLine(key);
        break;
      case cmdLine_inpFile:
        parsed = parseCmdLine(key) | parseInpFile(key);
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

  void Parser::checkMap(keyType key, const LUT &_map, int value) const
  {
    if (! isKeyDefined(key))
      return;

    LUTConstIter i;

    i = _map.find(value);
    if ( i == _map.end() ) {
      ostringstream os;
      os << "\tRegistered values are:\n";
      for (i=_map.begin(); i != _map.end(); ++i)
        os << "\t" << i->first << "\t==>\t" << i->second << '\n';
      string msg(os.str());
      OptionListConstIter O(Options.find(key));
      throw ParserException("Invalid value for parameter " + O->second + "\n" + msg);
    }
  }

  bool Parser::isKeyDefined(keyType key) const
  {
    if (OptionType.find(key) == OptionType.end()) {
      cerr << "*** Warning: Undefined key '" << key
           << "'\tSkipping..." << endl;
      return false;
    }
    return true;
  }

  template<>
  void Parser::convert(const string &str, bool &value, bool failIfLeftoverChars)
  {
    istringstream is(str);
    if (str == "yes" || str == "true" || str == "1") {
      value = true;
      return ;
    }
    if (str == "no" || str == "false" || str == "0") {
      value = false;
      return ;
    }
    // Otherwise
    throw  BadConversion(str);
  }



  template<>
  void Parser::convert(const string &str, char* &value, bool failIfLeftoverChars)
  {
    value = strdup(str.c_str());
  }

  template<>
  void Parser::convert(const string &str, float &value, bool failIfLeftoverChars)
  {
    istringstream is(str);
    char c;
    float x;

    if (!(is >> x)) {
      throw  BadConversion(str);
    } else {
      value = x;
      if (is.get(c) && is >> x) {
        switch (c) {
        case '/':
          value /= x;
          break;
        case '*':
          value *= x;
          break;
        case '^':
          value = powf(value, x);
          break;
        default:
          string op(1, c);
          throw  ParserException("Operator '" + op + "' not supported\n");
        }
      }
    }
  }

  template<>
  void Parser::convert(const string &str, double &value, bool failIfLeftoverChars)
  {
    istringstream is(str);
    char c;
    float x;

    if (!(is >> x)) {
      throw  BadConversion(str);
    } else {
      value = x;
      if (is.get(c) && is >> x) {
        switch (c) {
        case '/':
          value /= x;
          break;
        case '*':
          value *= x;
          break;
        case '^':
          value = (double)pow((double)value, (double)x);
          break;
        default:
          string op(1, c);
          throw  ParserException("Operator '" + op + "' not supported\n");
        }
      }
    }
  }


} // namespace parser
