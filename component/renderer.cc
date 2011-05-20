/**
 * @file renderer.cc
 * @b family renderer
 * @b type renderer
 * 
 * Renderer component that can work with images,
 * meshes&textures, primitives, etc...
 *
 * @b parameters
 * @li @c renderer.w Width of the render screen. Global. Shall be set at the game config file. Defaults to 800. @b int 
 * @li @c renderer.h Height of the render screen. Global. Shall be set at the game config file. Defaults to 600. @b int
 * @li @c camera.{x|y|z} Camera x, y and z position for rendering calculations. Global. Does nothing on object signature. Defaults to 0. @b float
 * @li @c renderer.surfaces list of surfaces to load and work with, in the form of id=file separated by spaces. Loaded at setup and whenever changed.  @b string
 * @li @c <id>.position.bind Tells if the position of this image shall follow position info (x, y). @b bool. Defaults to 1 (true)
 * @li @c <id>.position.absolute Tells if camera should be taken in account when rendering this. @b bool. 1 (true)
 * @li @c <id>.position.{x|y|z} Specifics about the position of the image. If position.bind is specified, it will become an offset. @b float
 * @li @c <id>.clip.{x|y|w|h} Clipping frame for the image. Defaults to whole image. @b float
 * @li @c <id>.alpha Alpha value for the surface. 0 is transparent, 1 is opaque. @b float
 *
 * 
 * @b globals:
 * @li @c imgpath Path where to look for image files. Shall be set at the main config file
 *
 */

#include "gear2d.h"
using namespace gear2d;

#include <set>
#include <string>
using namespace std;

#include "SDL.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <SDL_image.h>
using boost::algorithm::split;
using boost::algorithm::is_any_of;


// this handles surface parameters
class surface {
	public:
		// id of this surface
		string id;
		component::base * parent;
		
		// parameters to be referenced
		float x;
		float y;
		float z;
		float alpha;
		bool bind;
		bool absolute;
		
		// raw surface
		SDL_Surface * raw;
		
	public:
		bool operator<(const surface & other) const {
			// only zordering really matters.
			return (z < other.z);
		}
		
		bool operator==(const surface & other) const {
			if (this == &other) return true;
			if (
				id == other.id &&
				z == other.z &&
				x == other.x &&
				y == other.y &&
				raw == other.raw
			) { return true; }
			return false;
		}
		
		surface(component::base * parent, string id, SDL_Surface * raw)
		: parent(parent)
		, id(id)
		, x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, alpha(1.0f)
		, bind(true)
		, absolute(false)
		, raw(raw) { 
		}
		
		virtual ~surface() { }
};


class renderer : public component::base {
	private:
		typedef pair<float, surface *> zorder;

		static map<string, SDL_Surface *> rawbyfile;
		static SDL_Surface * screen;
		static bool initialized;
		static set<zorder> zordered;
		static int rendervotes;
		static list<renderer *> renderers;
			
	private:
		map<string, surface*> surfacebyid;
		string surfaces;
		
	public:
		renderer() {
			renderers.push_back(this);
		}
		virtual ~renderer() {
			renderers.remove(this);
			for (map<string, surface *>::iterator i = surfacebyid.begin(); i != surfacebyid.end(); i++) {
				surface * s = i->second;
				// remove from rendering list
				zordered.erase(zorder(s->z, s));
			}
		}
		
		virtual component::type type() { return "renderer"; }
		virtual component::family family() { return "renderer"; }
		virtual string depends() { return "spatial"; }
		
		virtual void handle(parameterbase::id pid, component::base * lastwrite) {
			int p;
			if (pid == "renderer.surfaces") {
				loadsurfaces(surfaces);
			}
			// handle when z changes, so we can update our zordered list
			else if ((p = pid.find(".position.z")) != 0) {
				string surfid = pid.substr(0, p);
				surface * s = surfacebyid[surfid];
				if (s == 0) return;
				// right, the very same parameter we're interested
				zordered.erase(zorder(s->z, s));
				zordered.insert(zorder(s->z, s));
			}
		}
		virtual void setup(object::signature & sig) {
			if (!initialized) {
				int w = eval(sig["renderer.w"], 800);
				int h = eval(sig["renderer.h"], 600);
				initialize(w, h);
				write("renderer.w", w);
				write("renderer.h", h);
			}
			
			string surfacelist = sig["renderer.surfaces"];
			bind("renderer.surfaces", surfaces);
			hook("renderer.surfaces");
			
			if (surfacelist == "") return;
			loadsurfaces(surfacelist, sig["imgpath"]);
			
			// initialize with signature;
			for (map<string, surface*>::iterator i = surfacebyid.begin(); i != surfacebyid.end(); i++) {
				string id = i->first;
				surface * s = i->second;
				
				// pid was set by bind() of by surface() in worst case.
				write(id + ".position.x", eval(sig[id + ".position.x"], 0.0f));
				write(id + ".position.y", eval(sig[id + ".position.y"], 0.0f));
				write(id + ".position.z", eval(sig[id + ".position.z"], 0.0f));
				write(id + ".bind", eval(sig[id + ".bind"], true));
				write(id + ".absolute", eval(sig[id + ".absolute"], true));
				write(id + ".alpha", eval<float>(sig[id + ".alpha"], 1.0f));
				hook(id + ".position.z");
				zordered.insert(zorder(s->z, s));
			}
		}
		
		virtual void update(float dt) {
			rendervotes++;
			if (rendervotes >= renderers.size()) {
				render();
				rendervotes = 0;
			}
		}
	private:
		// split and iterates through the surface list, loading them
		// and binding id and position parameters
		void loadsurfaces(string surfacelist, string imgpath = "") {
			set<string> surfaces;
			set<string>::iterator surfacedef;
			split(surfaces, surfacelist, is_any_of(" "));
			for (surfacedef = surfaces.begin(); surfacedef != surfaces.end(); surfacedef++) {
				int p = surfacedef->find('=');
				string id = surfacedef->substr(0, p);
				string file = surfacedef->substr(p+1);
				if (surfacebyid[id] != 0) continue;
				SDL_Surface * raw = getsurface(imgpath + file);
				surface * s = new surface(this, id, raw);
				surfacebyid[id] = s;
				
				// these will bind param names to our data
				bind(id + ".position.x", s->x);
				bind(id + ".position.y", s->y);
				bind(id + ".position.z", s->z);
				bind(id + ".bind", s->bind);
				bind(id + ".absolute", s->absolute);
				bind(id + ".alpha", s->alpha);
				// setup and hook flags
				
			}
		}
		
		SDL_Surface * getsurface(string file, bool reload = false) {
			SDL_Surface * tmp, *s;
			if ((s = rawbyfile[file]) != 0 && reload == false) { return s; }
			tmp = IMG_Load(file.c_str());
			if (tmp == 0) return 0;
			if (tmp->format->Amask != 0) {
				SDL_SetAlpha(tmp, SDL_RLEACCEL | SDL_SRCALPHA, 255);
				s = SDL_DisplayFormatAlpha(tmp);
			} else {
				SDL_SetColorKey(tmp, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(tmp->format, 0xff, 0, 0xff));
				s = SDL_DisplayFormat(tmp);
			}
			SDL_FreeSurface(tmp);
			rawbyfile[file] = s;
			return s;
		}
	private:
		static void render() {
			if (zordered.size() == 0) return;
			SDL_Rect dstrect;
			for (set<zorder>::iterator i = zordered.begin(); i != zordered.end(); i++) {
				surface * s = i->second;
				dstrect.x = dstrect.y = 0;
				if (s->bind) {
					const float & x = s->parent->raw<float>("x");
					const float & y = s->parent->raw<float>("y");
					dstrect.x = x;
					dstrect.y = y;
				}
				dstrect.x += s->x;
				dstrect.y += s->y;
				SDL_SetAlpha(s->raw, SDL_SRCALPHA | SDL_RLEACCEL, s->alpha * 255);
				SDL_BlitSurface(s->raw, 0, screen, &dstrect);
			}
			SDL_Flip(screen);
			SDL_Delay(1);
		}
		static void initialize(int w, int h) {
			if (initialized == true) return;
			if (!SDL_WasInit(SDL_INIT_VIDEO)) {
				if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
					throw evil(string("(Rendering component) Failed initializing SDL: ") + SDL_GetError());
				}
			}
			
			if (screen == 0) screen = SDL_SetVideoMode(w, h, 32, SDL_DOUBLEBUF);
			if (screen == 0) {
				throw evil(string("(Rendering component) Failed initializing video:") + SDL_GetError());
			}
		}
};

map<string, SDL_Surface *> renderer::rawbyfile;
set<renderer::zorder> renderer::zordered;
SDL_Surface * renderer::screen = 0;
bool renderer::initialized = false;
int renderer::rendervotes = 0;
list<renderer*> renderer::renderers;

extern "C" {
	component::base * build() {
		return new renderer;
	}
}