#include <stdio.h>
#include <SDL_rwops.h>
#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include "yaml.h"


#include "sigfile.h"
#include "SDL.h"
#include "log.h"


using namespace std;

string join(list<string> strlist) {
  string result;
  for (list<string>::iterator it = strlist.begin(); it != strlist.end(); it++) {
    result += *it;
    if (++it != strlist.end()) result += ".";
    it--;
  }
  return result;
}

class parser {
  private:
    parser(const string & filename, map<string, string> & target);
    ~parser();
    
  private /* types */:
    enum docstate {
      invalid = -1, /* documents that aren't maps */
      unknown, /* document is not yet ready */
      valid /* document is a mapping and is valid */
    };
    
    enum tokentype {
      key,
      value,
      submap
    };
      
  public:
    static bool load(const string & filename, map<string, string> & target);
    
  private /* methods */:
    void process(yaml_event_t & event);
      
  private /* vars */:
    map<string, string> & target;
    list<string> level;
    tokentype previous;
    docstate state;
    string scalar;
};


bool parser::load(const string & filename, map< string, string >& target) {
  try {
    parser p(filename, target);
  } catch (int i) {
    return false;
  }
  return true;
}

int rwgetc(SDL_RWops * rw) {
  unsigned char c;
  return (SDL_RWread(rw, &c, sizeof(char), 1) == 1) ? (int) c : EOF;
}

// slighly modified K&R implementation of fgets with rwops
char * rwgets(char * s, size_t n, SDL_RWops * rw) {
  int c;
  char * cs = s;
  while (--n > 0 && (c = rwgetc(rw)) != EOF) {
    if ((*cs++ = c) == '\n')
      break;
  }
  
  *cs = '\0';
  return (c == EOF && cs == s ? NULL : s);
}

parser::parser(const string & filename, map<string, string> & target)
: target(target)
, level()
, previous(parser::submap)
, state(parser::unknown) {
  modinfo("sigfile-parser");
  yaml_parser_t p;
  yaml_event_t event;
  
  /* Initialize parser */
  if(!yaml_parser_initialize(&p))
    trace("Failed to initialize parser!");

  static char buf[1024];
  SDL_RWops * rw;
  rw = SDL_RWFromFile(filename.c_str(), "r");
  stringstream sstr;
  sstr.str();
  while (rwgets(buf, sizeof(buf), rw) != NULL) {
    sstr << buf;
  }
  SDL_RWclose(rw);
  string inputstring = sstr.str();

  yaml_parser_set_input_string(&p, (unsigned char *)inputstring.c_str(), inputstring.size());
  
  do {
    if (!yaml_parser_parse(&p, &event)) {
      trace("YAML Parser error:", p.problem, "value (as int):", p.problem_value, "offset in file:", p.problem_offset);
      yaml_event_delete(&event);
      yaml_parser_delete(&p);
      throw(p.problem_value);
    }
    process(event);
  } while (event.type != YAML_STREAM_END_EVENT);
  
  yaml_event_delete(&event);
  yaml_parser_delete(&p);
}

void parser::process(yaml_event_t & event) {
  switch(event.type) {
    case YAML_NO_EVENT:
    case YAML_STREAM_START_EVENT:
    case YAML_STREAM_END_EVENT:
    case YAML_DOCUMENT_START_EVENT:
    case YAML_DOCUMENT_END_EVENT:
    case YAML_SEQUENCE_START_EVENT:
    case YAML_SEQUENCE_END_EVENT:
    case YAML_ALIAS_EVENT:
      break;
      
    case YAML_MAPPING_START_EVENT:
      // valid gear2d documents starts always are mappings.
      if (state == unknown) state = valid;
                    if (!scalar.empty()) {
                      level.push_back(scalar);
                      scalar.clear();
                    }
                    
                    previous = submap; // we are reading a submap now.
                    
                    break;
      
    case YAML_MAPPING_END_EVENT:
      if (!level.empty()) { // might be the top-level map.
        level.pop_back();
      }
      break;
      
    case YAML_SCALAR_EVENT:
      /* new key? */
      if (previous == submap || previous == value) {
        previous = key;
        scalar = (const char *)event.data.scalar.value;
      } else if (previous == key) { /* got a new value! */
        string k;
        if (!level.empty()) {
          k = join(level);
          if (scalar != "value") k = k + "." + scalar;
        } else k = scalar;
                    
                    /* get this new scalar */
                    scalar = (const char *)event.data.scalar.value;
        
        /* put the value in the target map */
        target[k] = scalar;
        previous = value;
      }
      break;
  }
}

parser::~parser() {
  
}

bool sigfile::load(const string & file, map<string, string> & target) {
  return parser::load(file, target);
}

bool sigfile::save(const string & target, const map< string, string >& source) {
  return false;
}



