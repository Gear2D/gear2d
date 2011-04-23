#include "gear2d.h"
#include <string>
using namespace std;

/**
 * @file dummy.cc
 * This is a dummy component to show
 * how to build external components.
 * 
 * Components must export a build function
 * in C linkage (no name mangling) to be registered
 * in the engine. */


using namespace gear2d;

gear2d::pbase::table paramtemplate;

class dummy : public component::base
		, public pbase::listener<string>
		, public pbase::listener<float> {
	public:
		dummy() {
		};
		
		virtual ~dummy() { }
		virtual component::type type() { return "dummy"; }
		
		virtual void init() {
			access<float>("xibiu").hook(*this);
			access<string>("test").hook(*this);
		}
		
		virtual void handle(pbase::id pid, const float & o, const float & n) {
			std::cout << pid << ": change from " << o << " to " << n << std::endl;
		}
		
		virtual void handle(pbase::id pid, const string & o, const string & n) {
			std::cout << pid << ": change from " << o << " to " << n << std::endl;
		}

		
		virtual const pbase::table & parameters() { return paramtemplate; }
		virtual void update(timediff dt) {
// 			access<float>("xibiu") = dt;
		}
};

extern "C" {
	int initialized = 0;
	component::base * build() {
		if (!initialized) {
			paramtemplate["test"] = new parameter<string>("fuuuka fuka mucha spaci");
			paramtemplate["xibiu"] = new parameter<float>;
			paramtemplate["xibiu"]->set("1234.5");
			initialized = 1;
		}
		return new dummy;
	}
}

