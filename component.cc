#include "component.h"
#include "engine.h"
#include "SDL.h"

namespace gear2d {
	namespace component {
		base::base() {
		}
		
		base::~base() {
		}
		
		void factory::set(component::type t, factory::builder b) {
			if (b == 0) return;
			builders[t] = b;
		}
		
		void factory::load(component::type t, std::string file) {
			/* TODO: check if we're in windows or linux */
			if (file == "") file = "component/lib" + t + ".so";
			
			/* get the object handler */
			factory::handler comhandler = SDL_LoadObject(file.c_str());
			if (comhandler == 0) {
				file = compath + "/" + file;
				comhandler = SDL_LoadObject(file.c_str());
				if (comhandler == 0) {
					std::cerr << "(Component factory) Error loading component " << t << ": " << SDL_GetError() << std::endl;
					return;
				}
			}

			/* get the builder function */
			factory::builder combuilder = (factory::builder)SDL_LoadFunction(comhandler, "build");
			if (combuilder == 0) {
				std::cerr << "(Component factory) Error loading component " << t << ": " << SDL_GetError() << std::endl;
				SDL_UnloadObject(comhandler);
				return;
			}
			
			/* register the handler and the builder */
			handlers[t] = comhandler;
			builders[t] = combuilder;
			
			std::cerr << "(Component factory) Component type " << t << " loaded" << std::endl;
			
			return;
		}
		
		base * factory::build(component::type t) {
			/* try to locate component based on type */
			buildertable::iterator tit = builders.find(t);
			if (tit == builders.end()) {
				std::cerr << "(Component factory) Component " << t << " not found!" << std::endl;
				return 0;
			}
			
			/* build it with the given function */
			factory::builder b = tit->second;
			base * component = b();
			return component;
		}
	}
}