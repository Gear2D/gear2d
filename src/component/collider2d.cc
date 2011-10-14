/**
 * @file collider2d.cc
 * @b family collider
 * @b type collider
 * @author Leonardo Guilherme de Freitas
 * @addtogroup collider
 * 
 * Calculates collision between all colliders, informing side.
 * <b>provides/acts upon</b>
 * @li @c collider.usez Use z in collision calculations (objects with different z's won't collide). Defaults to 0 @b bool (0 or 1)
 * @li @c collider.bind Moves along with spatial's x and y. Defaults to 1 @b bool
 * @li @c collider.tag Tag of this collider @b string
 * @li @c collider.avoid List of colliding tags names to ignore collision, separated by white-space @b string
 * @li @c collider.aabb.{x|y|w|h} Axis aligned bounding box for this object. Defaults to spatial's w,h. x and y defaults to 0. @b float
 * @li @c collider.collision Set whenever this component collides with another.
 * 	May be set multiple times on a frame. You want to hook on this one.
 * <b>collider casted as gear2d::component::base*</b>
 * @li @c collider.collision.side Side where the collision occurs more severely. Integer from 0 to 3, starting at left, clockwise. @b int
 * @verbatim 0: left, 1: up, 2: right, 3: down, -1: not collision. @endverbatim
 * @li @c collider.collision.speed.{x|y} Speed of the object that collided with yours. THE OTHER OBJECT, NOT YOURS. DUMBASS.
 * @li @c collider.collision.{x|y|w|h} Interception rectangle for the two bounding boxes
 */
#include <cmath>
#include "component.h"
using namespace gear2d;

#include <string>
using namespace std;

class collider : public component::base {
	private:
		struct colaabb {
			float x;
			float y;
			float w;
			float h;
			colaabb(float x = 0, float y = 0, float w = 0, float h = 0) : x(x), y(y), w(w), h(h) { }
		};
		
		colaabb aabb;
		
		string tag;
		string ignore;
		
	public:
		virtual ~collider() { colliders.erase(this); }
		virtual component::type type() { return "collider2d"; }
		virtual component::family family() { return "collider"; }
		virtual string depends() { return "spatial/space2d"; }
		virtual void handle(parameterbase::id pid, base* lastwrite, object::id owner) {
		}
		
		virtual void setup(object::signature & sig) { 
			init<string>("collider.type", sig["collider.type"], "aabb");
			init("collider.usez", sig["collider.usez"], false);
			
			bool binded = eval(sig["colliderd.bind"], true);
			write("collider.bind", binded); 
			
			float x, y, w, h;
			read("x", x); read("y", y), read("w", w), read("h", h);
			hook("w"); hook("h");

			aabb.x = eval(sig["collider.aabb.x"], 0);
			bind("collider.aabb.x", aabb.x);
			
			aabb.y = eval(sig["collider.aabb.y"], 0);
			bind("collider.aabb.y", aabb.y);
			
			aabb.w = eval(sig["collider.aabb.w"], w);
			bind("collider.aabb.w", aabb.w);
			
			aabb.h = eval(sig["collider.aabb.h"], h);
			bind("collider.aabb.h", aabb.h);
			write<component::base *>("collider.collision", 0);
			
			write("collider.collision.side", -1);
			
			write<float>("collider.collision.speed.x", 0);
			write<float>("collider.collision.speed.y", 0);
			
			write<float>("collider.collision.x", 0);
			write<float>("collider.collision.y", 0);
			write<float>("collider.collision.w", 0);
			write<float>("collider.collision.h", 0);
			
			bind("collider.tag", tag);
			bind("collider.ignore", ignore);
			ignore = sig["collider.ignore"];
			tag = sig["collider.tag"];
			if (tag == "") tag = sig["name"];
			
			colliders.insert(this);
		}
		
		virtual void update(float dt) {
			if (++voteforupdate >= colliders.size()) {
				calculate();
				voteforupdate = 0;
			}
		}
	
	private:
		
		
	private:
		
		// This set ups bullet world and context
		static void initialize() {
			
		}
		
		static void calculate() {
			if (colliders.size() <= 1) return;
			for (std::set<collider *>::iterator i = colliders.begin(); i != colliders.end(); i++) {
				
				collider * first = *i;
				// tests from this to the rest
				std::set<collider *>::iterator j = i;
				j++;
				for (; j != colliders.end(); j++) {
					collider * second = *j;
					const string & ftype = first->raw<string>("collider.type");
					const string & stype = second->raw<string>("collider.type");
					const bool & fbind = first->raw<bool>("collider.bind");
					const bool & sbind = second->raw<bool>("collider.bind");
					colaabb faabb(first->aabb), saabb(second->aabb);
					
					if (fbind) {
						faabb.x += first->raw<float>("x");
						faabb.y += first->raw<float>("y");
					}
					
					if (sbind) {
						saabb.x += second->raw<float>("x");
						saabb.y += second->raw<float>("y");
					}
					
					if (testaabb(faabb, saabb)) {
						// calculates intersection
						colaabb inter;
						inter.x = max(faabb.x, saabb.x);
						inter.y = max(faabb.y, saabb.y);
						inter.w = min(faabb.x + faabb.w, saabb.x + saabb.w) - max(faabb.x, saabb.x);
						inter.h = min(faabb.y + faabb.h, saabb.y + saabb.h) - max(faabb.y, saabb.y);
						
						int fc, sc;
						
						// when h bigger than w, side collision first
						if (inter.h > inter.w) {
							// test if intercep.x = first x, if so, first collision left
							if (inter.x == faabb.x) { fc = 0; sc = 2; }
							else { fc = 2; sc = 0; }
						} else {
							if (inter.y == faabb.y) { fc = 1; sc = 3; }
							else { fc = 3; sc = 1; }
						}
						
						
						first->write<float>("collider.collision.x", inter.x);
						first->write<float>("collider.collision.y", inter.y);
						first->write<float>("collider.collision.w", inter.w);
						first->write<float>("collider.collision.h", inter.h);
						first->write<int>("collider.collision.side", fc);
						first->write<float>("collider.collision.speed.x", second->read<float>("x.speed"));
						first->write<float>("collider.collision.speed.y", second->read<float>("y.speed"));
						
						second->write<float>("collider.collision.x", inter.x);
						second->write<float>("collider.collision.y", inter.y);
						second->write<float>("collider.collision.w", inter.w);
						second->write<float>("collider.collision.h", inter.h);
						second->write<int>("collider.collision.side", sc);
						second->write<float>("collider.collision.speed.x", first->read<float>("x.speed"));
						second->write<float>("collider.collision.speed.y", first->read<float>("y.speed"));
						
// 						cout << "fignore/stag: " << first->ignore << " " << second->tag << endl;
						if (first->ignore.find(second->tag) == string::npos) {
							first->write<component::base *>("collider.collision", second);
						}
							
// 						cout << "signore/ftag: " << second->ignore << " " << first->tag << endl;
						if (second->ignore.find(first->tag) == string::npos) {
							second->write<component::base *>("collider.collision", first);
						}
					}
				}
			}
		
		}
		static bool testaabb(colaabb & a, colaabb & b) {
			return (
				(b.x < a.x + a.w) &&
				(b.x + b.w > a.x) &&
				(b.y < a.y + a.h) &&
				(b.y + b.h > a.y) //
			);
		}

	private:
		static int voteforupdate;
		static std::set<collider *> colliders;
};

int collider::voteforupdate = 0;
std::set<collider*> collider::colliders;

extern "C" {
	component::base * build() { return new collider; }
}
