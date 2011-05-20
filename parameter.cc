#include "parameter.h"
#include "component.h"
namespace gear2d {
	void parameterbase::hook(component::base * c) {
		hooked.insert(c);
	}
	
	void parameterbase::pull() {
		for (std::set<component::base *>::iterator i = hooked.begin(); i != hooked.end(); i++) {
			component::base * c = *i;
			c->handle(pid, lastwrite);
		}
	}
}