#include "gear2d.h"
#include "log.h"
#include <stdio.h>
#include <string.h>


void help() {
  printf("gear2d [options] [scene-file]\n");
  printf("Options:\n"
         "\t-v        : Prints Gear2D version\n"
         "\t-h        : Prints this help\n"
         "\t-l<level> : Verbosity level to the logging messages. 0 is the lowest,\n"
         "\t            4 is the highest.\n"
         "\t-f<filter>: Filter string to apply to the logging messages \n");
}

int main(int argc, char ** argv, char ** env) {
  char * arg = 0;
  const char * scene = "gear2d.yaml";
  while (argc > 1) {
    arg = argv[argc-1];
    if (strlen(arg) == 2 && arg[0] == '-' && arg[1] == 'v') {
      printf("%s\n", gear2d::engine::version());
      exit(0);
    }
    else if (arg[0] == '-') {
      switch (arg[1]) {
        case 'l': {
          int level = atoi(arg+2);
          if (level <= gear2d::log::minimum) level = gear2d::log::minimum+1;
          if (level > gear2d::log::maximum) level = gear2d::log::maximum;
          gear2d::log::globalverb = (gear2d::log::verbosity)level;
          break;
        }
        
        case 'f': {
          gear2d::log::filter = (arg+2);
          break;
        }
        
        case 'h': {
          help();
          exit(0);
        }
        
        default: {
          printf("Unknown argument %s.\n", arg);
          help();
          exit(0);
        }
      }
    }
    else scene = arg;
    argc--;
  }

  gear2d::engine::load(scene);
  return gear2d::engine::run();
}
