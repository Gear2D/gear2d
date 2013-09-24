#ifndef LOG_H
#define LOG_H

/**
 * @file log.h
 * @author Leonardo Guilherme de Freitas
 */

#include <iostream>
#include <string>
#include "definitions.h"

namespace gear2d {
  
  /**
   * @class gear2d::log
   * @author Leonardo Guilherme de Freitas.
   * 
   * @brief Class for logging facilities.
   * 
   * You can use this class to log things on output streams or files. Arguments
   * will be passed to operator<< in std::ostream classes.
   * 
   * To use it, you should create an instance of this class in the beginning
   * of a block you want to trace and give it a trace string and a module name.
   * After that you can use the () operator to log messages and variables.
   * 
   * There are macros defined to make the object creation less painful.
   * 
   * Example:
   * 
   * @code
   * int main(int argc, char ** argv) {
   *    gear2d::log trace("main") // creates an object called 'trace' to trace the main() function
   *    trace("Inside main. Number of arguments:", argc);
   *    return 0;
   * }
   * @endcode
   * 
   * This would output:
   * 
   * @code
   * 
   * Entering in main
   *    Inside main. Number of arguments: 1
   * Leaving main
   * 
   * @endcode
   * 
   */
  class g2dapi log {
    public:
      
      /**
       * @enum log::verbosity
       * @author Leonardo Guilherme de Freitas
       * @brief Verbosity level, the lowest, the less will be shown.
       */
      enum verbosity {
        minimum = 0, /*! minimum verbosity - silent. */
        error,       /*! only critical messages and errors */
        warning,     /*! error messages and warnings */
        info,        /*! errors, warnings and informational */
        maximum      /*! maximum verbosity possible */
      };
      
      /**
       * @brief Constructor for a log object.
       * 
       * When the object gets constructed, it will show in the output stream a
       * message like "(module) entering in &lt;trace%gt;", if the level of this
       * message is below the global verbosity level. 
       * 
       * Every message shown after will be indented.
       * 
       * 
       * @param trace Trace string to shown in <i>"entering in"</i> messages
       * @param module Module that this block pertains to.
       * @param level Verbosity level of messages using this instance. Note that
       * you can adjust these in a per-message base.
       * 
       */
      log(const std::string & trace = std::string(), const std::string & module = std::string(), verbosity level = info);
      
      /**
       * @brief Destructor for a log object.
       * 
       * When a log object gets destroyed, it will show a message like "leaving
       * &lt;trace&gt;".
       */
      ~log();
      
      void module(const std::string & module);
      
      /**
       * @brief Put on the output stream a message with one argument.
       * @param level Verbosity level of this message 
       */
      template <typename T>
      inline log & operator()(const T& t1, verbosity level = info);
      
      /**
       * @brief Put on the output stream a message with two arguments.
       * @param level Verbosity level of this message 
       */
      template <typename T1, typename T2>
      inline log & operator()(const T1& t1, const T2& t2, verbosity level = info);
      
      /**
       * @brief Put on the output stream a message with three arguments.
       * @param level Verbosity level of this message 
       */
      template <typename T1, typename T2, typename T3>
      inline log & operator()(const T1& t1, const T2& t2, const T3& t3, verbosity level = info);
      
      /**
       * @brief Put on the output stream a message with four arguments.
       * @param level Verbosity level of this message 
       */
      template <typename T1, typename T2, typename T3, typename T4>
      inline log & operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4, verbosity level = info);
      
      /**
       * @brief Put on the output stream a message with five arguments.
       * @param level Verbosity level of this message 
       */
      template <typename T1, typename T2, typename T3, typename T4, typename T5>
      inline log & operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, verbosity level = info);
      
      /**
       * @brief Put on the output stream a message with six arguments.
       * @param level Verbosity level of this message 
       */
      template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
      inline log & operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, verbosity level = info);
      
    public:
      static verbosity globalverb; /*! global verbosity level of the logstream */
      static std::set<std::string> filter; /*! set of filter strings for module names */
      static std::set<std::string> ignore; /*! set of ignore filter strings for module names */
    
    public:
      static void open(const std::string & filename);
      
    private:
      static int indent;
      static std::ostream * logstream;
      static const char * logstring[];
	  
      
    private:
      std::string trace; /* trace string */
      std::string tracemodule; /* module string */
      verbosity level; /* level of this trace */
      bool traced; /* true if this log has been printed/traced */
      
    private:
      bool check(); /* check if it can log */
      void mark(); /* put the "entering in" when needed */
      
  };
  
  template<typename T>
  log & log::operator() (const T & t, log::verbosity level) {
#ifdef LOGTRACE 
    if (!check()) return *this;
    mark();
    for (int i = 0; i < indent; i++) *logstream << "  ";
    *logstream << logstring[level] << tracemodule << ": " << t << std::endl;
#endif
    return *this;
  }
  
  template <typename T1, typename T2>
  log & log::operator() (const T1 & t1, const T2 & t2, log::verbosity level) {
#ifdef LOGTRACE
    if (!check()) return *this;
    mark();
    for (int i = 0; i < indent; i++) *logstream << "  ";
    *logstream << logstring[level] << tracemodule << ": " << t1 << " " << t2 << std::endl;
#endif
    return *this;
  }
  
  template <typename T1, typename T2, typename T3>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, log::verbosity level) {
#ifdef LOGTRACE
    if (!check()) return *this;
    mark();
    for (int i = 0; i < indent; i++) *logstream << "  ";
    *logstream << logstring[level] << tracemodule << ": " << t1 << " " << t2 << " " << t3 << std::endl;
#endif
    return *this;
  }
  
  template <typename T1, typename T2, typename T3, typename T4>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, log::verbosity level) {
#ifdef LOGTRACE
    if (!check()) return *this;
    mark();
    for (int i = 0; i < indent; i++) *logstream << "  ";
    *logstream << logstring[level] << tracemodule << ": " << t1 << " " << t2 << " " << t3 << " " << t4 << std::endl;
#endif
    return *this;
  }
  
  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, log::verbosity level) {
#ifdef LOGTRACE
    if (!check()) return *this;
    mark();
    for (int i = 0; i < indent; i++) *logstream << "  ";
    *logstream << logstring[level] << tracemodule << ": " << t1 << " " << t2 << " " << t3 <<  " " << t4 << " " << t5 << std::endl;
#endif
    return *this;
  }
  
  template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  log & log::operator() (const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, const T6 & t6, log::verbosity level) {
#ifdef LOGTRACE
    if (!check()) return *this;
    mark();
    for (int i = 0; i < indent; i++) *logstream << "  ";
    *logstream << logstring[level] << tracemodule << ": " << t1 << " " << t2 << " " << t3 <<  " " << t4 << " " << t5 << " " << t6 << std::endl;
#endif
    return *this;
  }
}

/**
 * \def LOGTRACE
 * Macro used to activate or deactivate generation of logging code. Define it
 * to activate logging facilities.
 */
#ifdef LOGTRACE

#if defined(__GNUC__)


/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name */
#define loginfo \
gear2d::log trace(__PRETTY_FUNCTION__, "", gear2d::log::info)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name */
#define logerr \
gear2d::log trace(__PRETTY_FUNCTION__, "", gear2d::log::error)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name */
#define logwarn \
gear2d::log trace(__PRETTY_FUNCTION__, "", gear2d::log::warning)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modwarn(a) \
gear2d::log trace(__PRETTY_FUNCTION__, a, gear2d::log::warning)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define moderr(a) \
gear2d::log trace(__PRETTY_FUNCTION__, a, gear2d::log::error)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modinfo(a) \
gear2d::log trace(__PRETTY_FUNCTION__, a, gear2d::log::info)

#elif defined (_WIN32)

/*! Create a trace object to log info messages and initializes
 * the trace string to the function name */
#define loginfo \
gear2d::log trace(__FUNCTION__, "", gear2d::log::info)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name */
#define logerr \
gear2d::log trace(__FUNCTION__, "", gear2d::log::error)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name */
#define logwarn \
gear2d::log trace(__FUNCTION__, "", gear2d::log::warning)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modwarn(a) \
gear2d::log trace(__FUNCTION__, a, gear2d::log::warning)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define moderr(a) \
gear2d::log trace(__FUNCTION__, a, gear2d::log::error)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modinfo(a) \
gear2d::log trace(__FUNCTION__, a, gear2d::log::info)

#endif

#else //NO LOGTRACE

#define logtrace(a) \
  gear2d::log trace
  
#define loginfo \
  gear2d::log trace
  
#define logverb \
  gear2d::log trace
  
#define logerr \
  gear2d::log trace
  
#define logwarn \
  gear2d::log trace
    
#define modwarn \
  gear2d::log trace
  
#define moderr \
  gear2d::log trace
  
#define modinfo \
gear2d::log trace  

#define modwarn \
gear2d::log trace
  
  
#endif //LOGTRACE

#endif // LOG_H
