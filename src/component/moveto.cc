/**
 * @file moveto.cc
 * @b family controller
 * @b type moveto
 * 
 * A move-to controller that you can specify a target
 * and the object will set it speed to reach that target,
 * either smoothly or not.
 * 
 * @b provides
 * @li @c controller.target.{x|y|z} target x, y and z to reach. Defaults to its own position @b float 
 * @li @c controller.smooth Controller shall go smoothly. Defaults to true @b boolean
 * @li @c controller.reached This will be set to true when the object reaches the target @b boolean
 * 
 */

#include "gear2d.h"
using namespace gear2d;

#include <math.h>

class moveto : public component::base {
	private:
		const float * x;
		const float * y;
		const float * z;
	public:
		virtual component::family family() { return "controller"; }
		virtual component::type type() { return "moveto"; }
		virtual std::string depends() { return "spatial/space2d kinematics/kinematic2d"; }
		virtual void setup(object::signature & sig) {
			init("controller.target.x", sig["controller.target.x"], read<float>("x"));
			init("controller.target.y", sig["controller.target.y"], read<float>("y"));
			init("controller.target.z", sig["controller.target.z"], read<float>("z"));
			init("controller.smooth", sig["controller.smooth"], true);
			write("controller.reached", false);
			x = &raw<float>("x");
			y = &raw<float>("y");
			z = &raw<float>("z");
		}
		
		virtual void update(float dt) {
			const float & tx = raw<float>("controller.target.x");
			const float & ty = raw<float>("controller.target.y");
			const float & tz = raw<float>("controller.target.z");
			if (
				(fabs(*x - tx) < 0.1) &&
				(fabs(*y - ty) < 0.1) &&
				(fabs(*z - tz) < 0.1)
				)
			{
				write("controller.reached", true);
			}
			
			if (read<bool>("controller.smooth")) {
				write("x.accel", (tx - *x));
				write("y.accel", (ty - *y));
				write("z.accel", (tz - *z));
			}
		}
};

extern "C" {
	component::base * build() { return new moveto; }
}
