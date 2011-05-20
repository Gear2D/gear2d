/**
 * @file kinetic2d.cc
 * @addtogroup kinetics
 * @b family kinetics
 * @b type kinetic2d
 * 
 * This components apply acceleration, speed, rotational speed and
 * such to any spatial component (this is a 2d basic kinetic simulation)
 *
 * @b depends
 * @li spatial
 * 
 * @b using
 * @li @c x: x position where the result of transformation will be stored (from spatial) @b float
 * @li @c y: y position where the result of transformation will be stored (from spatial) @b float
 * 
 * @b provides
 * @li @c x.accel Acceleration for the x axis @b float
 * @li @c y.accel Acceleration for the y axis @b float
 * @li @c x.speed Speed for the x axis @b float
 * @li @c y.speed Speed for the y axis @b float
 */

#include "gear2d.h"
using namespace gear2d;
#include <iostream>
class kinetic2d
		: public component::base {
	public:
		kinetic2d() {
		};
		
		virtual ~kinetic2d() { }
		virtual component::type type() { return "kinetic2d"; }
		virtual component::family family() { return "kinetics"; }
		virtual std::string depends() { return "spatial"; }
		
		virtual void setup(object::signature & sig) {
			init<float>("x.speed", sig["x.speed"], 0);
			init<float>("y.speed", sig["y.speed"], 0);
			init<float>("x.accel", sig["x.accel"], 0);
			init<float>("y.accel", sig["y.accel"], 0);
		}
		
		virtual void update(timediff dt) {
			float xaccel, yaccel, xspeed, yspeed;
			read("x.accel", xaccel); read("y.accel", yaccel);
			add("x.speed", xaccel * dt);
			add("y.speed", yaccel * dt);
			add("x", read<float>("x.speed") * dt);
			add("y", read<float>("y.speed") * dt);
// 			cout << read<float>("x.accel") << "->" << read<float>("x.speed") << " " << read<float>("y.accel") << "->" << read<float>("y.speed") << endl;
		}
};

extern "C" {
	int initialized = 0;
	component::base * build() {
		if (!initialized) {
			initialized = 1;
		}
		return new kinetic2d;
	}
}