#ifndef LOG_H
#define LOG_H

/**
 * @file log.h
 * @author Leonardo Guilherme de Freitas
 */

#include <iostream>
#include <string>

namespace gear2d {
  
  /**
   * @class gear2d::log
   * @author Leonardo Guilherme de Freitas
   * @brief Class for logging facilities.
   * 
   * You can use this class to log things on output streams or files. Arguments
   * will be passed to operator<< in std::ostream classes.
   */
  class log {
    public:
      enum verbosity {
        minimum = 0,
        error,
        warning,
        info,
        verbose,
        maximum
      };
      
      log(const std::string & trace = std::string(), verbosity level = verbose);
      ~log();
      
      template <typename T>
      inline log & operator()(const T&, verbosity level = verbose);
      template <typename T1, typename T2>
      inline log & operator()(const T1&, const T2&, verbosity level = verbose);
      template <typename T1, typename T2, typename T3>
      inline log & operator()(const T1&, const T2&, const T3&, verbosity level = verbose);
      template <typename T1, typename T2, typename T3, typename T4>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, verbosity level = verbose);
      template <typename T1, typename T2, typename T3, typename T4, typename T5>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, const T5&, verbosity level = verbose);
      template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, verbosity level = verbose);
      
      
    public:
      static verbosity globalverb;
      
    private:
      static int indent;
      static std::ostream logstream;
      static const char * logstring[];
      
    private:
      std::string trace; /* trace string */
      verbosity level; /* level of this trace */
      bool traced; /* true if this log has been printed/traced */
      
    private:
      void mark(); /* put the "entering in" when needed */
      
  };
  
  template<typename T>
  log & log::operator() (const T & t, log::verbosity level) {
    if (globalverb < level) return *this;
    mark();
    for (int i = 0; i < indent; i++) logstream << "  ";
    logstream << logstring[level] << t << std::endl;
    return *this;
  }
  
  template <typename T1, typename T2>
  log & log::operator() (const T1 & t1, const T2 & t2, log::verbosity level) {
    if (globalverb < level) return *this;
    mark();
    for (int i = 0; i < indent; i++) logstream << "  ";
    logstream << logstring[level] << t1 << " " << t2 << std::endl;
    return *this;
  }
  
  template <typename T1, typename T2, typename T3>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, log::verbosity level) {
    if (globalverb < level) return *this;
    mark();
    for (int i = 0; i < indent; i++) logstream << "  ";
    logstream << logstring[level] << t1 << " " << t2 << " " << t3 << std::endl;
    return *this;
  }
  
  template <typename T1, typename T2, typename T3, typename T4>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, log::verbosity level) {
    if (globalverb < level) return *this;
    mark();
    for (int i = 0; i < indent; i++) logstream << "  ";
    logstream << logstring[level] << t1 << " " << t2 << " " << t3 << " " << t4 << std::endl;
    return *this;
  }
  
  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, log::verbosity level) {
    if (globalverb < level) return *this;
    mark();
    for (int i = 0; i < indent; i++) logstream << "  ";
    logstream << logstring[level] << t1 << " " << t2 << " " << t3 <<  " " << t4 << " " << t5 << std::endl;
    return *this;
  }
  
  template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, const T6 & t6, log::verbosity level) {
    if (globalverb < level) return *this;
    mark();
    for (int i = 0; i < indent; i++) logstream << "  ";
    logstream << logstring[level] << t1 << " " << t2 << " " << t3 <<  " " << t4 << " " << t5 << " " << t6 << std::endl;
    return *this;
  }
}

#ifdef _WIN32

#define logtrace(level) \
gear2d::log trace(__FUNCDNAME__, level)

#define loginfo \
gear2d::log trace(__FUNCDNAME__, gear2d::log::info)

#define logverb \
gear2d::log trace(__FUNCDNAME__, gear2d::log::verbose)

#define logerr \
gear2d::log trace(__FUNCDNAME__, gear2d::log::error)

#define logwarn \
gear2d::log trace(__FUNCDNAME__, gear2d::log::warning)

#else // NOT _WIN32

#define logtrace(level) \
gear2d::log trace(__PRETTY_FUNCTION__, level)

#define loginfo \
gear2d::log trace(__PRETTY_FUNCTION__, gear2d::log::info)

#define logverb \
gear2d::log trace(__PRETTY_FUNCTION__, gear2d::log::verbose)

#define logerr \
gear2d::log trace(__PRETTY_FUNCTION__, gear2d::log::error)

#define logwarn \
gear2d::log trace(__PRETTY_FUNCTION__, gear2d::log::warning)
#endif

#endif // LOG_H
