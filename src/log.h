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
      log(const std::string & trace, int verbosity = 0);
      ~log();
      
      template <typename T>
      inline log & operator()(const T&, int verbosity = -1);
      template <typename T1, typename T2>
      inline log & operator()(const T1&, const T2&, int verbosity = -1);
      template <typename T1, typename T2, typename T3>
      inline log & operator()(const T1&, const T2&, const T3&, int verbosity = -1);
      template <typename T1, typename T2, typename T3, typename T4>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, int verbosity = -1);
      template <typename T1, typename T2, typename T3, typename T4, typename T5>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, const T5&, int verbosity = -1);
      template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
      inline log & operator()(const T1&, const T2&, const T3&, const T4&, const T5&, const T6&, int verbosity = -1);
      
      static void output(const std::string & filename, bool append=false);
      static void output(std::ostream &);
      
    public:
      static int verbosity;
      
    private:
      static int indent;
      static std::ostream logstream;
  };
}
#endif
