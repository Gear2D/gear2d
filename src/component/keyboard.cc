/**
 * @file keyboard.cc
 * @b family keyboard
 * @b type keyboard
 * @author Leonardo Guilherme de Freitas
 * @addtogroup keyboard
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
 * @b Parameters:
 * @c keyboard.interested: keys that you are interested in knowing its state. They will be provided as "key.{key}" in parameters (like key.c for c key). @b string
 * @c key.{key}: State of the key in that frame. You need to have that key in keyboard.interested if you want to know about it. @b integer
 * @c keyboard.text: Unicode characters read in this frame @b wstring
 */

#include "gear2d.h"
using namespace gear2d;

#include <iostream>
#include <set>
#include <vector>
using namespace std;

#include "SDL.h"

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
		static vector< pair<int, bool> > keystatus;
		
		// set of all keys we have to check
		static std::set<int> usedkeys;
		
		// all kb components
		static set<keyboard *> kbcomponents;
		
		// how many components asked the update
		static int updated;
		
		// total of components of this kind
		static int updaters;
		
		// initialized?
		static bool initialized;
		
	private:
		set<string> mykeys;
		
	public:
		keyboard() {
		};
		
		virtual ~keyboard() { 
			updaters--;
			kbcomponents.erase(this);
		}
		virtual component::type type() { return component::type("keyboard"); }
		virtual component::family family() { return component::family("keyboard"); }
		virtual void handle(parameterbase::id pid, component::base * lastwrite, object::id owner) {
			if (pid == "keyboard.interested") {
				interest(read<string>("keyboard.interested"));
				cout << "keyboard.interested " << read<string>("keyboard.interested") << endl;
			}
		}
		
		virtual void setup(object::signature & sig) {
			cout << "keyboard hello" << endl;
			if (!initialized) initialize();
			// check the interested keys
			string interested = sig["keyboard.interested"];
			write("keyboard.interested", std::string(""));
			hook("keyboard.interested");
			write<string>("keyboard.interested", sig["keyboard.interested"]);
			write<std::wstring>("keyboard.text", std::wstring());
			updaters++;
			kbcomponents.insert(this);
		}
		
		virtual void update(timediff dt) {
			updated++;
			if (updated >= updaters) {
				doupdate();
				updated = 0;
			}
		}
		
	private:
		// this is called so each kb components gets a chance to warn
		// their friends of kb changes.
		void kbupdate() {
			for (std::set<string>::iterator it = mykeys.begin(); it != mykeys.end(); it++) {
				int key = keynames[*it];
				// values has changed, indeed. notify
				if (keystatus[key].second == true) {
					write(string("key.") + *it, keystatus[key].first);
				}
			}
		}
		
		void interest(string interested) {
			std::set<string> keys;
			split(keys, interested, ' ');
			for (std::set<string>::iterator it = keys.begin(); it != keys.end(); it++) {
				// build keyname
				string keyname = *it;
				if (keynames.find(keyname) == keynames.end()) {
					cerr << "Key " << keyname << " does not exist. Dropping it." << endl;
					continue;
				}
				int k = keynames[keyname];
				
				// tell ourselves we have this key
				mykeys.insert(keyname);
				
				// add the key. prefix
				keyname.insert(0, "key.");

				// bitch.
				bind<int>(keyname, keystatus[k].first);
				
				// tell to use these keys
				usedkeys.insert(k);
				
			}
		}
	private:
		static void doupdate() {
// 			SDL_PumpEvents(); // this is done by the engine now.
			if (usedkeys.size() == 0) return;
			
			if (kbstate[SDLK_q]) exit(0);
			for (std::set<int>::iterator it = usedkeys.begin(); it != usedkeys.end(); it++) {
				int key = *it;
				int kstate = kbstate[key]; // pressed or not-pressed
				int status = keystatus[key].first; // unpressed, clicked, pressed, released
				
				keystatus[key].second = false;
				// unpressed
				if (kstate == 0) {
					// status is clicked or pressed, next: released
					if (status == CLICKED || status == PRESSED) {
						keystatus[key].first = (int)RELEASED;
						keystatus[key].second = true;
					}
			
					// status is relesed, next: unpressed
					else if (status == RELEASED) {
						keystatus[key].first = (int)UNPRESSED;
						keystatus[key].second = true;
					}
				}
				// pressed
				else if (kstate == 1) {
					// if its on a non-pressing state, set as clicked
					if (status == UNPRESSED || status == RELEASED) {
						keystatus[key].first = (int)CLICKED;
						keystatus[key].second = true;
					}
					
					// status is clicked, next: pressed
					else if (status == CLICKED) {
						keystatus[key].first = (int)PRESSED;
						keystatus[key].second = true;
					}
				}
			}
			for (set<keyboard *>::iterator i = kbcomponents.begin(); i != kbcomponents.end(); i++) {
				(*i)->kbupdate();
			}
		}

		static void initialize() {
			if (!SDL_WasInit(SDL_INIT_EVENTTHREAD | SDL_INIT_VIDEO)) {
				if (SDL_InitSubSystem(SDL_INIT_EVENTTHREAD | SDL_INIT_VIDEO) != 0) {
					throw (evil(string("(Keyboard Component) Event threat init fail! ") + SDL_GetError()));
				}
				
			}
			SDL_EnableUNICODE(SDL_TRUE);
			keystatus.resize(SDLK_LAST, pair<int, bool>(0, false));
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
vector< pair<int, bool> > keyboard::keystatus;
map<string, int> keyboard::keynames;
set<keyboard *> keyboard::kbcomponents;

extern "C" {
	component::base * build() {
		return new keyboard;
	}
}