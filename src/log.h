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
      enum loglevel {
        none = -1,
        info,
        warn,
        err
      };
        
      log(const std::string & trace = std::string(), loglevel verbosity = info);
      ~log();
      
      template <typename T>
      inline log & operator()(const T&, loglevel verbosity = info);
      template <typename T1, typename T2>
      inline log & operator()(const T1&, const T2&, loglevel verbosity = info);
      template <typename T1, typename T2, typename T3>
      inline log & operator()(const T1&, const T2&, const T3&, loglevel verbosity = info);
      template <typename T1, typename T2, typename T3, typename T4>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, loglevel verbosity = info);
      template <typename T1, typename T2, typename T3, typename T4, typename T5>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, const T5&, loglevel verbosity = info);
      template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, loglevel verbosity = info);
      
      static void output(const std::string & filename, bool append=false);
      static void output(std::ostream &);
      
      
    public:
      static loglevel level;
      
    private:
      static int indent;
      static std::ostream logstream;
      
    private:
      std::string trace;
      loglevel verbosity;
      
  };
}

#define tracelog \
  gear2d::log trace(__PRETTY_FUNCTION__, log::info);
#endif
