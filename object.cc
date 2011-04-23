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
			delete p;
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
		;
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
		
		/* search for a list of components */
		if ((it = sig.find("attach")) != sig.end()) {
			/* check its not empty */
			if (it->second != "") {
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
					
					/* if c continues to be 0 */
					if (c == 0) continue;
					
					/* access its table of default parameters */
					const parameterbase::table & deftable = c->parameters();
					
					/* clone each parameter */
					for (parameterbase::table::const_iterator deftit = deftable.begin(); deftit != deftable.end(); deftit++) {
						parameterbase::value & v = obj->access(deftit->first);
						
						/* we should only set the parameter if not already set by another component */
						if (v == 0) {
							v = deftit->second->clone();
							/* also, set the desired pid */
							v->pid = deftit->first;
						}
					}
					
					/* now that its needed parameters are there, attach it */
					obj->attach(c);
				}
			}
		} /* end of component attaching loop */
		
		/* now that parameters are in its correct type, setup with object-defined ones from sig */
		for (it = sig.begin(); it != sig.end(); it++) {
			parameterbase::value & v = obj->access(it->first);
			
			/* if its not defined then we will define it as string */
			if (v == 0)
				v = new parameter<std::string>;
			
			/* set it with the signature */
			v->set(it->second);
		}
		
		/* finally, object is built */
		return obj;
	}
}