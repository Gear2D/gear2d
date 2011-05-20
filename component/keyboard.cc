/**
 * @file keyboard.cc
 * @b family keyboard
 * @b type keyboard
 * 
 * Default keyboard component. It provides the state of the key
 * as an integer with 4 possible values:
 * 
 * 0 for unpressed
 * 1 for clicked
 * 2 for pressed
 * 3 for released
 * 
 * Keys that match printable characters are identified by them:
 *
 * a - a key
 * b - b key
 * c - c key
 * 1 - 1 key
 * 2 - 2 key
 * 
 * and so on.
 * 
 * For those non-printable:
 * 
 * arrow-up, arrow-down, arrow-right, arrow-left for the arrows
 * ctrl-left - left control
 * ctrl-right - right control
 * alt-left - left alt
 * alt-right - right alt
 * shift-left - left shift
 * shift-right - right shift
 * tab - tab key
 * backspace - backspace key
 * f{1-12} - function keys 1 through 12
 * (space char) - space
 * return - return key
 * 
 * @b Parameters
 * @c keyboard.interested keys that you are interested in knowing its state. They will be provided as "key.{key}" in parameters (like key.c for c key). @b string
 * @c key.{key} State of the key in that frame. You need to have that key in keyboard.interested if you want to know about it. @b integer
 */

#include "gear2d.h"
using namespace gear2d;

#include <iostream>
#include <set>
#include <vector>
using namespace std;

#include "SDL.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
using boost::algorithm::split;
using boost::algorithm::is_any_of;

class keyboard : public component::base {
	private:
		enum {
			UNPRESSED,
			CLICKED,
			PRESSED,
			RELEASED
		};
	private:
		// array with keystates
		static Uint8 * kbstate;
		
		// map of name->keyid
		static map<string, int> keynames;
		
		// all status for each key
		static vector<unsigned char> keystatus;
		
		// set of all keys we have to check
		static std::set<int> usedkeys;
		
		// how many components asked the update
		static int updated;
		
		// total of components of this kind
		static int updaters;
		
		// initialized?
		static bool initialized;
		
	public:
		keyboard() {
		};
		
		virtual ~keyboard() { 
			updaters--;
		}
		virtual component::type type() { return component::type("keyboard"); }
		virtual component::family family() { return component::family("keyboard"); }
		void handle(parameterbase::id pid, component::base * lastwrite) {
			interest(read<string>(pid));
		}
		
		virtual void setup(object::signature & sig) {
			cout << "keyboard hello" << endl;
			if (!initialized) initialize();
			
			// check the interested keys
			string interested = sig["keyboard.interested"];
			init<string>("keyboard.interested", interested, "");
			hook("keyboard.interested");
			interest(interested);
			updaters++;
		}
		
		virtual void update(timediff dt) {
			updated++;
			if (updated >= updaters) {
				doupdate();
				updated = 0;
			}
		}
		
	private:
		void interest(string interested) {
			cout << "debug: " << "they are interested in " << interested << "!" << endl;
			std::set<string> keys;
			split(keys, interested, is_any_of(" "));
			for (std::set<string>::iterator it = keys.begin(); it != keys.end(); it++) {
				// build keyname
				string keyname = *it;
				if (keynames.find(keyname) == keynames.end()) {
					cerr << "Key " << keyname << " does not exist. Dropping it." << endl;
					continue;
				}
				int k = keynames[keyname];
				keyname.insert(0, "key.");

				// bitch.
				bind(keyname, keystatus[k]);
				
				// tell to use these keys
				usedkeys.insert(k);
			}
		}
	private:
		static void doupdate() {
			SDL_PumpEvents();
			if (usedkeys.size() == 0) return;
			if (kbstate[SDLK_q]) exit(0);
			for (std::set<int>::iterator it = usedkeys.begin(); it != usedkeys.end(); it++) {
				int key = *it;
				int kstate = kbstate[key]; // pressed or not-pressed
				int status = keystatus[key]; // unpressed, clicked, pressed, released
					
				// unpressed
				if (kstate == 0) {
					
					// status is clicked or pressed, next: released
					if (status == CLICKED || status == PRESSED) {
						keystatus[key] = (unsigned char)RELEASED;
					}
			
					// status is relesed, next: unpressed
					else if (status == RELEASED) {
						keystatus[key] = (unsigned char)UNPRESSED;
					}
				}
				else if (kstate == 1) {
					// if its on a non-pressing state, set as clicked
					if (status == UNPRESSED || status == RELEASED) {
						keystatus[key] = (unsigned char)CLICKED;
					}
					
					// status is clicked, next: pressed
					else if (status == CLICKED) {
						keystatus[key] = (unsigned char)PRESSED;
					}
				}
			}
		}

		static void initialize() {
			if (!SDL_WasInit(SDL_INIT_EVENTTHREAD | SDL_INIT_VIDEO)) {
				if (SDL_InitSubSystem(SDL_INIT_EVENTTHREAD | SDL_INIT_VIDEO) != 0) {
					throw (evil(string("(Keyboard Component) Event threat init fail! ") + SDL_GetError()));
				}
			}
			
			keystatus.resize(SDLK_LAST, 0);
			kbstate = SDL_GetKeyState(NULL);
			
			// initialize keynames...
			char * name;
			for (int i = 0; i < SDLK_LAST; i++) {
				name = SDL_GetKeyName(SDLKey(i));
				keynames[name] = i;
			}
			initialized = true;
		}
};

int keyboard::updated = 0;
int keyboard::updaters = 0;
bool keyboard::initialized = false;
Uint8 * keyboard::kbstate = 0;
std::set<int> keyboard::usedkeys;
vector<unsigned char> keyboard::keystatus;
map<string, int> keyboard::keynames;

extern "C" {
	component::base * build() {
		return new keyboard;
	}
}