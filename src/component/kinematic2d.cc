/**
 * @file kinetic2d.cc
 * @addtogroup kinetics
 * @b family kinetics
 * @b type kinetic2d
 * @author Leonardo Guilherme de Freitas
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
 * @li @c x.accel.{min|max} Minimum and maximum for acceleration on x axis @b float
 * 
 * @li @c y.accel Acceleration for the y axis @b float
 * @li @c y.accel.{min|max} Minimum and maximum for acceleration on y axis @b float
 * 
 * @li @c x.speed Speed for the x axis @b float
 * @li @c x.speed.{min|max} Minimum and maximum for speed on x axis @b float
 * @li @c y.speed Speed for the y axis @b float
 * @li @c y.speed.{min|max} Minimum and maximum for speed on y axis @b float
 * 
 * @warning Minimum and maximum (speed and accel) works like limits for the positive
 * and neegative values. If you want your object to have maximum speed at 100pixels/s 
 * and minimum -150pixels/s, you need to set them both. Maximum defaults do infinity and minimum
 * defaults to minus maximum. That is, if you set maximum to be 100p/s, minimum will be
 * -100p/s by default. If you do not set any, maximum will be infinity, minimum will be
 * minus infinity.
 * 
 */

#include "gear2d.h"
using namespace gear2d;

#include <iostream>
#include <limits>

class kinetic2d
		: public component::base {
	public:
		kinetic2d() {
		};
		
		virtual ~kinetic2d() { }
		virtual component::type type() { return "kinematic2d"; }
		virtual component::family family() { return "kinematics"; }
		virtual std::string depends() { return "spatial/space2d"; }
		
		virtual void setup(object::signature & sig) {
			init<float>("x.speed", sig["x.speed"], 0);
			init<float>("x.speed.max", sig["x.speed.max"], std::numeric_limits<float>::infinity());
			init<float>("x.speed.min", sig["x.speed.min"], -read<float>("x.speed.max"));
			init<float>("y.speed", sig["y.speed"], 0);
			init<float>("y.speed.max", sig["y.speed.max"], std::numeric_limits<float>::infinity());
			init<float>("y.speed.min", sig["y.speed.min"], -read<float>("y.speed.max"));
			
			init<float>("x.accel", sig["x.accel"], 0);
			init<float>("x.accel.max", sig["x.accel.max"], std::numeric_limits<float>::infinity());
			init<float>("x.accel.min", sig["x.accel.min"], -read<float>("x.accel.max"));
			
			init<float>("y.accel", sig["y.accel"], 0);
			init<float>("y.accel.max", sig["y.accel.max"], std::numeric_limits<float>::infinity());
			init<float>("y.accel.min", sig["y.accel.min"], -read<float>("y.accel.max"));			
		}
		virtual void update(timediff dt) {
			float xaccel, yaccel, xspeed, yspeed;
			read<float>("x.accel", xaccel); read<float>("y.accel", yaccel);
			read<float>("x.speed", xspeed); read<float>("y.speed", yspeed);
			clamp(xaccel, read<float>("x.accel.min"), read<float>("x.accel.max"));
			clamp(yaccel, read<float>("y.accel.min"), read<float>("y.accel.max"));
			write<float>("x.accel", xaccel);
			write<float>("y.accel", yaccel);
			xspeed += xaccel * dt;
			yspeed += yaccel * dt;
			clamp(xspeed, read<float>("x.speed.min"), read<float>("x.speed.max"));
			clamp(yspeed, read<float>("y.speed.min"), read<float>("y.speed.max"));
			write("x.speed", xspeed);
			write("y.speed", yspeed);
			add("x", xspeed * dt);
			add("y", yspeed * dt);
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