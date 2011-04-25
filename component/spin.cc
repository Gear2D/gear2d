#include "gear2d.h"
#include <string>
#include <cmath>
using namespace std;

/**
 * @file spin.cc
 * This is a spin component that changes x and y based
 * on a spin speed and a radius (everything on radians). */


using namespace gear2d;

gear2d::pbase::table paramtemplate;

class spin : public component::base {
	private:
		float angle;
		
	public:
		spin() : angle(0) {
		};
		
		virtual ~spin() { }
		virtual component::type type() { return "spin"; }
		
		virtual void init() {
		}
		
		virtual const pbase::table & parameters() { return paramtemplate; }
		virtual void update(timediff dt) {
			angle += access<float>("spinspeed").get() * dt;
			float & x = access<float>("x");
			x += cos(angle) * access<float>("spinradius").get();
			float & y = access<float>("y");
			y += sin(angle) * access<float>("spinradius").get();
		}
};

extern "C" {
	int initialized = 0;
	component::base * build() {
		if (!initialized) {
			paramtemplate["x"] = new parameter<float>(0);
			paramtemplate["y"] = new parameter<float>(0);
			paramtemplate["spinspeed"] = new parameter<float>(0);
			paramtemplate["spinradius"] = new parameter<float>(0);
			initialized = 1;
		}
		return new spin;
	}
}

