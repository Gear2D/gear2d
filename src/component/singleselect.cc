/**
 * @file singleselect.cc
 * @author Leonardo Guilherme de Freitas
 * @b family menu
 * @b type singleselect
 * @addtogroup menu
 * 
 * Defines a single-option menu. A single option
 * menu lets you define ordered options with identifcators.
 * It handles iteration, focus, selection.
 * 
 * @b provides
 * @li @c menu.options List of identificators for menu options @b string
 * @li @c menu.circular Shall this menu be treated as a circular menu (last->first->last). Defaults to false. @b bool
 * @li @c menu.next Touch to make focus go to the next option @b bool
 * @li @c menu.prev Touch to make focus go to the previous option @b bool
 * @li @c menu.focus Focused option. This will be set to the focused object. You can set it to focus an arbitrary object. @b string
 * @li @c menu.last Last option that was focused. @b string
 * @li @c <id>.order Order of this component for iteration @b integer
 * @li @c <id>.focused Changed to true when it got focus, false when not. The option with lowest order defaults to 1, others to 0. @b boolean
 */

#include "gear2d.h"
using namespace gear2d;
using namespace std;


class singleselect: public component::base {
	private:
		struct option;
		typedef pair<int, option *> ordered;
		
		struct option {
			string id;
			int order;
			int oldorder;
			bool focused;
			bool select;
			set<ordered>::iterator itr;
			option() 
			 : id("")
			 , order(0)
			 , oldorder(order)
			 , focused(false)
			 , select(false) {
				
			 }
		};
		
		
		map<string, option *> optionbyid;
		set<ordered> optionbyorder;
		option * focused;
		bool circular;
		bool initialized;
		
	public:
		singleselect() : focused(0) { }
		virtual component::family family() { return "menu"; }
		virtual component::type type() { return "singleselect"; }
		
		virtual void handle(parameterbase::id pid, component::base * lastw, object::id owns) {
			int p;
			if (pid == "menu.options") {
				loadoptions(raw<string>(pid));
				return;
			}
			else if (pid == "menu.focus") {
				string focusid = raw<string>("menu.focus");
				cout << "the focus is changing to " << focusid << endl;
				if (focusid == "") focusid = optionbyorder.begin()->second->id;
				option * opt = optionbyid[focusid];
				// do not touch it again...
				if (opt == 0 || opt == focused) return;
				if (focused != 0) {
					write(focused->id + ".focused", false);
					write("menu.last", focused->id);
				}
				focused = opt;
				write(focused->id + ".focused", true);
			}
			else if (pid == "menu.next") {
				set<ordered>::iterator itr = focused->itr;
				if (itr != optionbyorder.end()) itr++;
				if (itr == optionbyorder.end()) {
					if (circular) itr = optionbyorder.begin();
					else itr--;
				}
				write("menu.last", focused->id);
				write("menu.focus", itr->second->id);
			}
			else if (pid == "menu.prev") {
				set<ordered>::iterator itr = focused->itr;
				if (itr == optionbyorder.begin()) {
					if (circular) itr = optionbyorder.end();
					else itr++;
				}
				itr--;
				write("menu.last", focused->id);
				write("menu.focus", itr->second->id);
			}
			else
			if ((p = pid.find(".focused")) != parameterbase::id::npos) {
				// we set this ourselves, no need to re-run.
				if (lastw == this) return;
				string oid = pid.substr(0, p);
				option * opt = optionbyid[oid];
				if (opt == 0) return;
				if (opt->focused == true) {
					// we got focused but game do not know yet
					if (focused != opt) write("menu.focus", opt->id);
				}
			}
			else
			if ((p = pid.find(".order")) != parameterbase::id::npos) {
				string oid = pid.substr(0, p);
				option * opt = optionbyid[oid];
				if (opt != 0) {
					optionbyorder.erase(ordered(opt->oldorder, opt));
					opt->oldorder = opt->order;
					
					// insert there and update iterator
					pair<set<ordered>::iterator, bool> success = optionbyorder.insert(ordered(opt->order, opt));
					if (success.second) {
						// update iterator
						opt->itr = success.first;
						cout << "success! " << optionbyorder.size() << " ";
					}
				}
				cout << "ordering changes: " << oid << " to " << opt->order << endl;
				return;
			}
		}
		virtual void setup(object::signature & sig) {
			loadoptions(sig["menu.options"]);
			write<string>("menu.options", sig["menu.options"]);
			hook("menu.options");
			for (map<string, option *>::iterator i = optionbyid.begin(); i != optionbyid.end(); i++) {
				option * opt = i->second;
				string id = i->first;
				write(id + ".order", eval(sig[id + ".order"], 0));
				write(id + ".focused", eval(sig[id + ".focus"], false));
				cout << "option " << id << " was set up with " << opt->focused << " as focused " << opt->order << " as order " << endl;
			}
			string focus = sig["menu.focus"];
			write("menu.focus", string(""));
			write("menu.last", string(""));
			hook("menu.focus");
			if (focus == "") {
				if (optionbyorder.size() != 0) {
					focus = optionbyorder.begin()->second->id;
				}
			}
			write<string>("menu.focus", focus);
			bind("menu.circular", circular);
			circular = eval(sig["menu.circular"], false);
			write("menu.next", false);
			write("menu.prev", false);
			hook("menu.next");
			hook("menu.prev");
			initialized = false;
		}
		
		virtual void update(float dt) {
			// just to pull first selection
			if (!initialized) { write("menu.focus", raw<string>("menu.focus")); initialized = true; }
		}

	private:
		void loadoptions(const set<string> & ids) {
			for (set<string>::iterator id = ids.begin(); id != ids.end(); id++) {
				loadoption(*id);
			}
		}
		void loadoptions(const string & optline) {
			set<string> ids;
			if (optline != "") {
				split(ids, optline, ' ');
				loadoptions(ids);
			}
		}
		void loadoption(string id) {
			// do not rewrite an option.
			if (optionbyid[id] != 0) return;
			option * opt = new option();
			opt->id = id;
			bind(id + ".order", opt->order);
			hook(id + ".order");
			bind(id + ".focused", opt->focused);
			hook(id + ".focused");
			bind(id + ".select", opt->select);
			hook(id + ".select");
			optionbyid[id] = opt;
		}
};

extern "C" {
	component::base * build() { return new singleselect; }
}
