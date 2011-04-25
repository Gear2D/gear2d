#include "gear2d.h"
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include <list>
using namespace std;

/**
 * @file visualgfx.cc
 * This is a component that
 * loads and renders graphic visualgfxs
 * on the screen */


using namespace gear2d;

gear2d::pbase::table paramtemplate;
int finished = 0;
int instances = 0;



/* Make like a little bit easier, allowing lexcast to work */
void operator>>(std::istringstream & ss, std::list<std::string> & l) {
	while (!ss.eof()) {
		std::string t;
		ss >> t;
		l.push_back(t);
	}
}

/**
 * @brief Visual as graphical representation
 * This component represents an object using a graphic
 * (or a list of them).
 * 
 * Parameters affecting this component:
 * <b>graphics</b> A list of graphics to load to represent the object, separated by a space. (list\<string\>)
 * <b>__screenwidth</b> The width of the screen (global) (string)
 * <b>__screenheight</b> The height of the screen (global) (string)
 * <b>x</b> Base X for graphic rendering (float)
 * <b>y</b> Base Y for graphic rendering (float)
 * <b>\<graphicname\>.x</b> X coordinate for a given graphic name (float)
 * <b>\<graphicname\>.y</b> Y coordinate for a given graphic name (float)
 * Anytime you change its value, the component will load/unload and render the graphic */
class visualgfx : public component::base
	, public pbase::listener< std::list<string> > {
	private:
		parameter<int> screenwidth;
		parameter<int> screenheight;
		SDL_Surface * screen;
		map<string, SDL_Surface *> surfaces;
		
	public:
		visualgfx() { instances++; };
		virtual ~visualgfx() { instances--; };
		virtual component::type type() { return "visualgfx"; }
		
		virtual void init() { 
			
			screenwidth.set(access<string>("__screenwidth").get());
			screenheight.set(access<string>("__screenheight").get());
			screen = SDL_GetVideoSurface();
			if (screen == 0) {
				screen = SDL_SetVideoMode(screenwidth, screenheight, 32, SDL_DOUBLEBUF);
			}
			
			if (screen == 0) {
				std::cerr << "visualgfx failed initializing. Your program will fail very soon." << std::endl;
			}
			
			/* get the graphic list */
			list<string> & glist = access< list<string> >("graphics");
			
			/* load all of them */
			for (list<string>::iterator git = glist.begin(); git != glist.end(); git++) {
				surfaces[*git] = load(*git);
			}
			
			/* hook to the graphic list */
			access< list<string> >("graphics").hook(*this);
		}
		
		virtual void handle(pbase::id pid, const std::list<string> & oldv, const std::list<string> & newv) {
			
		}
		
		virtual const pbase::table & parameters() { return paramtemplate; }
		virtual void update(timediff dt) {
			SDL_Rect srcrect, dstrect;
			for (map<string, SDL_Surface *>::iterator surfit = surfaces.begin(); surfit != surfaces.end(); surfit++) {
				if (surfit->second == 0) continue;
				dstrect.x = access<float>("x").get();
				dstrect.y = access<float>("y").get();
// 				dstrect.w = dstrect.h = 0;
				
				SDL_BlitSurface(surfit->second, NULL, screen, &dstrect);
			}
			
			/* flip only when all gfx components finished rendering */
			finished++;
			if (finished == instances) { SDL_Flip(screen); finished = 0; }
		}
		
		private:
		SDL_Surface * load(string filename) {
			SDL_Surface * tmp = IMG_Load(filename.c_str());
			SDL_Surface * surface = 0;
			if (tmp == 0) {
				cerr << "Not possible to load " << filename << ": " << IMG_GetError() << endl;
				return 0;
			}
			
			if (tmp->format->Amask != 0) {
				SDL_SetAlpha(tmp, SDL_RLEACCEL | SDL_SRCALPHA, 0);
				surface = SDL_DisplayFormatAlpha(tmp);
			} else {
				SDL_SetColorKey(tmp, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(tmp->format, 0xff, 0, 0xff));
				surface = SDL_DisplayFormat(tmp);
			}
			SDL_FreeSurface(tmp);
			return surface;
		}
};

/*
template <typename Target, typename Source>
	Target lexcast(Source & s) {
		std::istringstream iss(s);
		Target t;
		iss >> t;
		if (iss.fail()) std::cerr << "Conversion from(" << s << ") failed!" << std::endl;
		std::cout << "We all hate lexcast" << std::endl;
		return t;
	}*/

extern "C" {
	int initialized = 0;
	component::base * build() {
		if (!initialized) {
			paramtemplate["graphics"] = new parameter< std::list<std::string> >;
			
			/* initialize SDL screen */
			if (!SDL_WasInit(SDL_INIT_VIDEO)) {
				if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
					std::cerr << "Error initializing visualgfx component! Will fail now." << std::endl;
					return 0;
				}
			}
			initialized = 1;
		}
		return new visualgfx;
	}
}

