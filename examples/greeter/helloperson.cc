#include <iostream>
#include "gear2d.h"
using namespace gear2d;
using namespace std;
 
class helloperson : public component::base {
	public:
		// constructor and destructor
		helloperson() { }
		virtual ~helloperson() { }
		
		// greeter family
		virtual gear2d::component::family family() { return "greeter"; }
		
		// helloperson type
		virtual gear2d::component::type type() { return "helloperson"; }
		
		// setup phase, to initialize paramters and other stuff
		virtual void setup(object::signature & sig) {
			// initialize the parameter person
			// using the object signature, default
			// to "Anonymous"
			init<string>("person", sig["person"], "Anonymous");
			
			// initialize/writes the parameter "greetedtimes" to hold 0
			write("greetedtimes", 0);
		}			
		
		virtual void update(timediff dt) {
			// add 1 to the number of times we're greeted someone
			add("greetedtimes", 1);
			
			// Print how many times we've greeted someone
			std::cout << "Hello, " << read<string>("person") << "! ";
			std::cout << "I have greeted you " << read<int>("greetedtimes") << " times already!" << std::endl;
		}
};

// the build function
extern "C" { component::base * build() { return new helloperson(); } }