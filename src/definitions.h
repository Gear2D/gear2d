#ifndef gear2d_definitions_h
#define gear2d_definitions_h

#include <vector>
#include <string>
#include <exception>

/**
 * @file definitions.h
 * @author Leonardo Guilherme de Freitas
 * This file contains useful inline functions or templates */

namespace gear2d {
	class object;
	namespace component { 
		class base;
		typedef void (component::base::*call)(std::string pid, component::base * lastwrite, gear2d::object * owner);
	}
	
	/** @brief Type used to identify time difference in seconds (SECONDS!) */
	typedef float timediff;
	
	/** @brief Type that defines a file name */
	typedef std::string filename;
	
	
	
	/** @brief Clamp a value between its max and min */
	template <typename T>
	void clamp(T & value, const T min, const T max) {
		if (value > max) value = max;
		if (value < min) value = min;
	}
	
	
	/**
	 * @brief Evil class
	 * This is an evil class, so you can catch the evil whenever
	 * you try good things. */
	class evil : public std::exception {
		private:
			std::string describe;
			
		public:
			evil(std::string describe = "Something evil happened!") throw() : describe(describe) { }
			virtual ~evil() throw() { };
			virtual const char* what() const throw() { return describe.c_str(); }
	};
	
/*	template<typename T> T & min(const T & a, const T & b) { return a < b ? a : b; }
	template<typename T> T & max(const T & a, const T & b) { return a > b ? a : b; }*/
}

#endif