/**
 * @file rigidbody2d.cc
 * @addtogroup dynamics
 * @b family dynamics
 * @b type rigidbody2d
 * 
 * This components reacts to collision making speed change
 * as if the object was bouncing.
 *
 * @b depends
 * @li collider, kinetics, spatial
 * 
 * @b using
 * @li @c collider.collision Hooking point to know when we collided
 * @li @c collider.collision.side Side where the collision happened
 * @li @c collider.collision.speed.{x|y} As the speed of the other colliding object
 * 
 * @b provides
 * @li @c dynamics.mass Mass of this object in kilograms. Defaults to 0.1 @b float
 * @li @c dynamics.kinetic Should this component not move in reacting to collision (like a wall)
 * 	Please note that even if kinetic, its mass still makes diference in reaction calculations
 * 
 * 	Note that this component (as should be with any dynamics component) only
 * 	correctly react with objects that have mass defined (so have some sort of dynamics component)
 */

#include <cmath>

#include "gear2d.h"
using namespace gear2d;

#include <limits>
using namespace std;

class bounce : public component::base {
	private:
		float dt;
	public:
		bounce() : dt(0) { }
		virtual component::family family() { return "dynamics"; }
		virtual component::type type() { return "rigidbody2d"; }
		virtual std::string depends() { return "spatial collider kinetics"; }
		virtual void handle(parameterbase::id pid, component::base * lastwrite) {
			if (pid == "collider.collision") {
				component::base * c = read<component::base *>("collider.collision");
				// we do nothing with an object without mass
				if (!c->exists("dynamics.mass")) return;
				if (read<bool>("dynamics.kinetic")) return;
				const float & m1 = raw<float>("dynamics.mass");
				const float & m2 = c->raw<float>("dynamics.mass");
				
				// detect which side we-re colliding.
				const float & xspeed = raw<float>("x.speed");
				const float & yspeed = raw<float>("y.speed");
				const int & side = raw<int>("collider.collision.side");
				const float & csx = raw<float>("collider.collision.speed.x");
				const float & csy = raw<float>("collider.collision.speed.y");
				const float & colxspeed =  raw<float>("collider.collision.speed.x");
				const float & colyspeed =  raw<float>("collider.collision.speed.y");

				// restore x and y based on speed
				add("x", -xspeed * dt);
				add("y", -yspeed * dt);
				
				// adjust x and y to non-colliding positions
				float resxspeed, resyspeed;
				if (side == 0 || side == 2) {
					resxspeed = ((m1-m2)/(m1+m2))*xspeed + ((2*m2)/(m1+m2))*colxspeed;
					write("x.speed", resxspeed);
				}
				if (side == 1 || side == 3) {
					resyspeed = ((m1-m2)/(m1+m2))*yspeed + ((2*m2)/(m1+m2))*colyspeed;
					write("y.speed", resyspeed);
				}
			}
		}
		
		virtual void setup(object::signature & sig) {
			if (sig["dynamics.mass"] == "infinity") {
				write<float>("dynamics.mass", numeric_limits<float>::infinity());
			} else {
				init<float>("dynamics.mass", sig["dynamics.mass"], 1.0f);
			}
			init<bool>("dynamics.kinetic", sig["dynamics.kinetic"], false);
			hook("collider.collision");
		}
		
		virtual void update(float dt) { this->dt=dt; }
};

extern "C" {
	component::base * build() { return new bounce; }
}

