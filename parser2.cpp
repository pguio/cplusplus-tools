/**************************************************************************
 *
 * $Id: parser2.cpp,v 1.9 2016/06/01 14:53:55 patrick Exp $
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

#include <parser2.h>

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
    Cmd(), Args(0), Options(), Aliases(),
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
      if (str == "debug_parser") {
        debugParser = true;
      }
    }

    if (debugParser) {
      viewArgs(cout);
      viewOptions(cout);
    }

    using types::none;

    insertOption("-h", none, "print this help, then exit", Any());
    insertOptionAlias("-h", "--help");

    insertOption("-v", none, "print version number, then exit", Any());
    insertOptionAlias("-v", "--version");

    insertOption("-t", none, "print a template of available options, then exit", Any());
    insertOptionAlias("-t", "--create-template");

    insertOption("-i",types::charStr, "Filename of an input parameter file",Any());
    insertOptionAlias("-i","--input");

    inputFileNameParsed = parseOption("-i", inputFileName, cmdLine);
    if ((inputFileNameParsed = parseOption("-i", inputFileName, cmdLine))) {
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

  void Parser::setPrefix(const string &name)
  {
    prefixName = name;
  }

  void Parser::insertOption(const string &opt, valueType type,
                            const string &desc, const Any &defval)
  {
    if (debugParser) {
      cerr << "insertOption(" << opt << ","
           << type << "," << desc << ")" << endl;
      viewOptions(cerr);
    }
    if ( Options.find(opt) != Options.end() ) {
      cerr << "*** Warning : " <<
           "option (" << opt << "," << TypeValues.find(type)->second << ")";
      if (! className.empty())
        cerr << " in class '" << className << "'";
      if (! progName.empty())
        cerr << " in program '" << progName << "'";
      cerr << " alread exists\nSkipping insertion of option "
           << "(" << opt << ")" << endl;
      return;
    }

    Options.insert(OptionPair(string(prefixName+opt), Option(type,desc,defval)));
    Aliases.insert(AliasPair(string(prefixName+opt),string(prefixName+opt)));
  }


  void Parser::insertOptionAlias(const string &opt, const string &alias)
  {
    if (debugParser) {
      cerr << "insertOptionAlias(" << opt << "," << alias << ")" << endl;
      viewOptions(cerr);
    }
    if ( Options.find(opt) == Options.end() ) {
      cerr << "*** Warning : option " << opt;
      if (! className.empty())
        cerr << " in class '" << className << "'";
      if (! progName.empty())
        cerr << " in program '" << progName << "'";
      cerr << " not found\nSkipping insertion of alias" <<
           "(" << opt << "," << alias << ")" << endl;
      return;
    }

    Aliases.insert(AliasPair(string(prefixName+opt),string(alias)));
  }


  void Parser::parseLevelDebugOption(const string &name)
  {
    insertOption(name, types::integer, "Level of debug information", Any(classDebugLevel));
    parseOption(name, classDebugLevel);
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
    for (OptionIter O(Options.begin()) ; O!=Options.end(); ++O) // For all options
      if ((!className.empty() && O->first != "-h" && O->first != "-v" &&
           O->first != "-t" && O->first != "-i") || className.empty() ) {

        AliasIter A(Aliases.find(O->first));
        string optMsg(" " + A->second);
        for (++A; A!=Aliases.upper_bound(O->first); ++A)
          optMsg +=  ", " + A->second;
        formatString(optMsg,tab1); // Format

        if (! TypeValues.find(O->second.type)->second.empty())
          optMsg += TypeValues.find(O->second.type)->second;
        formatString(optMsg,tab2); // Format

        optMsg += O->second.description;
        formatString(optMsg,tab2,tab3); // Format
        os << optMsg << '\n';
      }
  }

  void Parser::viewTemplateOptions(string &opt) const
  {
    for (OptionIter O(Options.begin()) ; O!=Options.end(); ++O) // For all options
      if ((!className.empty() && O->first != "-h" && O->first != "-v" &&
           O->first != "-t" && O->first != "-i") || className.empty() ) {

        string hashes("\n" + string(last_tab,'#'));

        AliasIter A(Aliases.find(O->first));
        string templateMsg("\n## Option      : " + A->second);
        for (++A; A!=Aliases.upper_bound(O->first); ++A) // For all aliases
          templateMsg += ", " + A->second;
        templateMsg = hashes + templateMsg;

        if (! TypeValues.find(O->second.type)->second.empty())
          templateMsg += "\n## Type        : " + TypeValues.find(O->second.type)->second;

        templateMsg += "\n## Description : " + O->second.description + hashes;
        templateMsg += "\n" + O->first + "=" ;

        if (! O->second.value.empty()) {
          ostringstream os;
          os << O->second.value;
          templateMsg += os.str();
        }
        opt += templateMsg;
      }
  }

  bool Parser::parseHelp() const
  {
    if (parseOption("-h", cmdLine)) {
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
    if (parseOption("-v", cmdLine)) {
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
    if (parseOption("-t", cmdLine)) {
      if (! progName.empty()) {
        string header("\n## Automatically generated template setup file");
        const time_t now(time(0));
        string str_now(ctime(&now));
        str_now.erase(str_now.size()-1);
        header += "\n## Date              : " + str_now;
        string hashes(last_tab,'#');
        header = hashes + header;
        header += "\n## Command           : " + progName;
        for (ArgListConstIter A(Args.begin()) ; A!=Args.end(); ++A)
          header += " " + *A;
        header += "\n" + hashes + "\n#";
        cout << header;
      }
      if (! className.empty()) {
        string hashes("\n" + string(last_tab,'#'));
        string header("\n## Options for class : " + className);
        header += "\n## Package           : " + packageName;
        string version(numVersion);
        string::size_type pos(0);
        while ((pos=version.find('\n',pos)) != version.npos) {
          if (pos < version.size()-1) {
            version.insert(++pos,"## ");
            pos += 4;
          } else
            version.erase(version.size()-1);
        }
        header += "\n## Version           : " + version;
        header = hashes + header + hashes + "\n#";
        string opt;
        viewTemplateOptions(opt);
        header += opt;
        cout << header;
      }
      return true;
    }
    return false;
  }

  bool Parser::parseCmdLine(const string &opt) const
  {
    if (! isKeyDefined(opt))
      return false;

    for (ArgListConstIter A(Args.begin()); A!=Args.end(); ++A)
      for (AliasIter O(Aliases.find(opt)); O!=Aliases.upper_bound(opt); ++O)
        if (*A == O->second)
          return true;

    return false;
  }

  bool Parser::parseInpFile(const string &opt) const
  {
    if (! isKeyDefined(opt) || ! inputFileNameParsed)
      return false;

    // there is no "nocreate" in StdC++ :(
    // ios::in is default
    ifstream fid(inputFileName.c_str());
    if (! fid)
      throw ParserException("Cannot open file '" + inputFileName + "'\n");

    // automatic memory management, also doesn't overflow
    string line;
    // fixes the usual eof() bug
    while (getline(fid, line)) {
      if (line[0]!='#' && line[0]!='%' && (line[0]!='/' && line[0]!='/')) {
        for (AliasIter O(Aliases.find(opt)); O!=Aliases.upper_bound(opt); ++O)
          if (line == O->second)
            return true;
      }
    }
    return false;
  }

  bool Parser::parseOption(const string &opt, parseMode mode) const
  {
    try {

      bool parsed = false;
      switch (mode) {
      case inpFile:
        parsed = parseInpFile(opt);
        break;
      case cmdLine:
        parsed = parseCmdLine(opt);
        break;
      case inpFile_cmdLine:
        parsed = parseInpFile(opt) | parseCmdLine(opt);
        break;
      case cmdLine_inpFile:
        parsed = parseCmdLine(opt) | parseInpFile(opt);
        break;
      }
      return parsed;

    } catch (ParserException &e) {
      AliasIter O(Aliases.find(opt));
      string msg("In option " + O->second);
      for (++O; O!=Aliases.upper_bound(opt); ++O)
        msg += ", " + O->second;

      msg.append("\n" + string(e.what()));
      throw ParserException(msg);
    }
  }

  void Parser::checkMap(const string &opt, const LUT &map, int value) const
  {
    if (! isKeyDefined(opt))
      return;

    LUTConstIter i;

    i = map.find(value);
    if ( i == map.end() ) {
      ostringstream os;
      os << "\tRegistered values are:\n";
      for (i=map.begin(); i != map.end(); ++i)
        os << "\t" << i->first << "\t==>\t" << i->second << '\n';
      string msg(os.str());
      OptionIter O(Options.find(opt));
      throw ParserException("Invalid value for parameter " + O->first + "\n" + msg);
    }
  }

  bool Parser::isKeyDefined(const string &opt) const
  {
    if (Aliases.find(opt) == Aliases.end()) {
      cerr << "*** Warning: Undefined option '" << opt
           << "'\tSkipping..." << endl;
      return false;
    }
    return true;
  }

  template<>
  void Parser::convert(const string &str, bool &value, bool failIfLeftoverChars)
  {
//    istringstream is(str);
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
          value = pow(value, x);
          break;
        default:
          string op(1, c);
          throw  ParserException("Operator '" + op + "' not supported\n");
        }
      }
    }
  }


} // namespace parser
