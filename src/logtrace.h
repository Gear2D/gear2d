#ifndef LOG_H
#define LOG_H

/**
 * @file logtrace.h
 * @author Leonardo Guilherme de Freitas
 */

#include <iostream>
#include <string>
#include <set>
#include <string>
#include <fstream>

#ifdef ANDROID
#include <android/log.h>
#endif


#if defined(_WIN32)
# if !defined(__GNUC__)
#   pragma warning(1 : 4519) /* VC++, please... */
#   pragma warning(disable : 4514)
#   pragma warning(disable : 4820);
# endif
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
  class logtrace {
    private:
      #ifdef ANDROID
      // Based on http://stackoverflow.com/questions/8870174/is-stdcout-usable-in-android-ndk
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
       * message like "[ In module: trace", if the level of this
       * message is below the global verbosity level. 
       * 
       * Every message shown after will be indented.
       * 
       * @param module Module that this block pertains to.
       * @param trace Trace string to shown in <i>"entering in"</i> messages
       * @param level Verbosity level of messages using this instance. Note that
       * you can adjust these in a per-message base.
       * 
       */
      logtrace(const std::string & module = std::string(), const std::string & trace = std::string(), verbosity level = info);
      
      /**
       * @brief Constructor for a logtrace object without a trace function.
       * 
       * When the object gets constructed, it will show in the output stream a
       * message like "[ In module", if the level of this
       * message is below the global verbosity level. 
       * 
       * Every message shown after will be indented.
       * 
       * @param module Module that this block pertains to.
       * @param level Verbosity level of messages using this instance. Note that
       * you can adjust these in a per-message base.
       * 
       */
      logtrace(const std::string & module, verbosity level);
      logtrace(logtrace::verbosity level);
      
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
      static verbosity & globalverb(); /*! global verbosity level of the logstream */
      static verbosity & globalverb(const verbosity &newverb); /*! global verbosity level of the logstream */
      static std::set<std::string> & filter(); /*! set of filter strings for module names */
      static std::set<std::string> & ignore(); /*! set of ignore filter strings for module names */
    
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
      static int & indent(); /* indent level */
      static std::ostream *& logstream(); /* associated logstream */
      char logchar[5] = { 'E', 'E', 'W', 'I', 'I' }; /* array to translate loglevels to logchars */
      
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
  
  inline logtrace::logtrace(const std::string & module, logtrace::verbosity level) : logtrace(module, "", level) { }
  inline logtrace::logtrace(logtrace::verbosity level) : logtrace("", "", level) { }
  inline logtrace::logtrace(const std::string & module, const std::string & trace, logtrace::verbosity level)
  : trace(trace)
  , tracemodule(module)
  , level(level)
  , traced(false)
  , done(true) {
    
    if (!check()) return;
    mark();
  }
  
  inline void logtrace::mark() {
    if (traced || !check() || globalverb() < maximum || tracemodule.empty()) return;
    for (int i = 0; i < indent(); i++) *logstream() << "  ";
    *logstream() << "[ In " << tracemodule << (trace.empty() ? "" : ": ") << trace << std::endl;
    indent()++;
    traced = true;
    done = true;
  }
  
  inline bool logtrace::check() {
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
  
  
  
  inline logtrace::~logtrace() {
    if ((globalverb() < maximum && !traced) || tracemodule.empty() || (!check())) return;
    indent()--;
    for (int i = 0; i < indent(); i++) *logstream() << "  ";
    *logstream() << "] Leaving " << tracemodule << (trace.empty() ? "" : ": ") << trace << std::endl;
  }
  
  inline void logtrace::module (const std::string & mod) {
    tracemodule = mod;
  }
  
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
    *logstream() << std::endl;
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
      for (int i = 0; i < indent(); i++) *logstream() << "  ";
      *logstream() << logchar[level] << ' ' << tracemodule << ": ";
      done = false;
    }
    *logstream() << t << " ";
    operator()(vs...);
    
#endif
    return *this;
  }

#ifdef LOGTRACE

#if defined(__GNUC__)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name */
#define loginfo \
logtrace trace("", __PRETTY_FUNCTION__, logtrace::info)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name */
#define logerr \
logtrace trace("", __PRETTY_FUNCTION__, logtrace::error)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name */
#define logwarn \
logtrace trace("", __PRETTY_FUNCTION__, logtrace::warning)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modwarn(a) \
logtrace trace(a, __PRETTY_FUNCTION__, logtrace::warning)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define moderr(a) \
logtrace trace(a, __PRETTY_FUNCTION__, logtrace::error)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modinfo(a) \
logtrace trace(a, __PRETTY_FUNCTION__, logtrace::info)

#elif defined (_WIN32)

/*! Create a trace object to log info messages and initializes
 * the trace string to the function name */
#define loginfo \
logtrace trace("", __FUNCTION__, logtrace::info)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name */
#define logerr \
logtrace trace("", __FUNCTION__, logtrace::error)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name */
#define logwarn \
logtrace trace("", __FUNCTION__, logtrace::warning)

/*! Create a trace object to log warning messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modwarn(a) \
logtrace trace(a, __FUNCTION__, logtrace::warning)

/*! Create a trace object to log error messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define moderr(a) \
logtrace trace(a, __FUNCTION__, logtrace::error)

/*! Create a trace object to log informational messages and initializes
 * the trace string to the function name and the module a
 * @param a Module name */
#define modinfo(a) \
logtrace trace(a, __FUNCTION__, logtrace::info)

#endif

#else //NO LOGTRACE

#define logtrace(a) \
  logtrace trace
  
#define loginfo(a) \
  logtrace trace
  
#define logverb(a) \
  logtrace trace
  
#define logerr(a) \
  logtrace trace
  
#define logwarn(a) \
  logtrace trace
    
#define modwarn(a) \
  logtrace trace
  
#define moderr(a) \
  logtrace trace
  
#define modinfo(a) \
logtrace trace  
  
#endif //LOGTRACE

#endif // LOG_H
