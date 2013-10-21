#include "log.h"

#include <iostream>
#include <string>
#include <fstream>

using namespace gear2d;

const char * log::logstring[] = { "", "E ", "W ", "I ", "" };

void log::open(const std::string & filename) {
  std::ofstream * filestream = new std::ofstream(filename, std::ofstream::out | std::ofstream::trunc);
  if (logstream != &std::cout) { logstream->flush(); delete logstream; }
  logstream = filestream;
}


log::log(const std::string & trace, const std::string & module, log::verbosity level) 
  : trace(trace)
  , tracemodule(module)
  , level(level)
  , traced(false)
  , done(true) {
  
  if (!check()) return;
  mark();
}

void log::mark() {
  if (traced || !check() || globalverb < maximum) return;
  for (int i = 0; i < indent; i++) *logstream << "  ";
  *logstream << "[ In " << tracemodule << ": " << trace << std::endl;
  indent++;
  traced = true;
  done = true;
}

bool log::check() {
  if (globalverb < level) return false; /* check if verbosity level allows */

  /* check to see if there's a filter and if this is string is in there */
  if (!filter.empty() && filter.find(tracemodule) == filter.end()) return false;

  /* check to see if module is on the ignore list */
  if (ignore.find(tracemodule) != ignore.end())
    return false;

  return true;
}



log::~log() {
  if ((globalverb < maximum && !traced) || trace.empty() || (!check())) return;
  indent--;
  for (int i = 0; i < indent; i++) *logstream << "  ";
  *logstream << "] Leaving " << tracemodule << ": " << trace << std::endl;
}

void log::module (const std::string & mod) {
  tracemodule = mod;
}



std::ostream * log::logstream = &std::cout;
int log::indent = 0;
log::verbosity log::globalverb = log::error;
std::set<std::string> log::filter;
std::set<std::string> log::ignore;
