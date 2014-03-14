#ifndef LOG_H
#define LOG_H

/**
 * @file log.h
 * @author Leonardo Guilherme de Freitas
 */

#include <iostream>
#include <string>
#include "definitions.h"

#if defined(_WIN32)
# if !defined(__GNUC__)
#   pragma warning(1 : 4519) /* VC++, please... */
#   pragma warning(disable : 4514)
#   pragma warning(disable : 4820);
# endif
#   if defined(logtrace_EXPORTS) /* defined by cmake, thanks god. */
#       define  logtraceapi  __declspec(dllexport) 
#   else
#       define  logtraceapi  __declspec(dllimport) 
#   endif
#else
#   define logtraceapi
#endif

  /**
   * @class logtrace
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
   * There are macros defined to make the object creation easy
   * 
   * Example:
   * 
   * @code
   * int main(int argc, char ** argv) {
   *    logtrace trace("main") // creates an object called 'trace' to trace the main() function
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
  class logtraceapi logtrace {
    public:
      
      /**
       * @enum verbosity
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
       * @brief Constructor for a logtrace object.
       * 
       * When the object gets constructed, it will show in the output stream a
       * message like "(module) entering in &lt;trace&gt;", if the level of this
       * message is below the global verbosity level. 
       * 
       * Every message shown after will be indented.
       * 
       * @param trace Trace string to shown in <i>"entering in"</i> messages
       * @param module Module that this block pertains to.
       * @param level Verbosity level of messages using this instance. Note that
       * you can adjust these in a per-message base.
       * 
       */
      logtrace(const std::string & trace = std::string(), const std::string & module = std::string(), verbosity level = info);
      
      /**
       * @brief Destructor for a logtrace object.
       * 
       * When a logtrace object gets destroyed, it will show a message like "leaving
       * &lt;trace&gt;".
       */
      ~logtrace();
      
      /**
       * @brief Defines the current module of this trace object.
       * 
       * Call this if you want to change the module that is shown in messages
       * 
       * @param module Module name for this block
       */
      void module(const std::string & module);
      
      /**
       * @brief Logs a message in the information verbosity level.
       * 
       * This is useful if you created your trace object with another loglevel
       * and want to change it just for this message in particular */
      template <typename... Ts>
      inline logtrace & i(const Ts&... vs);

      /**
       * @brief Logs a message in the error verbosity level.
       * 
       * This is useful if you created your trace object with another loglevel
       * and want to change it just for this message in particular */
      template <typename... Ts>
      inline logtrace & e(const Ts&... vs);
      
      /**
       * @brief Logs a message in the warning verbosity level.
       * 
       * This is useful if you created your trace object with another loglevel
       * and want to change it just for this message in particular */      
      template <typename... Ts>
      inline logtrace & w(const Ts&... vs);

      inline logtrace & operator()(void);
       
      template <typename... Ts>
      inline logtrace & operator() (const Ts&... vs);
      
      template <typename T, typename... Ts>
      inline logtrace & operator()(const T & t1, const Ts&... vs);
      
    public:
      static verbosity globalverb; /*! global verbosity level of the logstream */
      static std::set<std::string> filter; /*! set of filter strings for module names */
      static std::set<std::string> ignore; /*! set of ignore filter strings for module names */
    
    public:
      /**
       * @brief Open a file to save the log messages to.
       * 
       * You may log messages to a file by passing the file address as a
       * parameter. Note that the file will be truncated if it exists.
       * 
       * @param filename The name of the file to save messages to.
       */
      static void open(const std::string & filename);
      
    private:
      static int indent; /* indent level */
      static std::ostream * logstream; /* associated logstream */
      static const char * logstring[]; /* array to translate loglevels to logstrings */
      
    private:
      std::string trace; /* trace string */
      std::string tracemodule; /* module string */
      verbosity level; /* level of this trace */
      bool traced; /* true if this logtrace has been printed/traced */
      bool done; /* true if line has ended */
      
    private:
      bool check(); /* check if it can logtrace */
      void mark(); /* put the "entering in" when needed */
  };
  
  template <typename... Ts>
  inline logtrace & logtrace::i(const Ts&... vs) {
#ifdef LOGTRACE
    auto previous = level;
    level = logtrace::info;
    (*this)(vs...);
    level = previous;
#endif
    return *this;
  }
  
  template <typename... Ts>
  inline logtrace & logtrace::w(const Ts&... vs) {
#ifdef LOGTRACE
    auto previous = level;
    level = logtrace::warning;
    (*this)(vs...);
    level = previous;
#endif
    return *this;
  }
  
  template <typename... Ts>
  inline logtrace & logtrace::e(const Ts&... vs) {
#ifdef LOGTRACE
    auto previous = level;
    level = logtrace::error;
    (*this)(vs...);
    level = previous;
#endif
    return *this;
  }
  
  logtrace & logtrace::operator()(void) {
#ifdef LOGTRACE
    *logstream << std::endl;
    done = true;
#endif
    return *this;
  }  

  template<typename T, typename... Ts>
  logtrace & logtrace::operator() (const T & t, const Ts&... vs) {
#ifdef LOGTRACE 
    if (!check()) return *this;
    mark();
    if (done) {
      for (int i = 0; i < indent; i++) *logstream << "  ";
      *logstream << logstring[level] << tracemodule << ": ";
      done = false;
    }
    *logstream << t << " ";
    operator()(vs...);
    
#endif
    return *this;
  }

#ifdef LOGTRACE

#if defined(__GNUC__)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name */
#define loginfo \
logtrace trace(__PRETTY_FUNCTION__, "", logtrace::info)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name */
#define logerr \
logtrace trace(__PRETTY_FUNCTION__, "", logtrace::error)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name */
#define logwarn \
logtrace trace(__PRETTY_FUNCTION__, "", logtrace::warning)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modwarn(a) \
logtrace trace(__PRETTY_FUNCTION__, a, logtrace::warning)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define moderr(a) \
logtrace trace(__PRETTY_FUNCTION__, a, logtrace::error)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modinfo(a) \
logtrace trace(__PRETTY_FUNCTION__, a, logtrace::info)

#elif defined (_WIN32)

/*! Create a trace object to log info messages and initializes
 * the trace string to the function name */
#define loginfo \
logtrace trace(__FUNCTION__, "", logtrace::info)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name */
#define logerr \
logtrace trace(__FUNCTION__, "", logtrace::error)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name */
#define logwarn \
logtrace trace(__FUNCTION__, "", logtrace::warning)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modwarn(a) \
logtrace trace(__FUNCTION__, a, logtrace::warning)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define moderr(a) \
logtrace trace(__FUNCTION__, a, logtrace::error)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modinfo(a) \
logtrace trace(__FUNCTION__, a, logtrace::info)

#endif

#else //NO LOGTRACE

#define logtrace(a) \
  logtrace trace
  
#define loginfo \
  logtrace trace
  
#define logverb \
  logtrace trace
  
#define logerr \
  logtrace trace
  
#define logwarn \
  logtrace trace
    
#define modwarn \
  logtrace trace
  
#define moderr \
  logtrace trace
  
#define modinfo \
logtrace trace  

#define modwarn \
logtrace trace
  
  
#endif //LOGTRACE

#endif // LOG_H
