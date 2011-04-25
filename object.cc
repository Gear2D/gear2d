#include <fstream>
#include "object.h"
#include "component.h"
#include "engine.h"
#include "yaml-cpp/yaml.h"

namespace gear2d {
	object::object() { 
		destroyed = false;
	}
	
	object::~object() {
		// delete all components
		for (componentcontainer::iterator i = components.begin(); i != components.end(); i++) {
			component::base * c = i->second;
			components.erase(i);
			delete c;
		}
		
		// delete all properties
		for (parameterbase::table::iterator i = parameters.begin(); i != parameters.end(); i++) {
			parameterbase * p = i->second;
			parameters.erase(i);
			if (p->dodestroy) delete p;
		}
	}
	
	object::id object::oid() { return this; }
	
	void object::attach(component::base * newc) {
		if (newc == 0) return;
		component::base * oldc = deattach(newc->type());
		if (oldc != 0) { delete oldc; }
		newc->owner = this;
		components[newc->type()] = newc;
		newc->init();
		engine::add(newc);
	}
	
	component::base * object::deattach(component::type type) {
		component::base * oldc = components[type];
		if (oldc == 0) return 0;
		components.erase(type);
		engine::remove(oldc);
		return oldc;
	}

	parameterbase::value & object::access(parameterbase::id pid) {
		return parameters[pid];
	}
	
	void object::destroy() {
		destroyed = true;
	}
	
	/* -- factory methods */
	object::factory::factory(component::factory & cfactory) : cfactory(cfactory) {

	}
	
	void object::factory::load(object::type objtype, std::string filename) {
		/* open the file */
		if (filename == "") filename = objtype + ".yaml";
		std::ifstream fin(filename.c_str());
		
		/* initialize yaml parser */
		YAML::Parser parser(fin);
		YAML::Node node;
		
		signature & sig = signatures[objtype];
		while (parser.GetNextDocument(node)) node >> sig;
		
		/* check if initial table wasn't loaded */
		if (inittable.size() == 0 && commonsig.size() != 0) {
			/* create a initial param table based on given signature */
			for (object::signature::iterator sigit = commonsig.begin(); sigit != commonsig.end(); sigit++) {
				inittable[sigit->first] = new parameter<std::string>(sigit->second);
			}
		}
	}
	
	void object::factory::set(object::type objtype, object::signature sig) {
		signatures[objtype] = sig;
		return;
	}

	object * object::factory::build(object::type objtype) {
		map<object::type, object::signature>::iterator sigit;
		
		/* this type has no sig at all */
		if ((sigit = signatures.find(objtype)) == signatures.end()) return new object();
		/* from this point we assume sigit is valid */
		
		/* get the signature */
		object::signature & sig = sigit->second;
		object::signature::iterator it;
		
		/* creates a new object */
		object * obj = new object();
		
		/* list of components to be attached when parameter loading is done */
		std::list<component::base *> toattach;
		
		/* search for a list of components */
		if ((it = sig.find("attach")) != sig.end()) {
			/* check its not empty */
			if (it->second != "") {
				
				/* obtain a vector of attachables */
				std::vector<std::string> attachables = split(it->second);
				
				/* now instantiate each component */
				for (unsigned int i = 0; i < attachables.size(); i++) {
					
					/* get the factory to build this component */
					component::type comtype(attachables[i]);
					component::base * c = cfactory.build(comtype);
					
					/* maybe we need to load it... */
					if (c == 0) {
						cfactory.load(comtype);
						c = cfactory.build(comtype);
					}
					
					/* if c continues to be 0, ignore it. */
					if (c == 0) continue;
					
					/* access its table of default template parameters */
					const parameterbase::table & deftable = c->parameters();
					
					/* clone each parameter */
					for (parameterbase::table::const_iterator deftit = deftable.begin(); deftit != deftable.end(); deftit++) {
						parameterbase::value & v = obj->access(deftit->first);
						
						/* we should only set the parameter if not already set by another component */
						if (v == 0) {
							/* clone from the default table */
							v = deftit->second->clone();
							/* also, set the desired pid */
							v->pid = deftit->first;
						}
					}
					
					/* Mark it to be attached; */
					toattach.push_back(c);
					
				} /* end of component loading and parametering */
			
				/* component-needed parameters are loaded. Load the ones not specified, as strings */
				for (it = sig.begin(); it != sig.end(); it++) {
					/* obtain the pointer */
					parameterbase::value & v = obj->access(it->first);
					
					/* if its not defined then we will define it as string */
					if (v == 0)
						v = new parameter<std::string>;
					
					/* set it with the signature */
					v->set(it->second);
					v->pid = it->first;
				}				
				
				/* at last, set global parameters */
				for (parameterbase::table::iterator initit = inittable.begin(); initit != inittable.end(); initit++) {
					obj->access(initit->first) = initit->second;
					obj->access(initit->first)->pid = initit->first;
				} /* end of global parameters */
			}
		} /* end of attachables processing */
		
		/* now attach each of them */
		while (!toattach.empty()) {
			component::base * c = toattach.front();
			toattach.pop_front();
			
			/* attach should take care of c->init() */
			obj->attach(c);
		}
		
		/* finally, object is built */
		return obj;
	}
}