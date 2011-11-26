/**
 * @file space2d.cc
 * @b type space2d
 * @b family spatial
 * @author Leonardo Guilherme de Freitas
 * @addtogroup spatial
 * Spatial component that handles and process positioning,
 * size and scale of objects
 * 
 * Parameters:
 */

#include "gear2d.h"
using namespace gear2d;

#include <iostream>
using namespace std;

class spatial
		: public component::base {
	public:
		spatial() {
		};
		
		virtual ~spatial() { }
		virtual component::type type() { return "space2d"; }
		virtual component::family family() { return "spatial"; }
		virtual void setup(object::signature & sig) {
			init("x", sig["x"], 0.0f);
			init("y", sig["y"], 0.0f);
			init("z", sig["z"], 0.0f);
			init("w", sig["w"], 0.0f);
			init("h", sig["h"], 0.0f);
			init("d", sig["d"], 0.0f);
		}			
		
		virtual void update(timediff dt) {
		}
};

extern "C" {
	int initialized = 0;
	component::base * build() {
		if (!initialized) {
			initialized = 1;
		}
		return new spatial;
	}
}
