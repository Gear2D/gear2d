#include "engine.h"
#include "component.h"
#include "object.h" 
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <vector>
#include <string>
#include "SDL.h" // for the timer

namespace gear2d {
	std::map<std::string, std::string> * engine::config;
	component::factory * engine::cfactory;
	object::factory * engine::ofactory;
	std::map<component::type, std::list<component::base *> > * engine::components;
	std::list<component::base *> * engine::removed;
	bool engine::initialized;
	bool engine::started;
	
	
	void engine::add(component::base * c) {
		if (components == 0) {
			std::cerr << "Initialize the engine before attaching a component to an object" << std::endl;
			init();
		}
		if (c == 0) return;
		(*components)[c->type()].push_back(c); 
	}

	void engine::remove(component::base * c) {
		init();
		removed->push_back(c);
	}
	
	void engine::init(bool force) {
		if (initialized == true && force == false) return;
		if (config != 0) delete config;
		config = new std::map<std::string, std::string>;
		
		if (components != 0) delete components;
		components = new std::map<component::type, std::list<component::base *> >;
		
		if (removed != 0) delete removed;
		removed = new std::list<component::base *>;

		if (ofactory != 0) delete ofactory;
		if (cfactory != 0) delete cfactory;
		cfactory = new component::factory;
		ofactory = new object::factory(*cfactory);
		initialized = true;
		started = false;
	}

	void engine::load(std::string configfile) {
		init();
		
		std::ifstream fin(configfile.c_str());
		
		/* initialize yaml parser */
		YAML::Parser parser(fin);
		YAML::Node node;
		
		/* TODO: add other yaml features */
		while (parser.GetNextDocument(node)) node >> *config;
		
		/* get component-search path */
		std::string compath = (*config)["compath"];
		if (compath == "") {
			std::cerr << "Gear2D: Error locating system-wide component path." << std::endl;
		}
		
		cfactory->compath = compath;
		
		/* load the indicated objects */
		std::vector<object::type> objectlist = split((*config)["objects"]);
		
		for (int i = 0; i < objectlist.size(); i++) {
			ofactory->load(objectlist[i]);
			ofactory->build(objectlist[i]);
		}
	}
	
	bool engine::run() {
		// make sure we init
		init();
		timediff begin = 0, end = 0, dt = 0;
		started = true;
		while (started) {
			dt = end - begin;
			begin = SDL_GetTicks();
			/* first remove components from the running pipeline */
			for (std::list<component::base *>::iterator i = removed->begin(); i != removed->end(); i++) {
				component::type t = (*i)->type();
				(*components)[t].remove((*i));
				
				// do not manage empty lists
				if ((*components)[t].size() == 0)
					components->erase(t);
			}
			
			/* now update pipeline accordingly */
			std::map<component::type, std::list<component::base *> >::iterator comtpit;
			for (comtpit = components->begin(); comtpit != components->end(); comtpit++) {
				component::type t = comtpit->first;
				std::list<component::base *> & list = comtpit->second;
				for (std::list<component::base*>::iterator comit = list.begin(); comit != list.end(); comit++) {
					(*comit)->update(dt);
				}
			}
			
			/* no components, quit the engine */
			if (components->empty()) started = false;
			
			SDL_Delay(1);
			end = SDL_GetTicks();
		}
	}
}