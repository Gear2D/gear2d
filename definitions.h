#ifndef gear2d_definitions_h
#define gear2d_definitions_h

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>

/**
 * @file definitions.h
 * @author Leonardo Guilherme de Freitas
 * This file contains useful inline functions or templates,
 * like lexical cast, string splitting into vectors, typedefs,
 * etc. */

namespace gear2d {
	/** @brief Type used to identify time difference in seconds (SECONDS!) */
	typedef float timediff;
	
	/** @brief Type that defines a file name */
	typedef std::string filename;
	
	/** @brief A string to any-type converter as long as there exists a conversion 
	 ** @warning Compilation will fail if no conversion is to be found. */
	/* took idea from http://www.codeguru.com/forum/showthread.php?t=231054 and from boost */
	template <typename Target, typename Source>
	Target lexcast(Source & s) {
		std::istringstream iss(s);
		Target t;
		iss >> t;
		if (iss.fail()) std::cerr << "Conversion from(" << s << ") failed!" << std::endl;
		std::cout << "We all hate lexcast" << std::endl;
		return t;
	}
	
	/** @brief A string splitter */
	inline std::vector<std::string> split(std::string s, char d = ' ') {
		unsigned int i = 0;
		std::vector<std::string> v(std::count(s.begin(), s.end(), d)+1);
		std::stringstream iss(s);
		while (!iss.eof()) {
			std::getline(iss, v[i], d);
			i++;
		}
		return v;
	}
// 	std::vector<std::string> split(std::string s) {
// 		std::vector<std::string> tokens;
// 		std::copy(std::istream_iterator<std::string>(s),
// 			std::istream_iterator<std::string>(),
// 			std::back_inserter<std::vector<std::string> >(tokens));
// 	}

	
	
}

#endif