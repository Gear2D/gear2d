#include <stdio.h>
#include <string>
#include <iostream>
#include <list>
#include "yaml.h"


#include "sigfile.h"



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
    static void load(const string & filename, map<string, string> & target);
    
  private /* methods */:
    void process(yaml_event_t & event);
      
  private /* vars */:
    map<string, string> & target;
    list<string> level;
    tokentype previous;
    docstate state;
    string scalar;
};


void parser::load(const string & filename, map< string, string >& target) {
  parser p(filename, target);
}


parser::parser(const string & filename, map<string, string> & target)
: target(target)
, level()
, previous(parser::submap)
, state(parser::unknown) {
  yaml_parser_t p;
  yaml_event_t event;
  
  /* Initialize parser */
  if(!yaml_parser_initialize(&p))
    fputs("Failed to initialize parser!\n", stderr);
  
  FILE * fh = fopen(filename.c_str(), "r");
  if(fh == NULL)
    fputs("Failed to open file!\n", stderr);
  
  yaml_parser_set_input_file(&p, fh);
  
  do {
    if (!yaml_parser_parse(&p, &event)) {
      printf("Parser error %d\n", p.error);
      yaml_event_delete(&event);
      yaml_parser_delete(&p);
      fclose(fh);
      exit(EXIT_FAILURE); // throw things at people here.
    }
    process(event);
  } while (event.type != YAML_STREAM_END_EVENT);
  
  yaml_event_delete(&event);
  yaml_parser_delete(&p);
  fclose(fh);
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

void sigfile::load(const string & file, map<string, string> & target) {
  parser::load(file, target);
}

void sigfile::save(const string & target, const map< string, string >& source) {

}



