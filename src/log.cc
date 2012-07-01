#include "log.h"

#include <iostream>
#include <string>

using namespace gear2d;

const char * log::logstring[] = { "", "[E] ", "[W] ", "[I] ", "[M] ", "" };

log::log(const std::string & trace, log::verbosity level) 
  : trace(trace)
  , level(level)
  , traced(false) {
  
  if (globalverb < level || trace.empty()) return;
  mark();
}

void log::mark() {
  if (traced) return;
  for (int i = 0; i < indent; i++) logstream << "  ";
  logstream << "In " << trace << std::endl;
  indent++;
  traced = true;
}


log::~log() {
  if ((globalverb < level && !traced) || trace.empty()) return;
  indent--;
  for (int i = 0; i < indent; i++) logstream << "  ";
  logstream << "Leaving " << trace << std::endl;
}


std::ostream log::logstream(std::cout.rdbuf());
int log::indent = 0;
log::verbosity log::globalverb = log::error;