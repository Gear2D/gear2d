/**
 * @file mouse.cc
 * @addtogroup mouse
 * @brief Provides mouse input.
 * 
 * @b Parameters:
 * @li mouse.{x|y} x and y position for the mouse cursor
 * @li mouse.{1|2|3} buttons one through 3 of the mouse
 * It reports the position where the mouse is and which of the three buttons are clicked.
 */


#include "gear2d.h"
#include "SDL.h"
using namespace gear2d;

class mouse : public component::base {
	public:
		mouse() {
			voters++;
		}
		
		virtual ~mouse() {
			voters--;
		}
		
		virtual component::type type() { return "mouse"; }
		virtual component::family family() { return "mouse"; }
		virtual void setup(object::signature & sig) {
			if (!initialized) initialize();
			write<int>("mouse.x", 0);
			write<int>("mouse.y", 0);
			write<int>("mouse.1", 0);
			write<int>("mouse.2", 0);
			write<int>("mouse.3", 0);
			
			
			/* ----------------- */
			
		}
		
		virtual void update(timediff dt) {
			voting++;
			if (voting >= voters) {
				globalupdate(dt);
				voting = 0;
			}

			if (read<int>("mouse.x") != x) write("mouse.x", x);
			if (read<int>("mouse.y") != y) write("mouse.y", y);
			if (read<int>("mouse.1") != bt1) write<int>("mouse.1", bt1 != 0);
			if (read<int>("mouse.2") != bt2) write<int>("mouse.2", bt2 != 0);
			if (read<int>("mouse.3") != bt3) write<int>("mouse.3", bt3 != 0);
		}
		
	private:
		static int voters;
		static int voting;
		
		/* pos */
		static int x;
		static int y;
		
		/* buttons */
		static int bt1;
		static int bt2;
		static int bt3;
		
		static bool initialized;
		
	private:
		static void initialize() {
			if (initialized) return;
			if (!SDL_WasInit(SDL_INIT_EVENTTHREAD)) {
				if (SDL_InitSubSystem(SDL_INIT_EVENTTHREAD | SDL_INIT_VIDEO) != 0) {
					std::cerr << "(Mouse component) could not initialize event thread." << std::endl;
					return;
				}
			}
			initialized = true;
		}
		static void globalupdate(timediff dt) {
			if (!initialized) return;
			Uint8 flags = SDL_GetMouseState(&x, &y);
			bt1 = flags & SDL_BUTTON(1);
			bt2 = flags & SDL_BUTTON(2);
			bt3 = flags & SDL_BUTTON(3);
		}
};

int mouse::voters = 0;
int mouse::voting = 0;
int mouse::x = 0;
int mouse::y = 0;
int mouse::bt1 = 0;
int mouse::bt2 = 0;
int mouse::bt3 = 0;
bool mouse::initialized = false;

extern "C" {
	component::base * build() { return new mouse(); }
}