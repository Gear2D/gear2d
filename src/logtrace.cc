#include "logtrace.h"

#ifdef ANDROID
void logtrace::initandroidlog() {
  static bool initialized = false;
  if (!initialized) {
    std::cout.rdbuf(new androidbuf);
    initialized = true;
  }
}
#endif 

/* logtrace static calls */
std::ostream *& logtrace::logstream() {
  static std::ostream * stream = &std::cout;
  return stream;
}

int & logtrace::indent() {
  static int i = 0;
  return i;
}

logtrace::verbosity & logtrace::globalverb() {
  static logtrace::verbosity verb = logtrace::error;
  return verb;
}

logtrace::verbosity & logtrace::globalverb(const verbosity & newverb) {
  globalverb() = newverb;
  return globalverb();
}


std::set<std::string> & logtrace::filter() {
  static std::set<std::string> filters;
  return filters;
}

std::set<std::string> & logtrace::ignore() {
  static std::set<std::string> ignores;
  return ignores;
}

void logtrace::open(const std::string & filename) {
  std::ofstream * filestream = new std::ofstream(filename, std::ofstream::out | std::ofstream::trunc);
  if (logstream() != &std::cout) { logstream()->flush(); delete logstream(); }
  logstream() = filestream;
}

logtrace::logtrace(const std::string & module, logtrace::verbosity level) : logtrace(module, "", level) { }
logtrace::logtrace(logtrace::verbosity level) : logtrace("", "", level) { }
logtrace::logtrace(const std::string & module, const std::string & trace, logtrace::verbosity level)
  : trace(trace)
  , tracemodule(module)
  , level(level)
  , traced(false)
  , done(true) {
    
    if (!check()) return;
    mark();
}

 bool logtrace::check() {
  if (globalverb() < level) return false; /* check if verbosity level allows */
    
    /* check to see if there's a filter and if this is string is in there */
    if (!filter().empty() && filter().find(tracemodule) == filter().end()) return false;
    
    /* check to see if module is on the ignore list */
    if (ignore().find(tracemodule) != ignore().end())
      return false;
    
  #ifdef ANDROID
    initandroidlog();
  #endif
  
  return true;
}

