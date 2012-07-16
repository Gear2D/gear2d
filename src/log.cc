#include "log.h"

#include <iostream>
#include <string>

using namespace gear2d;

const char * log::logstring[] = { "", "E ", "W ", "I ", "" };

log::log(const std::string & trace, const std::string & module, log::verbosity level) 
  : trace(trace)
  , tracemodule(module.empty() ? "" : module + ": ")
  , level(level)
  , traced(false) {
  
  if (globalverb < level || trace.empty()) return;
  mark();
}

void log::mark() {
  if (traced || globalverb < info) return;
  for (int i = 0; i < indent; i++) logstream << "  ";
  logstream << "{ In " << tracemodule << trace << std::endl;
  indent++;
  traced = true;
}


log::~log() {
  if ((globalverb < info && !traced) || trace.empty()) return;
  indent--;
  for (int i = 0; i < indent; i++) logstream << "  ";
  logstream << "} Leaving " << tracemodule << trace << std::endl;
}

void log::module (const std::string & mod) {
  tracemodule = mod.empty() ? "" : mod + ": ";
}



std::ostream log::logstream(std::cout.rdbuf());
int log::indent = 0;
log::verbosity log::globalverb = log::error;