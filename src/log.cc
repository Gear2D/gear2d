#include "log.h"

using namespace gear2d;

const char * logstring[] = { "information: ", "warning: ", "error: " };

log::log(const std::string & trace, log::loglevel verbosity) 
  : trace(trace)
  , verbosity(verbosity) {
  
  if (verbosity < level) return;
    logstream << "Entering " << trace << std::endl;
}

log::~log() {
  if (verbosity < level) return;
  logstream << "Leaving " << trace << std::endl;
}

template<typename T>
log & log::operator() (const T & t, log::loglevel verbosity) {
  logstream << logstring[verbosity] << t << std::endl;
  
}

template <typename T1, typename T2>
log & log::operator() (const T1 & t1, const T2 & t2, log::loglevel verbosity) {
  logstream << logstring[verbosity] << t1 << " " << t2 << std::endl;
}

template <typename T1, typename T2, typename T3>
log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, log::loglevel verbosity) {
  logstream << logstring[verbosity] << t1 << " " << t2 << " " << t3 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4>
log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, log::loglevel verbosity) {
  logstream << logstring[verbosity] << t1 << " " << t2 << " " << t3 << " " << t4 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, log::loglevel verbosity) {
  logstream << logstring[verbosity] << t1 << " " << t2 << " " << t3 <<  " " << t4 << " " << t5 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, const T6 & t6, log::loglevel verbosity) {
  logstream << logstring[verbosity] << t1 << " " << t2 << " " << t3 <<  " " << t4 << " " << t5 << " " << t6 << std::endl;
}

void log::output (const std::string & filename, bool append) {
  
}

void log::output (std::ostream &) {

}



std::ostream log::logstream(std::cout.rdbuf());
int log::indent = 0;
log::loglevel log::level = log::info;