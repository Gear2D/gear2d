#include "engine.h"
#include "component.h"
#include "object.h" 
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <ctime>
#include "SDL.h" // for the timer

#include <algorithm>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
using boost::algorithm::split;
using boost::algorithm::is_any_of;


namespace gear2d {
	std::map<std::string, std::string> * engine::config;
	component::factory * engine::cfactory;
	object::factory * engine::ofactory;
	std::map<component::type, std::list<component::base *> > * engine::components;
	std::list<component::base *> * engine::removedcom;
	std::set<object::id> * engine::destroyedobj;
	bool engine::initialized;
	bool engine::started;
	
	
	void engine::add(component::base * c) {
		if (components == 0) {
			std::cerr << "(Gear2D Engine) Initialize the engine before attaching a component to an object" << std::endl;
		}
		if (c == 0) return;
		(*components)[c->family()].push_back(c); 
	}

	void engine::remove(component::base * c, bool rightnow) {
		if (rightnow == false) removedcom->push_back(c);
		else {
			(*components)[c->family()].remove(c);
			c->owner = 0;
			delete c;
		}
	}
	
	void engine::destroy(object::id oid) {
		destroyedobj->insert(oid);
	}
	
	void engine::init(bool force) {
		if (initialized == true && force == false) return;
		if (config != 0) delete config;
		srand(std::time(0));
		
		config = new std::map<std::string, std::string>;
		
		if (components != 0) delete components;
		components = new std::map<component::family, std::list<component::base *> >;
		
		if (removedcom != 0) delete removedcom;
		removedcom = new std::list<component::base *>;
		
		if (destroyedobj != 0) delete destroyedobj;
		destroyedobj = new std::set<object::id>;

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
			std::cerr << "(Gear2D Engine) Error locating system-wide component path." << std::endl;
		}
		
		cfactory->compath = compath;
		config->erase("compath");
		
		/* pre-load some of the components */
		/* TODO: travel the compath looking for family/component */
		std::vector<std::string> comlist;
		split(comlist, (*config)["compreload"], is_any_of(" "));
		for (int i = 0; i < comlist.size(); i++) {
			std::cerr << "(Gear2D Engine) Pre-loading " << comlist[i] << std::endl;
			cfactory->load(comlist[i]);
		}
		config->erase("compreload");
		
		/* load the indicated objects */
		std::vector<object::type> objectlist;
		split(objectlist, (*config)["objects"], is_any_of(" "));
		config->erase("objects");
		
		/* The rest is added to the object factory global parameters */
		ofactory->commonsig = *config;
		
		/* and now build the pointed objects */
		for (int i = 0; i < objectlist.size(); i++) {
			ofactory->load(objectlist[i]);
			ofactory->build(objectlist[i]);
		}
	}
	
	bool engine::run() {
		// make sure we init
		init();
		int begin = 0, end = 0, dt = 0;
		
		started = true;
		while (started) {
			dt = end - begin;
			begin = SDL_GetTicks();
			timediff delta = dt/1000.0f;
			
			for (std::set<object::id>::iterator i = destroyedobj->begin(); i != destroyedobj->end(); i++) {
				// this will push object's components to removedcom, hopefully.
				delete (*i);
			}
			
			destroyedobj->clear();
			
			/* first remove components from the running pipeline */
			for (std::list<component::base *>::iterator i = removedcom->begin(); i != removedcom->end(); i++) {
				component::type f = (*i)->family();
				(*components)[f].remove((*i));
				
// 				(*i)->owner = 0;
				delete (*i);
				
				// do not manage empty lists
				if ((*components)[f].size() == 0)
					components->erase(f);
			}
			
			// clear the removed list
			removedcom->clear();
			
			static char omg[200];
			static std::string wm;
			wm = "";
			/* now update pipeline accordingly */
			std::map<component::family, std::list<component::base *> >::iterator comtpit;
			for (comtpit = components->begin(); comtpit != components->end(); comtpit++) {
				component::family f = comtpit->first;
				std::list<component::base *> & list = comtpit->second;
				int b = SDL_GetTicks();
				for (std::list<component::base*>::iterator comit = list.begin(); comit != list.end(); comit++) {
					(*comit)->update(delta);
				}
				sprintf(omg, "%s:%.4d ", comtpit->first.c_str(), SDL_GetTicks() - b);
				wm += omg;
			}
			sprintf(omg, "total (last): %.3d", dt);
			wm += omg;
			/* no components, quit the engine */
			if (components->empty()) started = false;
			SDL_Delay(2);
 			SDL_WM_SetCaption(wm.c_str(), 0);
			end = SDL_GetTicks();
		}
	}
}