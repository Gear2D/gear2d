#include "gear2d.h"

/**
 * @file spatial.cc
 * @author Leonardo Guilherme de Freitas
 * Spatial component that handles and process positioning,
 * size and scale of objects
 * 
 * Parameters:
 */

using namespace gear2d;
using namespace std;

gear2d::pbase::table ptemplate;

class spatial
		: public component::base {
	public:
		spatial() {
		};
		
		virtual ~spatial() { }
		virtual component::type type() { return "spatial"; }
		virtual void init() { }
		virtual const pbase::table & parameters() { return ptemplate; }
		virtual void update(timediff dt) { }
};

extern "C" {
	int initialized = 0;
	component::base * build() {
		if (!initialized) {
			ptemplate["x"] = new parameter<float>(0.0f);
			ptemplate["y"] = new parameter<float>(0.0f);
			ptemplate["z"] = new parameter<float>(0.0f);
			
			ptemplate["w"] = new parameter<float>(0.0f);
			ptemplate["h"] = new parameter<float>(0.0f);
			ptemplate["d"] = new parameter<float>(0.0f);
			initialized = 1;
		}
		return new spatial;
	}
}