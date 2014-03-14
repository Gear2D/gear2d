#include "log.h"

#include <iostream>
#include <string>
#include <fstream>

const char * logtrace::logstring[] = { "", "E ", "W ", "I ", "" };
#ifdef ANDROID
// Based on http://stackoverflow.com/questions/8870174/is-stdcout-usable-in-android-ndk
#include <android/log.h>
class androidbuf: public std::streambuf {
public:
    enum { bufsize = 128 };
    androidbuf() { this->setp(buffer, buffer + bufsize - 1); }
private:
    int overflow(int c) {
        if (c == traits_type::eof()) {
            *this->pptr() = traits_type::to_char_type(c);
            this->sbumpc();
        }
        return this->sync()? traits_type::eof(): traits_type::not_eof(c);
    }
    int sync() {
        int rc = 0;
        if (this->pbase() != this->pptr()) {
            __android_log_print(ANDROID_LOG_INFO,
                               "Native",
                               "%s",
                               std::string(this->pbase(),
                                           this->pptr() - this->pbase()).c_str());
            rc = 0;
            this->setp(buffer, buffer + bufsize - 1);
        }
        return rc;
    }
    char buffer[bufsize];
};

static void initandroidlog() {
  static bool initialized = false;
  if (!initialized) {
    std::cout.rdbuf(new androidbuf);
    initialized = true;
  }
}
#endif

void logtrace::open(const std::string & filename) {
  std::ofstream * filestream = new std::ofstream(filename, std::ofstream::out | std::ofstream::trunc);
  if (logstream != &std::cout) { logstream->flush(); delete logstream; }
  logstream = filestream;
}

logtrace::logtrace(const std::string & trace, const std::string & module, logtrace::verbosity level) 
  : trace(trace)
  , tracemodule(module)
  , level(level)
  , traced(false)
  , done(true) {
  
  if (!check()) return;
  mark();
}

void logtrace::mark() {
  if (traced || !check() || globalverb < maximum) return;
  for (int i = 0; i < indent; i++) *logstream << "  ";
  *logstream << "[ In " << tracemodule << ": " << trace << std::endl;
  indent++;
  traced = true;
  done = true;
}

bool logtrace::check() {
  if (globalverb < level) return false; /* check if verbosity level allows */

  /* check to see if there's a filter and if this is string is in there */
  if (!filter.empty() && filter.find(tracemodule) == filter.end()) return false;

  /* check to see if module is on the ignore list */
  if (ignore.find(tracemodule) != ignore.end())
    return false;

#ifdef ANDROID
   initandroidlog();
#endif

  return true;
}



logtrace::~logtrace() {
  if ((globalverb < maximum && !traced) || trace.empty() || (!check())) return;
  indent--;
  for (int i = 0; i < indent; i++) *logstream << "  ";
  *logstream << "] Leaving " << tracemodule << ": " << trace << std::endl;
}

void logtrace::module (const std::string & mod) {
  tracemodule = mod;
}



std::ostream * logtrace::logstream = &std::cout;
int logtrace::indent = 0;
logtrace::verbosity logtrace::globalverb = logtrace::error;
std::set<std::string> logtrace::filter;
std::set<std::string> logtrace::ignore;
