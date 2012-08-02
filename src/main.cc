#include "gear2d.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

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
      if (arg[1] == 'l') {
        int level = atoi(arg+2);
        if (level <= gear2d::log::minimum) level = gear2d::log::minimum+1;
        if (level > gear2d::log::maximum) level = gear2d::log::maximum;
        gear2d::log::globalverb = (gear2d::log::verbosity)level;
      } else if (arg[1] == 'f') {
        gear2d::log::filter = (arg+2);
      }
    }
    else scene = arg;
    argc--;
  }

  gear2d::engine::load(scene);
  return gear2d::engine::run();
}
