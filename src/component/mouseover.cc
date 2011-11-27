#include "gear2d.h"
using namespace gear2d;

/**
 * @file mouseover.cc
 * @b Parameters:
 * @li @c mouseover set to true when mouse is over the rectangle, false when not.
 */
class mouseover : public gear2d::component::base {
	public:
		virtual gear2d::component::type type() { return "mouseover"; }
		virtual gear2d::component::family family() { return "mouseover"; }
		virtual std::string depends() { return "spatial mouse/mouse"; }
		
		virtual void setup(object::signature& sig) {
			write("mouseover", false);
		}
		
		virtual void update(timediff dt) {
			int mx, my; float x, y, w, h;
			read<float>("x", x); read<float>("y", y); read<float>("w", w); read<float>("h", h);
			read<int>("mouse.x", mx); read<int>("mouse.y", my);
			if ((mx > x && mx < x + w) && (my > y && my < y + h)) {
				if (read<bool>("mouseover") == false)
					write("mouseover", true);
			}
			else {
				if (read<bool>("mouseover") == true)
					write("mouseover", false);
			}
		}
		
		virtual void handle(parameterbase::id pid, base* lastwrite, object::id owner) {
			
		}
};

extern "C" {
	gear2d::component::base * build() {
		return new mouseover;
	}
}