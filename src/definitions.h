#ifndef gear2d_definitions_h
#define gear2d_definitions_h

#include <vector>
#include <set>
#include <list>
#include <exception>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>


/**
 * @author Leonardo Guilherme de Freitas
 */

namespace gear2d {
	class object;
	namespace component { 
		class base;
		
		/**
		 * @brief Definition of a parameter handler.
		 * 
		 * This typedef specifies the signature for a component member function
		 * pointer to be called when you hook to a parameter. Using this specification
		 * you can create custom callbacks other than handle(). 
		 * 
		 * @warning Note that even complying to this signature, you still need
		 * to cast your callback to component::base::call when hooking it.
		 * 
		 * @code
		 * class test : public component::base {
		 *   public:
		 *      // example of callback
		 *      virtual void xcallback(std::string pid, gear2d::component::base * lastwrite, gear2d::object * owner);
		 *   public:
		 *      setup() {
		 *        // example on how to register it
		 *        hook("x", (component::base::call)&xcallback);
		 *      }
		 * };
		 * @endcode
		 */
		typedef void (component::base::*call)(std::string pid, component::base * lastwrite, gear2d::object * owner);
	}
	
	/** @brief Type used to identify time difference in seconds (SECONDS!). */
	typedef float timediff;
	
	/** @brief Type that defines a file name. */
	typedef std::string filename;
	
	
	
	/** @brief Clamp a value between its max and min. */
	template <typename T>
	void clamp(T & value, const T min, const T max) {
		if (value > max) value = max;
		if (value < min) value = min;
	}
	
	/** @brief return the max value between x and y */
	template <typename T>
	int max(T & x, T & y) {
		if (x < y) return x;
		else return y;
	}
	
	/**
	 * @brief Evil class.
	 * 
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
	
	/**
	 * @brief String splitter to any container.
	 * @param elems Target element container.
	 * @param s String to be splitted.
	 * @param delim Delimiting char.
	 * @return elems
	 * 
	 * Uses strinstream and getline to split a string using the given delimiter.
	 */
	template <typename container_t>
	container_t & split(container_t & elems, const std::string & s, char delim) {
		std::stringstream ss(s);
		std::string item;
		std::insert_iterator<container_t> it(elems, elems.begin());
		while(std::getline(ss, item, delim)) {
			*it = item;
			it++;
		}
		return elems;
	}
	
	/**
	 * @brief Handy shortcut for the split function.
	 * @param s String to be splitted.
	 * @param delim Delimiting char.
	 * @return Instance of the container
	 */
	template <typename container_t>
	container_t split(const std::string & s, char delim) {
		container_t elems;
		return split(s, delim, elems);
	}
	
	/**
	 * @brief Evaluate a string to a type
	 * @param raw Raw string to be evaluated
	 * @param def Default if raw is empty
	 * 
	 * This tries to do a lexical cast to the raw string to the
	 * destination (datatype) type.
	 * 
	 * You can (and should!) do a template specialization of this method
	 * if you know better than this function.
	 * @code
	 * template<> eval<your-fancy-type>(std::string raw, your-fancy-type def) {
	 *     // do your stuff to convert raw string to your-fancy-type
	 *     // and then return it
	 * }
	 * @endcode
	 * 
	 * The default works very well for basic types.
	 * 
	 * @warning The lexical cast may fail if converting
	 * from string to some of the basic types like float
	 * or int if raw is empty. Check your arguments.
	 */
	template<typename datatype>
	datatype eval(std::string raw, datatype def) {
		std::stringstream sstr;
		datatype t;
		sstr << raw;
		sstr >> t;
		if (sstr.fail()) t = def;
		return t;
	}
}
#endif