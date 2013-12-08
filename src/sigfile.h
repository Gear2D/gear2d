#ifndef gear2d_sigfile_h
#define gear2d_sigfile_h

#include <string>
#include "yaml.h"
#include <list>
#include <map>

using std::string;
using std::map;

class sigfile {
  public:
    static bool load(const string & file, map<string, string> & target);
    static bool save(const string & target, const map<string, string> & source);
};



#endif // gear2d_sigfile_h
