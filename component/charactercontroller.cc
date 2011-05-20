/**
 * @file character-controller.cc
 * @b family controller
 * @b type character
 * @b requires spatial kinetics
 * 
 * Controls x and y triggered by up, down, left and right.
 * 
 * Those keys can be configured in the config file or changed
 * dynamically.
 * 
 * @b parameters
 * @li @c controller.vertical+ key name that will trigger vertical plus movement @b string
 * @li @c controller.vertical- key name that will trigger vertical minus movement @b string
 * @li @c controller.horizontal+ key name that will trigger horizontal plus movement @b string
 * @li @c controller.horizontal- key name that will trigger horizontal minus movement @b string
 * @li @c controller.horizontal.accel Amount of acceleration to be added at each SECOND to speed when pressed @b float
 * @li @c controller.vertical.accel Amount of acceleration to be added at each SECOND to speed when pressed @b float
 * @li @c controller.horizontal.deaccel Amount of acceleration to be decreased each SECOND from speed when NOT pressed @b float
 * @li @c controller.horizontal.deaccel Amount of acceleration to be decreased each SECOND from speed when NOT pressed @b float
 */

#include "gear2d.h"
using namespace gear2d;

#include <string>
using namespace std;

class charactercontroller : public component::base {
	private:
		float haccel;
		float vaccel;
		float hdaccel;
		float vdaccel;
		
	public:
		virtual component::family family() { return "controller"; }
		virtual component::type type() { return "charactercontroller"; }
		virtual string depends() { return "kinetics keyboard"; }
		virtual void setup(object::signature & sig) {
			// 0,0 -> top-left -> Y BEGINS FROM UP, GODDAMIT. 
			string interesting = "";
			string key;
			
			if ((key = sig["controller.vertical+"]) == "") key = "down";
			write("controller.vertical+", key);
			interesting += key + " ";
			
			if ((key = sig["controller.vertical-"]) == "") key = "up";
			write("controller.vertical-", key);
			interesting += key + " ";
			
			if ((key = sig["controller.horizontal+"]) == "") key = "right";
			write("controller.horizontal+", key);
			interesting += key + " ";
			
			if ((key = sig["controller.horizontal-"]) == "") key = "left";
			write("controller.horizontal-", key);
			interesting += key;
			
			write("keyboard.interested", (string) interesting);
		
			haccel = eval(sig["controller.horizontal.accel"], 2.0f);
			write("controller.horizontal.accel", haccel);
			hook("controller.horizontal.accel");
			
			vaccel = eval(sig["controller.vertical.accel"], 2.0f);
			write("controller.vertical.accel", vaccel);
			hook("controller.vertical.accel");
			
			hdaccel = eval(sig["controller.horizontal.deaccel"], 2.0f);
			write("controller.horizontal.deaccel", hdaccel);
			hook("controller.horizontal.deaccel");
			
			vdaccel = eval(sig["controller.vertical.deaccel"], 2.0f);
			write("controller.vertical.deaccel", vdaccel);
			hook("controller.vertical.deaccel");
		}
		
		virtual void update(timediff dt) {
			string key("key.");
// 			cout << key + read<string>("controller.vertical-") << ": " << read<int>(key + read<string>("controller.vertical-")) << endl;
			// vertical- pressed
			if (read<int>(key + read<string>("controller.vertical-")) == 2) { write("y.accel", -vaccel); }
			else if (read<int>(key + read<string>("controller.vertical+")) == 2) { write("y.accel", vaccel); }
			
			// vertical not pressed at all.
			else {
				// if speed positive, deaccel
				if (read<float>("y.speed") > 0.01) { write("y.accel", -vdaccel); }
				
				// if negative, accel
				else if (read<float>("y.speed") < 0.01) { write("y.accel", vdaccel); }
				
				// if zero, zero then.
				else write("y.accel", 0.0f);
			}
			
			// horizontal pressed
			if (read<int>(key + read<string>("controller.horizontal-")) == 2) { write("x.accel", -haccel); }
			else if (read<int>(key + read<string>("controller.horizontal+")) == 2) { write("x.accel", haccel); }
			
			// horizontal not pressed at all.
			else {
				// if speed positive, deaccel
				if (read<float>("x.speed") > 0.001) { write("x.accel", -hdaccel); }
				else if (read<float>("x.speed") < -0.001) { write("x.accel", hdaccel); }
				else write("x.accel", 0.0f);
			}
		}
};

extern "C" {
	component::base * build() {
		return new charactercontroller();
	}
}